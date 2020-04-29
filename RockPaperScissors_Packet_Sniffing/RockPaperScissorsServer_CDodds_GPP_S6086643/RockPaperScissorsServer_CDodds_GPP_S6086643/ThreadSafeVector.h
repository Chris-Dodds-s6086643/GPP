#pragma once
#include <vector>
#include <mutex>
#include <string>
#include <functional>
template <class T>

class ThreadSafeVector
{
private:
	std::vector<T> innerVector;
	mutable std::mutex vectorMutex;

public:
	ThreadSafeVector() : innerVector(), vectorMutex() {}
	~ThreadSafeVector() {}

	void push(T valueToPush)
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		innerVector.push_back(valueToPush);
	}

	int Size()
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		return innerVector.size();
	}
	
	T& GetAt(int index)
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		return innerVector.at(index);
	}

	void EraseAt(int index)
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		innerVector.erase(index);
	}

	void MapVoidFunction(std::function<void(T)>& functionToApply)
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		for (int i = 0; i < innerVector.size(); i++)
		{
			functionToApply(innerVector);
		}
	}

	void MapVoidFunctionParamStringExceptGivenIndex(std::function<void(T, std::string)>& functionToApply, std::string stringInput, int indexToAvoid)
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		for (int i = 0; i < innerVector.size(); i++)
		{
			if (i != indexToAvoid)
			{
				functionToApply(innerVector, stringInput);
			}
		}
	}

	/**
	Calls a function (bool f(T, int)) with each object in the entire vector;
	Will perform an early exit of the vector loop on the function returning false.
		E.G function { return (T.id != givenInt) }
		when the function gives false then the ID would not be unique and so loop can exit.

	*@Param functionToApply a function as laid out in the <functional> library, takes a template type object and an int.
	*@param givenInt the int that will also be used in the functionToApply.
	*/
	bool MapBoolFunctionParamIntReturnBool(std::function<bool(T, int)>& functionToApply, int givenInt)
	{
		std::lock_guard<std::mutex> lock(vectorMutex);
		bool outBool = true;
		for (int i = 0; i < int(innerVector.size()); i++)
		{
			outBool = functionToApply(innerVector[i], givenInt);
			if (outBool == false)
			{
				return false;
			}
		}
		return outBool;
	}

};
