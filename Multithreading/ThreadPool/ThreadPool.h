#pragma once
#include <iostream>

#include <atomic>
#include <functional>
#include <future>

#include "ThreadSafeQueue.h"
#include "JoinThreads.h"
#include "FunctionWrapper.h"
#include "WorkStealingQueue.h"

//need to be outside the class
thread_local static unsigned int _myIndex;
thread_local static WorkStealingQueue* _localWorkQueue; 

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

	template<typename FunctionType>
	std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType f)
	{
		typedef typename std::result_of<FunctionType()>::type resultType;

		std::packaged_task<resultType()> task(std::move(f));
		std::future<resultType> res(task.get_future());

		if (_localWorkQueue)
		{
			_localWorkQueue->Push(std::move(task));
		}
		else
		{
			_poolWorkQueue.Push(std::move(task));
		}

		return res;
	}

	void RunPendingTask();

private:
	typedef FunctionWrapper _taskType;
	std::atomic_bool _done;
	ThreadSafeQueue<FunctionWrapper> _poolWorkQueue;
	std::vector<std::unique_ptr<WorkStealingQueue>> _queues;
	std::vector<std::thread> _threads;
	JoinThreads _joiner;

	void WorkerThread(unsigned int myIndex);
	bool PopTaskFromLocalQueue(_taskType& task);
	bool PopTaskFromPoolQueue(_taskType& task);
	bool PopTaskFromOtherThreadQueue(_taskType& task);
};