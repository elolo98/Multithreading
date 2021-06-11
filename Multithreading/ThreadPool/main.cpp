#include <iostream>
#include <chrono>
#include <stdio.h>
#include <random>

#include "ThreadPool.h"
#include "Quicksort.h"

void Spin(float milliseconds)
{
    //std::cout << "spin" << std::endl;
	milliseconds /= 1000.0f;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	double ms = 0;
	while (ms < milliseconds)
	{
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		ms = time_span.count();
	}
}

struct timer
{
	std::string name;
	std::chrono::high_resolution_clock::time_point start;

	timer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
	~timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << name.c_str() << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
	}
};

// a cpu-busy task.
void work_proc()
{
    std::random_device rd;
    std::mt19937 rng(rd());

    // build a vector of random numbers
    std::vector<int> data;
    data.reserve(100000);
    std::generate_n(std::back_inserter(data), data.capacity(), [&]() { return rng(); });
    std::sort(data.begin(), data.end(), std::greater<int>());
}

int main()
{
    ThreadPool* threadPool = new ThreadPool();

    // Serial test
    {
        auto t = timer("Serial test: ");
        Spin(100);
        Spin(100);
        Spin(100);
        Spin(100);
    }

    //run five batches of 100 items
       

    // Execute test
    {
        auto t = timer("Execute() test: ");

       /* for (int x = 0; x < 5; ++x)
        {
            // queue 100 work tasks
            for (int i = 0; i < 100; ++i)
                threadPool->Submit([] { work_proc; });
        }

        threadPool->Submit([] { Spin(100); });
        threadPool->Submit([] { Spin(100); });
        threadPool->Submit([] { Spin(100); });
        threadPool->Submit([] { Spin(100); });*/




        //QuickSort using Thread Pool
        std::list<int> myList;
        myList.push_back(6);
        myList.push_back(5);
        myList.push_back(2);
        myList.push_back(8);
        myList.push_back(25);
        myList.push_back(41);
        myList.push_back(72);
        myList.push_back(10);
        myList.push_back(77);
        myList.push_back(25);
        myList.push_back(32);
        myList.push_back(121);
        myList.push_back(31);
        myList.push_back(42);
        myList.push_back(342);
        myList.push_back(62);
        myList.push_back(28);

        auto result = ParrallelQuickSort<int>(myList);

        for (int var : result)
            std::cout << var << std::endl;
    }

    return 0;
}
