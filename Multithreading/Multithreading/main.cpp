#include <iostream>
#include <chrono>
#include <string>

#include "JobSystem.h"

void ThreadFunction()
{
    std::cout << "I'm in a thread!\n ";
}

void Spin(float milliseconds)
{
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
        std::cout << name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
    }
};

int main()
{
    //std::thread t1(ThreadFunction);

    //t1.join();
    JobSystem jobSystem;
    jobSystem.Initialize();

    // Serial test
    {
        auto t = timer("Serial test: ");
        Spin(100);
        Spin(100);
        Spin(100);
        Spin(100);
    }

    // Execute test
    {
        auto t = timer("Execute() test: ");
        jobSystem.Execute([] { Spin(100); });
        jobSystem.Execute([] { Spin(100); });
        jobSystem.Execute([] { Spin(100); });
        jobSystem.Execute([] { Spin(100); });
        jobSystem.Wait();
    }

    return 0;
}
