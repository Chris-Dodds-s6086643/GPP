#pragma once
#include <utility>
#include <unordered_map>
#include <mutex>
#include <string>
#include <functional>
template <class K,class T>

class ThreadSafeUnorderedMap
{
private:
	std::unordered_map<K, T> unorderedMap;
	mutable std::mutex mapMutex;
	
public:
	ThreadSafeUnorderedMap() : unorderedMap(), mapMutex() {}

	~ThreadSafeUnorderedMap() {}

	void emplace(K key, T valueToEmplace)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		unorderedMap.emplace(key, valueToEmplace);
	}

	int getNextAvailableKeyId()
	{
		int newID = 0;
		std::lock_guard<std::mutex> lock(mapMutex);
		while (unorderedMap.find(newID) != unorderedMap.end())
		{
			newID++;
		}
		return newID;
	}

	T& accessValue(K key)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		return unorderedMap[key];
	}

	void removeValueAtKey(K key)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		unorderedMap.erase(key);
	}

	void functionOverMap(std::function<void(T, std::string)>& functionToApply, std::string messageToSend, int senderID)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		for (std::unordered_map<int, ClientPlayer*>::iterator i = unorderedMap.begin(); i != unorderedMap.end(); i++)
		{
			if (i->first != senderID)
			{
				functionToApply(i->second, messageToSend);
			}
		}
	}

	auto getHashFunction() 
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		return unorderedMap.hash_function();
	}
};