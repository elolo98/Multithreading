#pragma once

template<typename T, size_t capacity>
class ThreadSafeRingBuffer
{
public:
    // Push an item to the end if there is free space
    //  Returns true if succesful
    //  Returns false if there is not enough space
    inline bool push_back(const T& item)
    {
        bool result = false;
        _lock.lock();
        size_t next = (_head + 1) % capacity;
        if (next != _tail)
        {
            _data[_head] = item;
            _head = next;
            result = true;
        }
        _lock.unlock();
        return result;
    }

    // Get an item if there are any
    //  Returns true if succesful
    //  Returns false if there are no items
    inline bool pop_front(T& item)
    {
        bool result = false;
        _lock.lock();
        if (_tail != _head)
        {
            item = _data[_tail];
            _tail = (_tail + 1) % capacity;
            result = true;
        }
        _lock.unlock();
        return result;
    }
private:
    T _data[capacity];
    size_t _head = 0;
    size_t _tail = 0;
    std::mutex _lock;
};