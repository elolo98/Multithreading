#pragma once

#include <list>

#include "ThreadPool.h"

template<typename T>
struct Sorter
{
	ThreadPool pool;

	std::list<T> DoSort(std::list<T>& chunkData)
	{
		if (chunkData.empty())
		{
			return chunkData;
		}

		std::list<T> result;

		result.splice(result.begin(), chunkData, chunkData.begin());
		T const& partition_val = *result.begin();

		typename std::list<T>::iterator divide_point =
			std::partition(chunkData.begin(), chunkData.end(),
				[&](T const& val) {return val < partition_val; });

		std::list<T> new_lower_chunk;
		new_lower_chunk.splice(new_lower_chunk.end(),
			chunkData, chunkData.begin(),
			divide_point);

		std::future<std::list<T> > new_lower =
			pool.Submit(std::bind(&Sorter::DoSort, this,
				std::move(new_lower_chunk)));

		std::list<T> new_higher(DoSort(chunkData));
		result.splice(result.end(), new_higher);

		while (new_lower.wait_for(std::chrono::seconds(0)) ==
			std::future_status::timeout)
		{
			pool.RunPendingTask();
		}

		result.splice(result.begin(), new_lower.get());
		return result;
	}
};

template<typename T>
std::list<T> ParrallelQuickSort(std::list<T> input)
{
	if (input.empty())
	{
		return input;
	}

	Sorter<T> s;
	return s.DoSort(input);
}