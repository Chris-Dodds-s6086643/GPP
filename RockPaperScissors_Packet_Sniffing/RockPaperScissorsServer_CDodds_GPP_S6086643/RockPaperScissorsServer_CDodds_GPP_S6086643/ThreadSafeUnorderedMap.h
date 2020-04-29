#pragma once
#include <utility>
#include <unordered_map>
#include <mutex>
#include <string>
#include <functional>
template <class K, class T, class M>

/**
Unordered map utilising mutex to maintain thread safety;
*
* @Template Class K, key type to be used in the map.
* @Template Class T, value type to be used in the map.
* @Template Class M, value used in loops etc. performed over the map.
*/
class ThreadSafeUnorderedMap
{
private:
	std::unordered_map<K, T> unorderedMap;
	mutable std::mutex mapMutex;

public:
	ThreadSafeUnorderedMap() : unorderedMap(), mapMutex() {}

	~ThreadSafeUnorderedMap() 
	{
		for (const auto& i : unorderedMap)
		{
			try
			{
				delete i.second;
			}
			catch (std::exception e)
			{
			}
		}
		unorderedMap.clear();
	}

	void Emplace(K key, T valueToEmplace)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		unorderedMap.emplace(key, valueToEmplace);
	}

	int GetNextAvailableKeyId()
	{
		int newID = 0;
		std::lock_guard<std::mutex> lock(mapMutex);
		while (unorderedMap.find(newID) != unorderedMap.end())
		{
			newID++;
		}
		return newID;
	}

	T& AccessValue(K key)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		return unorderedMap[key];
	}

	void RemoveValueAtKey(K key)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		try
		{
			delete(unorderedMap.at(key));
		}
		catch (std::exception e)
		{}
		unorderedMap.erase(key);
	}

	void FunctionWithStringOverMapExcludingGivenKey(std::function<void(T, M)>& functionToApply, M messageToSend, K keyToignore)
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		for (const auto& i : unorderedMap)
		{
			if (i.first != keyToignore)
			{
				functionToApply(i.second, messageToSend);
			}
		}
	}

	auto GetHashFunction()
	{
		std::lock_guard<std::mutex> lock(mapMutex);
		return unorderedMap.hash_function();
	}
};