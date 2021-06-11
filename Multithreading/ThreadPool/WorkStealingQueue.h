#pragma once

#include "FunctionWrapper.h"
#include <deque>
#include <mutex>

//We hope work stealing is a rare event
// Better implementation : write a lock-free queue that allows the owner thread 
//to push and pop at one end while other threads can steal entries from the other
class WorkStealingQueue
{
private:
	typedef FunctionWrapper _dataType;
	std::deque<_dataType> _theQueue;
	mutable std::mutex _theMutex;

public:
	WorkStealingQueue() {}
	WorkStealingQueue(const WorkStealingQueue& other) = delete;
	WorkStealingQueue& operator=(
		const WorkStealingQueue& other) = delete;

	void Push(_dataType data);
	bool Empty() const;
	bool TryPop(_dataType& res);
	bool TrySteal(_dataType& res);
};