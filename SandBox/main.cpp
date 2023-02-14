#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

struct SharedResource
{
	SharedResource() :
		cv_mutex(), cv(), counter(0)
	{

	}

	/*
	This mutex is used for three purposes:
	1) to synchronize accesses to counter
	2) to synchronize accesses to std::cerr
	3) for the condition variable cv
	*/
	std::mutex cv_mutex;
	std::condition_variable cv;
	int counter;
};


void waits(SharedResource& sharedRes)
{
	bool status{};
	constexpr size_t timeOut = 4;
	std::unique_lock<std::mutex> lk(sharedRes.cv_mutex);
	std::cerr << "Thread ID: " << std::this_thread::get_id() << " Waiting... \n";
	//The predicate version (2) returns pred() (the return value of pred()), 
	//regardless of whether the timeout was triggered (although it can only be false if triggered).
	while (!(status = sharedRes.cv.wait_for(lk, std::chrono::seconds(timeOut), [&]()->bool { return (sharedRes.counter==1); })))
	{
		std::cerr << " I  woke up every " << timeOut << " seconds\n";
		std::cerr << " my status is: " << status << "\n";
	}
	
	std::cerr << "Thread ID: " << std::this_thread::get_id() << "...finished waiting." << "counter: " << sharedRes.counter << std::endl;
	std::cerr << " my status is: " << status << "\n";
}

void signals(SharedResource& sharedRes)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	{
		std::lock_guard<std::mutex> lk(sharedRes.cv_mutex);
		std::cerr << "Notifying...\n";
	} // The lk object will be unlocked after this scope ends.

	sharedRes.cv.notify_all();

	std::this_thread::sleep_for(std::chrono::seconds(8));

	{
		std::lock_guard<std::mutex> lk(sharedRes.cv_mutex);
		sharedRes.counter = 1;
		std::cerr << "Notifying again...\n";

	}// The lk object will be unlocked after this scope ends.
	sharedRes.cv.notify_one();

}

int main()
{
	SharedResource sharedRes;

	std::thread
		t1(waits, std::ref(sharedRes)),
		t2(waits, std::ref(sharedRes)),
		t3(waits, std::ref(sharedRes)),
		t4(signals, std::ref(sharedRes));
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

