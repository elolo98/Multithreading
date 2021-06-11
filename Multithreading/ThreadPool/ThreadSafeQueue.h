#pragma once

#include <queue> 
#include <mutex>
#include <thread>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue
{
private:
	struct Node
	{
		std::shared_ptr<T> _data;
		std::unique_ptr<Node> _next;
	};

	std::mutex _headMutex;
	std::unique_ptr<Node> _head;
	std::mutex _tailMutex;
	Node* _tail;
	std::condition_variable _dataCondition;


	Node* GetTail()
	{
		std::lock_guard<std::mutex> tailLock(_tailMutex);
		return _tail;
	}

	std::unique_ptr<Node> PopHead()
	{
		std::unique_ptr<Node> oldHead = std::move(_head);
		_head = std::move(oldHead->_next);
		return oldHead;
	}

	std::unique_lock<std::mutex> WaitForData()
	{
		std::unique_lock<std::mutex> headLock(_headMutex);
		_dataCondition.wait(headLock, [&] {return _head.get() != GetTail(); });
		return std::move(headLock);
	}

	std::unique_ptr<Node> WaitPopHead()
	{
		std::unique_lock<std::mutex> headLock(WaitForData());
		return PopHead();
	}

	std::unique_ptr<Node> WaitPopHead(T& value)
	{
		std::unique_lock<std::mutex> headLock(WaitForData());
		value = std::move(*_head->_data);
		return PopHead();
	}

	std::unique_ptr<Node> TryPopHead()
	{
		std::lock_guard<std::mutex> headLock(_headMutex);
		if (_head.get() == GetTail())
		{
			return std::unique_ptr<Node>();
		}
		return PopHead();
	}

	std::unique_ptr<Node> TryPopHead(T& value)
	{
		std::lock_guard<std::mutex> headLock(_headMutex);
		if (_head.get() == GetTail())
		{
			return std::unique_ptr<Node>();
		}
		value = std::move(*_head->_data);
		return PopHead();
	}

public:
	ThreadSafeQueue() : _head(new Node), _tail(_head.get()) {}
	ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
	ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;

	std::shared_ptr<T> TryPop()
	{
		std::unique_ptr<Node> oldHead = TryPopHead();
		return oldHead ? oldHead->_data : std::shared_ptr<T>();
	}

	bool TryPop(T& value)
	{
		std::unique_ptr<Node> const oldHead = TryPopHead(value);
		return oldHead.get();
	}

	void Push(T newValue)
	{
		std::shared_ptr<T> newData(
			std::make_shared<T>(std::move(newValue)));
		std::unique_ptr<Node> p(new Node);
		{
			std::lock_guard<std::mutex> tailLock(_tailMutex);
			_tail->_data = newData;
			Node* const newTail = p.get();
			_tail->_next = std::move(p);
			_tail = newTail;
		}
		_dataCondition.notify_one();
	}

	std::shared_ptr<T> WaitAndPop()
	{
		std::unique_ptr<Node> const oldHead = WaitPopHead();
		return oldHead->_data;

	}

	void WaitAndPop(T& value)
	{
		std::unique_ptr<Node> const oldHead = WaitPopHead(value);

	}

	bool Empty()
	{
		std::lock_guard<std::mutex> headLock(_headMutex);
		return (_head.get() == GetTail());
	}
};
