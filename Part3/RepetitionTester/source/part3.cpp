
#include "types.h"
#include <cstdio>

#include "rep_tester.cpp"

int main (int args_num, const char** args)
{
	
	int a = 0;
	
	static repetition_tester tester;
	InitTester(&tester, 1, 0, "test_scope");
	
	while(tester.b_is_testing)
	{
		BeginTimer(&tester);
		for(int a = 0; a < 10000; a++)
		{
			int b = a;
		}
		EndTimer(&tester);
		
	}
	
	return 0;
};