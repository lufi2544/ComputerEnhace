
#include "cpu_defines_single.cpp"

//////////
//The intention of this object is to test different functionalities 
// multiple times and find the best time possible for that process
/////////

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
	f64 worst_time;
	u64 cpu_frequency;
};

internal void
InitTester(repetition_tester *tester, u8 test_time, u64 Bytes, const char* name)
{
	tester->test_time = test_time;
	tester->name = name;
	tester->bytes_to_test = Bytes;
	tester->b_is_testing = true;
	tester->initial_time = ReadOSTimer();
}

internal void
BeginTimer(repetition_tester *tester)
{
	tester->b_is_testing = true;
	tester->scope_time = ReadOSTimer();
}

internal void
EndTimer(repetition_tester *tester)
{
	// To be honest the time is not time, it is OS Timer Ticks.
    u64 new_time = ReadOSTimer();
    u64 time = new_time - tester->scope_time;
    
    u64 OSTimeStampCounterTimerFrequency = GetOSTimerFrequency();
    
    // Convert time to milliseconds
    f64 time_passed_ms = ((f64)time / OSTimeStampCounterTimerFrequency) * 1000;
    
    // Update best and worst time
    if (time_passed_ms < tester->best_time || tester->best_time == 0)
    {
        tester->best_time = time_passed_ms;
    }
    
    if (time_passed_ms > tester->worst_time)
    {
        tester->worst_time = time_passed_ms;
    }
	
    f64 total_time_elapsed =
		((f64)(new_time - tester->initial_time) / OSTimeStampCounterTimerFrequency);
	
    // Stop testing if we exceed the desired test time
    if (total_time_elapsed >= tester->test_time)
    {
        tester->b_is_testing = false;
    }
}

internal void 
PrintStatus(repetition_tester *_tester)
{
	f64 gb_tested = (f64)_tester->bytes_to_test / (1024.0 * 1024.0 * 1024.0);
	f64 best_time_seconds = (f64)_tester->best_time / 1000.0;  // Convert ms to seconds
	f64 worst_time_seconds = (f64)_tester->worst_time / 1000.0;
	
	printf("GB Tested %.4f", gb_tested);
	f64 gb_per_second_best = gb_tested / best_time_seconds;
	f64 gb_per_second_worst = gb_tested / worst_time_seconds;
	
	// Debug Output
	printf("GB: %.8f \n", gb_tested);
	printf("===== Test %s time: %i s bytes: %llu =====\n", _tester->name, _tester->test_time, _tester->bytes_to_test);
	printf("Best Time: %.8f sec, Bandwidth: %.8f GB/s \n", best_time_seconds, gb_per_second_best);
	printf("Worst Time: %.8f sec, Bandwidth: %.8f GB/s \n", worst_time_seconds, gb_per_second_worst);
	printf("================================== \n");
}
