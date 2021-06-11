#include "JobSystem.h"
#include <iostream>
#include <immintrin.h>

JobSystem::JobSystem()
{
	_numThreads = 0;
	_currentLabel = 0;
}

JobSystem::~JobSystem() {}

void JobSystem::Initialize()
{
	_finishedLabel.store(0);

	// Number of concurrent threads supported.
	// If the value is not well defined or not computable, returns 0
	unsigned int numCores = std::thread::hardware_concurrency();
	_numThreads = std::max(1u, numCores);

    std::cout << "Num threads : " << _numThreads << std::endl;
    // Create all our worker threads while immediately starting them:
	for (uint32_t threadID = 0; threadID < _numThreads; ++threadID)
	{
        std::thread worker(&JobSystem::FoundJob, this);
        // forget about this thread, 
        // let it do it's job in the infinite loop that we created in FoundJob
        worker.detach();
	}
}

void JobSystem::Execute(const std::function<void()>& job)
{
    //the main thread label is updated
    _currentLabel += 1;

    //try to push a new job until it is pushed successfuly
    while (!_jobPool.push_back(job))
        Poll();

    _wakeCondition.notify_one();
}

void JobSystem::Wait()
{
    while (IsBusy())
        Poll();
}

bool JobSystem::IsBusy()
{
	return _finishedLabel.load() < _currentLabel;
}

void JobSystem::FoundJob()
{
    std::function<void()> job; // the current job for the thread, it's empty at start.

    // This is the infinite loop that a worker thread will do 
    while (true)
    {
        if (_jobPool.pop_front(job)) // try to grab a job from the jobPool queue
        {
            // It found a job, execute it:
            job(); // execute job
            _finishedLabel.fetch_add(1); // update worker label state
        }
        else
        {
            // no job, put thread to sleep
            std::unique_lock<std::mutex> lock(_wakeMutex);
            _wakeCondition.wait(lock);
        }
    }
}

void JobSystem::Poll()
{
    //Wake one worker thread
    _wakeCondition.notify_one();

    //Allow this thread to be rescheduled
    std::this_thread::yield();
    //_mm_pause(); //better than yield
}
