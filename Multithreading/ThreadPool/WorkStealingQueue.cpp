#include "WorkStealingQueue.h"

void WorkStealingQueue::Push(_dataType data)
{
	std::lock_guard<std::mutex> lock(_theMutex);
	_theQueue.push_front(std::move(data));
}

bool WorkStealingQueue::Empty() const
{
	std::lock_guard<std::mutex> lock(_theMutex);
	return _theQueue.empty();
}

bool WorkStealingQueue::TryPop(_dataType& res)
{
	std::lock_guard<std::mutex> lock(_theMutex);
	if (_theQueue.empty())
	{
		return false;
	}
	res = std::move(_theQueue.front());
	_theQueue.pop_front();
	return true;
}

bool WorkStealingQueue::TrySteal(_dataType& res)
{
	std::lock_guard<std::mutex> lock(_theMutex);
	if (_theQueue.empty())
	{
		return false;
	}
	res = std::move(_theQueue.back());
	_theQueue.pop_back();
	return true;
}
