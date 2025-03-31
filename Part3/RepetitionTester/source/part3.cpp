
#include "types.h"
#include <cstdio>

#include "rep_tester.cpp"
#include "ishak_buffer.cpp"
#include "sys/stat.h"

struct read_params
{
	buffer buff;
	char const* file_name;
};

#include <mach/mach.h>

void getPageFaults() {
    task_events_info_data_t info;
    mach_msg_type_number_t count = TASK_EVENTS_INFO_COUNT;
    mach_port_t task = mach_task_self();
    
    if (task_info(task, TASK_EVENTS_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) 
    {
        printf("Page faults : %i \n", info.faults);
    } 
    else 
    {
    }
}

int main (int args_num, const char** args)
{
	static repetition_tester tester;
    static repetition_tester tester1;
	
	//// Fread /////
	read_params params;
	params.file_name = "input.json";
	FILE* file = fopen(params.file_name, "rb");
	if (file == 0)
	{
		printf("Unable to read the file \n");
		return 1;
	}
	
#if _WIN32
	struct _stat64 Stat;
	_stat64(params.file_name, &Stat);
#else
	struct stat Stat;
	stat(params.file_name, &Stat);
#endif
	//66609763 - page faults with malloc in the repetition test
    //275746 - page faults without malloc in the repetition test
	params.buff = AllocateBuffer(Stat.st_size);
	InitTester(&tester, 10, params.buff.Size, "test_scope");
    
	tester.cpu_frequency = GetOSTimerFrequency();
	while(tester.b_is_testing)
	{
		BeginTimer(&tester);
		fread(params.buff.Bytes, sizeof(u8), params.buff.Size, file);
		EndTimer(&tester);
		if(tester.times == 2)
        {
            PrintStatus(&tester);
        }
	}
    
    getPageFaults();
    PrintStatus(&tester);
    
    fclose(file);
    
    return 0;
};