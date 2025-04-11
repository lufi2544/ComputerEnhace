
#include "Profiling/source/cpu_defines_single.cpp"

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/mman.h>
#include <unistd.h>
#endif // __APPLE__



//////////
//The intention of this object is to test different functionalities 
// multiple times and find the best time possible for that process
/////////

//ugh..
struct repetition_tester
{
	repetition_tester() = default;
    
    bool b_is_testing;
	u8 test_time;
	const char* name;
	u64 bytes_to_test;
	u64 initial_time;
	u64 scope_time;
	f64 current_time;
	f64 best_time;
	f64 best_time_ts;
	f64 worst_time;
	f64 worst_time_ts;
	u64 cpu_frequency;
    u64 times_best;
    u64 times_worst;
    u64 times;
    
    u64 page_faults_start;
    u64 page_faults_hard_start;
    u64 page_faults_begin_cycle;
    u64 page_faults_hard_begin_cycle;
    
    u64 page_faults_start_best;
    u64 page_faults_start_worst;
    u64 page_faults_hard_start_best;
    u64 page_faults_hard_start_worst;
};

#ifdef __APPLE__

internal_f 
void get_page_faults(u64 *o_page_faults, u64 *o_page_faults_hard)
{
    task_events_info_data_t info;
    mach_msg_type_number_t count = TASK_EVENTS_INFO_COUNT;
    mach_port_t task = mach_task_self();
    
    if (task_info(task, TASK_EVENTS_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) 
    {
        *o_page_faults = info.faults;
        *o_page_faults_hard = info.pageins;
    }
    else 
    {
        *o_page_faults = 0;
        *o_page_faults_hard = 0;
    }
}

internal_f u64
get_memory_page_size()
{
    return getpagesize();
}

void InitPageTouchingTest(u32 _page_count)
{
    u64 page_size = get_memory_page_size();
    u64 total_size = _page_count * page_size;
    
    printf("Page Count, Touch Count, Fault Count, Extra Faults\n");
    
    for(u64 touch_count = 0; touch_count < _page_count; ++touch_count)
    {
        u64 touch_size = page_size * touch_count;
        u8 *data = (u8*)mmap(0, total_size, PROT_READ | PROT_WRITE,MAP_SHARED | MAP_PRIVATE | MAP_ANON, -1, 0);
        if (data)
        {
            u64 start_faults, start_hard;
            get_page_faults(&start_faults, &start_hard);
            for (u64 i = 0; i < touch_size; ++i)
            {
                data[total_size - 1 - i] = (u8)i;
            }
            u64 end_faults, end_hard;
            get_page_faults(&end_faults, &end_hard);
            u64 faults = end_faults - start_faults;
            printf("%u, %llu, %llu, %lld\n", _page_count, touch_count, faults, (faults - touch_count));
            
            munmap(data, total_size);
        }
    }
}


#else // __APPLE__

internal_f void
get_page_faults(u64 *o_page_faults, u64 *o_page_faults_hard)
{
    printf("get_page_faults not implemented for this platform");
}

internal_f u64
get_memory_page_size()
{
    printf("get_memory_page_size  not implemented for this platform");
    return 0;
}

void InitPageTouchingTest(u32 _page_count)
{
	
	printf("InitPageTouchingtest is not implemented for this Platform");	
}


#endif // Platforms implementation


internal_f void
InitTester(repetition_tester *tester, u8 test_time, u64 Bytes, const char* name)
{
	tester->test_time = test_time;
	tester->name = name;
	tester->bytes_to_test = Bytes;
	tester->b_is_testing = true;
	tester->initial_time = ReadOSTimer();
    get_page_faults(&tester->page_faults_start, &tester->page_faults_hard_start);
}

internal_f void
BeginTimer(repetition_tester *tester)
{
	tester->b_is_testing = true;
	tester->scope_time = ReadOSTimer();
    get_page_faults(&tester->page_faults_begin_cycle, &tester->page_faults_hard_begin_cycle);
}

internal_f void
EndTimer(repetition_tester *tester)
{
    tester->times++;
	// To be honest the time is not time, it is OS Timer Ticks.
    u64 new_time = ReadOSTimer();
    u64 time = new_time - tester->scope_time;
    
    u64 OSTimeStampCounterTimerFrequency = GetOSTimerFrequency();
    
    // Convert time to milliseconds
    f64 time_passed_ms = ((f64)time / OSTimeStampCounterTimerFrequency) * 1000;
    
    u64 page, hard;
    get_page_faults(&page, &hard);
    // Update best and worst time
    if (time_passed_ms < tester->best_time || tester->best_time == 0)
    {
        tester->best_time = time_passed_ms;
        tester->best_time_ts = time_passed_ms;
        tester->times_best = tester->times;
        tester->page_faults_start_best = page - tester->page_faults_begin_cycle;
        tester->page_faults_hard_start_best = hard - tester->page_faults_hard_begin_cycle;
    }
    
    if (time_passed_ms > tester->worst_time)
    {
        tester->worst_time = time_passed_ms;
        tester->worst_time_ts = time_passed_ms;
        tester->times_worst = tester->times;
        tester->page_faults_start_worst = page - tester->page_faults_begin_cycle;
        tester->page_faults_hard_start_worst = hard - tester->page_faults_hard_begin_cycle;
    }
	
    f64 total_time_elapsed =
		((f64)(new_time - tester->initial_time) / OSTimeStampCounterTimerFrequency);
	
    // Stop testing if we exceed the desired test time
    if (total_time_elapsed >= tester->test_time)
    {
        tester->b_is_testing = false;
    }
    
}

internal_f void 
PrintStatus(repetition_tester *_tester)
{
	f64 gb_tested = (f64)_tester->bytes_to_test / (1024.0 * 1024.0 * 1024.0);
	f64 best_time_seconds = (f64)_tester->best_time / 1000.0;  // Convert ms to seconds
	f64 worst_time_seconds = (f64)_tester->worst_time / 1000.0;
	
	printf("GB Tested %.4f", gb_tested);
	f64 gb_per_second_best = gb_tested / best_time_seconds;
	f64 gb_per_second_worst = gb_tested / worst_time_seconds;
    
    f64 kb_per_fault_best = ((f64)(gb_tested * 1024 * 1024)) / _tester->page_faults_start_best;
    f64 kb_per_fault_worst = ((f64)(gb_tested * 1024 * 1024)) / _tester->page_faults_start_worst;
    
	
	// Debug Output
	printf("GB: %.8f \n", gb_tested);
	printf("===== Test %s time: %i s bytes: %llu =====\n", _tester->name, _tester->test_time, _tester->bytes_to_test);
	printf("Best Time: %.8f sec, ts: %.8f, Bandwidth: %.8f GB/s, Times: %llu, Kb/Page Fault %.4f \n", best_time_seconds, _tester->best_time_ts / 1000, gb_per_second_best, _tester->times_best, kb_per_fault_best);
	printf("Worst Time: %.8f sec, ts: %.8f,  Bandwidth: %.8f GB/s,  Times: %llu,  Kb/Page Fault %.4f \n", worst_time_seconds, _tester->worst_time_ts / 1000,  gb_per_second_worst, _tester->times_worst, kb_per_fault_worst);
    
    u64 page_faults = 0; 
    u64 page_faults_hard = 0;
    get_page_faults(&page_faults, &page_faults_hard);
	printf("Memory Page Size: %llu Kb, Page faults: %llu , Page faults hard %llu \n", get_memory_page_size() / 1024, page_faults - _tester->page_faults_start, page_faults_hard - _tester->page_faults_hard_start);
	printf("================================== \n");
}
