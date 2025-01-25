/**
* @file thread.h
* @brief Thread class to run a function member of a class as a thread
* @date 2013
* @last_update 20/01/2025
* @version 1.0
**/ 

#ifndef __THREAD_H__	
#define __THREAD_H__	

#include <thread>
#include "logger.h"

////////////////////
//
// Thread class
//
// Runs a function member T_func of a class T as a new thread
// (main thread waits for its finish)
//
//////////////////
namespace com {
		
	template <class T, class T_func>
	class Thread {

	public:
		/**
		* @ brief Starts a thread with a function member of a class
		* @returns 0 if successful, -1 otherwise
		**/
		static int Start(T_func f, T* pthis) {

			try {
				/////////////////////////
				std::thread t(f, pthis);
				//////////////////////////

				//waits for the thread to finish
				t.join();

			}
			catch (...) {
				LOG_ERROR("Error during thread execution - Thread::Start");
				return -1;
			}

			return 0;
		}

	};
}


#endif	