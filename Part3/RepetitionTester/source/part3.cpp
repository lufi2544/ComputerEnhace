
#include "types.h"
#include <cstdio>
#include "sys/stat.h"

#include "rep_tester.cpp"
#include "ishak_buffer.cpp"
#include "memory_pointer_observer.cpp"
#include "circular_buffer.cpp"

struct read_params
{
	buffer buff;
	char const* file_name;
};

int main (int args_num, const char** args)
{
	/*static repetition_tester tester;
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
	InitTester(&tester, 10, Stat.st_size, "test_scope");
    
	tester.cpu_frequency = GetOSTimerFrequency();
	while(tester.b_is_testing)
	{
		BeginTimer(&tester);
		fread(params.buff.Bytes, sizeof(u8), params.buff.Size, file);
		EndTimer(&tester);
		if(tester.times == 2)
        {
            //PrintStatus(&tester);
        }
	}
    
    PrintStatus(&tester);
    
    fclose(file);*/
    
    
    //InitPageTouchingTest(4096);
    
    
    // void* data = memory_ptr_test();
    
    int to_map = (getpagesize() * 4) / sizeof(int);
    int pages;
    u8*base = (u8*)init_circular_buffer_test(to_map * sizeof(int), &pages);
    base[0] = 122;
    for(int i = 0; i < pages; ++i)
    {
        u8 *it = base + (getpagesize() * i);
        printf("Page %i : %i \n", i, it[0]);
    }
    
    return 0;
    
};