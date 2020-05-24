#pragma once	
#include "Networking.h"
#include "InputManager.h"
#include "ThreadSafeQueue.h"
#include "Message.h"
#include <iostream>
//#include <Windows.h>
//#include <stdio.h>
//#include <atomic>
//
//
//std::atomic<bool>* isConsoleOpen;
//
//BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
//{
//	switch (fdwCtrlType)
//	{
//	case CTRL_CLOSE_EVENT:
//		std::cout << "Closing thing;";
//	case CTRL_LOGOFF_EVENT:
//		std::cout << "Logoff thing;";
//	case CTRL_SHUTDOWN_EVENT:
//		isConsoleOpen->store(false);
//		std::cout << "Shutdown thing;";
//		return TRUE;
//	default:
//		return FALSE;
//		break;
//	}
//}

int main()
{
	/*if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{*/
	//isConsoleOpen = new std::atomic<bool>(true);
	ThreadSafeQueue<Message> threadSafeMessageQueue;
	InputManager iM(threadSafeMessageQueue);
	std::cout << "Hello World!\n";
	//}
	//else
	//{
	//}
    
}
