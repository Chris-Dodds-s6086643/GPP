#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
template <class T>

class ThreadSafeQueue
{
private:
	std::queue<T> queue;
	mutable std::mutex mutex;
	std::condition_variable condition;

public:
	ThreadSafeQueue() : queue(), mutex(), condition() {}

	~ThreadSafeQueue() {}

	void push(T valueToPush)
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(valueToPush);
		condition.notify_one();
	}

	void waitPop(T& poppedValue)
	{
		std::unique_lock<std::mutex> lock(mutex);
		while (queue.empty())
		{
			condition.wait(lock);
		}
		poppedValue = std::move(queue.front);
		queue.pop();
	}

	bool tryPop(T& value)
	{
		std::unique_lock<std::mutex> lock(mutex);
		if (queue.empty())
		{
			return false;
		}
		value = std::move(queue.front());
		queue.pop();
		return true;
	}
};

