

#include <iostream>
#include <string>
#include "TestInput.h"
#include "CJumpCloudJsonStats.h"
#include <thread>
#include <chrono> //for sleep yuck!

using std::string;

CJumpCloudJsonStats g_multiThreadTest(g_szProvidedExample1);

//multithread test helpers
void addActionsMultiThreadTest()
{
	//Ideally as time would allow I would use some eventing/wait to simulate thread processing
	std::this_thread::sleep_for(std::chrono::seconds(5));
	g_multiThreadTest.addAction(g_szProvidedExample2);
	g_multiThreadTest.addAction(g_szProvidedExample3);
}

void printAvgMultiThreadTest()
{
	g_multiThreadTest.getStats();
}


//main will test CJumpCloudJsonStats
int main()
{
	CJumpCloudJsonStats stats(g_szProvidedExample1);
	uint32_t jumpCnt;

	//test check initial insert
	if (stats.getActionCountByActionName("jump", jumpCnt) == OK)
	{
		if (jumpCnt != 1)
		{
			std::cout << "ERROR : jumpCnt was incorrect\n";
		}
		else
		{
			std::cout << "SUCCESS : jumpCnt was correct \n";
		}
	}
	else
	{
		std::cout << "ERROR could not find jump in hashmap\n";
	}

	uint32_t jumpTime;
	if (stats.getActionTimeByActionName("jump", jumpTime) == OK)
	{
		if (jumpTime != 100)
		{
			std::cout << "ERROR : incorrect jump time was returned\n";
		}
		else
		{
			std::cout << "SUCCESS : jump time was correct \n";
		}
	}
	else
	{
		std::cout << "ERROR : jump action not found\n";
	}

	uint32_t jumpAvg;
	if (stats.getActionAvgByActionName("jump", jumpAvg) == OK)
	{
		if (jumpAvg != 100)
		{
			std::cout << "ERROR : incorrect jump avg was returned\n";
		}
		else
		{
			std::cout << "SUCCESS : jump avg was correct \n";
		}
	}
	else
	{
		std::cout << "ERROR : jump action not found\n";
	}
	

	//test action that is updated
	if (stats.addAction(g_szProvidedExample2) != OK)
	{
		std::cout << "ERROR :  adding " << g_szProvidedExample2 << " with addAction\n";
	}
	else
	{
		if (stats.addAction(g_szProvidedExample3) != OK)
		{
			std::cout << "ERROR :  adding " << g_szProvidedExample3 << " with addAction\n";
		}
		else
		{
			uint32_t walkCnt, walkTime, walkAvg;
			if (stats.getActionCountByActionName("walk", walkCnt) != OK)
			{
				std::cout << "ERROR : failed to find walk\n";
			}
			else
			{
				if (walkCnt != 2)
				{
					std::cout << "ERROR : incorrect walk count\n";
				}
				else
				{
					std::cout << "SUCCESS : correct walk count\n";
				}
			}
			if (stats.getActionAvgByActionName("walk", walkAvg) != OK)
			{
				std::cout << "ERROR : failed to find walk\n";
			}
			else
			{
				if (walkAvg != 125)
				{
					std::cout << "ERROR : incorrect walk avg\n";
				}
				else
				{
					std::cout << "SUCCESS : correct walk avg\n";
				}
			}
			if (stats.getActionTimeByActionName("walk", walkTime) != OK)
			{
				std::cout << "ERROR : failed to find walk\n";
			}
			else
			{
				if (walkTime != 250)
				{
					std::cout << "ERROR : incorrect walk time\n";
				}
				else
				{
					std::cout << "SUCCESS : correct walk time\n";
				}
			}
		}
	}

	//test for case when there is no Action - expect NOTFOUND to be returned
	uint32_t fooSum;
	if (stats.getActionCountByActionName("foo", fooSum) != NOTFOUND)
	{
		std::cout << "ERROR : expected null \n";
	}
	else
	{
		std::cout << "SUCCESS : returned correct error code\n";
	}


	//todo add more validation.  C++ does not natively support JSON and per guidance
	//I limited dependencies but ideally there would more validation
	//I added more validation functions in CJumpCloudJsonStats to offset the limitations
	//due to no native support of Json in C++
	std::string avgStr = stats.getStats();
	if (avgStr.empty() || avgStr.size() == 0)
	{
		std::cout << "ERROR : getStats failed to return a valid string\n";
	}

	
	//test case : validate invalid Json return INVALIDJSON
	if (stats.addAction("baddata") != INVALIDJSON)
	{
		std::cout << "ERROR : expected INVALIDJSON but returned incorrect JumpCloudJsonState\n";
	}
	else
	{
		std::cout << "SUCCESS : correct return value returned for invalid JSON\n";
	}

	//simple multithread test cases to confirm CS/mutex are working.
	//todo with more time add some eventing to avoid using sleeps
	std::thread addActionThread(addActionsMultiThreadTest);
	std::thread printAvgThread(printAvgMultiThreadTest);

	printAvgThread.join();
	if (g_multiThreadTest.getAllActionCount() != 1)
	{
		std::cout << "ERROR : item count expected to be 1 but was not\n";
	}
	else
	{
		std::cout << "SUCCESS : multithread test had correct number of items in actionHash\n";
	}

	addActionThread.join();
	if (g_multiThreadTest.getAllActionCount() != 2)
	{
		std::cout << "ERROR : item count expected to be 2 but was not\n";
	}
	else
	{
		std::cout << "SUCCESS : multithread test part 2 had correct number of items in actionHash\n";
	}
	
}


