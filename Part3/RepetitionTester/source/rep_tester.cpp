
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
	char* name;
	u64 bytes_to_test;
	u64 initial_time;
	u64 scope_time;
	f64 current_time;
	f64 best_time;
};

internal void
InitTester(repetition_tester *tester, u8 test_time, u64 Bytes, char* const name)
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
	u64 new_time = ReadOSTimer();
	u64 time = new_time - tester->scope_time;
	
	u64 OSTimeStampCounterTimerFrequency = GetOSTimerFrequency();
	
	// TODO convert this to ms and use it.
	f64 time_passed_ms = ((f64)time / OSTimeStampCounterTimerFrequency) * 1000;
	
	if(time_passed_ms < tester->best_time || tester->best_time == 0)
	{
		printf("Detected a better time %s prev: %.8f now: %.8f \n ",tester->name, tester->best_time, time_passed_ms);
		tester->best_time = time_passed_ms;
	}
	
	tester->current_time += time_passed_ms;
	
	printf("Total time passed: %.8f \n", tester->current_time / 1000);
	// Check if we should keep testing
	if (tester->current_time >= (f64)tester->test_time * 1000)
	{
		tester->b_is_testing = false;
	}
}



