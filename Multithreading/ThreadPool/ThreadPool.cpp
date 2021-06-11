#include "ThreadPool.h"

ThreadPool::ThreadPool() : _done(false), _joiner(_threads)
{
	unsigned const threadCount = std::thread::hardware_concurrency();
	try
	{
		for (unsigned i = 0; i < threadCount; ++i)
		{
			_queues.push_back(std::unique_ptr<WorkStealingQueue>(
				new WorkStealingQueue));
		}
		for (unsigned i = 0; i < threadCount; ++i)
		{
			_threads.push_back(
				std::thread(&ThreadPool::WorkerThread, this, i));
		}
	}
	catch (...)
	{
		_done = true;
		throw;
	}
}

ThreadPool::~ThreadPool()
{
	_done = true;
}

void ThreadPool::RunPendingTask()
{
	_taskType task;
	if (PopTaskFromLocalQueue(task) ||
		PopTaskFromPoolQueue(task) ||
		PopTaskFromOtherThreadQueue(task))
	{
		task();
	}
	else
	{
		std::this_thread::yield();
	}
}

void ThreadPool::WorkerThread(unsigned int myIndex)
{
	_myIndex = myIndex;
	_localWorkQueue = _queues[_myIndex].get();

	while (!_done)
	{
		RunPendingTask();
	}
}

bool ThreadPool::PopTaskFromLocalQueue(_taskType& task)
{
	return _localWorkQueue && _localWorkQueue->TryPop(task);

}

bool ThreadPool::PopTaskFromPoolQueue(_taskType& task)
{
	return _poolWorkQueue.TryPop(task);
}

bool ThreadPool::PopTaskFromOtherThreadQueue(_taskType& task)
{
	for (unsigned i = 0; i < _queues.size(); ++i)
	{
		unsigned const index = (_myIndex + i + 1) % _queues.size();
		if (_queues[index]->TrySteal(task))
		{
			return true;
		}
	}
	return false;
}
