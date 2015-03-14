#include "Threading.h"

#include <thread>
#include <mutex>

#include <vector>

#include <cstdio>

Threading::Threading(){}
Threading::~Threading(){}

bool Threading::Startup()
{
	RunThreadingTut();   //run actual program as is not needing opengl

	return true;
}
void Threading::Shutdown(){}

bool Threading::Update(){return true;}		/////
void Threading::Draw(){}				   ///// not needed

void print(int i)
{
	static std::mutex myMutex;
	std::lock_guard<std::mutex> guard(myMutex);

	printf("Hello Thread %i\n", i);
	printf("I'm here....\n");
	printf("....Not here\n");

	

}

void Threading::RunThreadingTut()
{
	
	////start another thread
	//std::thread myThread(print);
	//
	////waits until the thread has ended
	//myThread.join();

	std::vector<std::thread> threads;
	std::mutex myMutex;

	for (int i = 0; i < 50; i++)
	{
		threads.push_back(std::thread(
			//lambda
			[&myMutex](int i){
			std::lock_guard<std::mutex> guard(myMutex);
			printf("Hello Thread %i\n", i);
		}, i
		));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

}

