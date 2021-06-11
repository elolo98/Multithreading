#pragma once

#include <functional>
#include <algorithm>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "ThreadSafeRingBuffer.h"

struct JobSystem
{
public:
	JobSystem();
	~JobSystem();

	void Initialize();
	void Execute(const std::function<void()>& job);
	void Wait();
	bool IsBusy();

private:

	void FoundJob();
	void Poll();

	uint32_t _numThreads;
	std::condition_variable _wakeCondition;
	std::mutex _wakeMutex;
	uint64_t _currentLabel;
	std::atomic<uint64_t> _finishedLabel;

	ThreadSafeRingBuffer<std::function<void()>, 256> _jobPool;
};