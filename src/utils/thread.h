/**
* @file thread.h
* @brief convenience functions and classed to run callaeble objects or member functions as threads
* @details: created 2013, last_update 19/07/2025
* @dev: pss
**/ 

#ifndef __THREAD_H__	
#define __THREAD_H__	

#include <thread>
#include "logger.h"

namespace bitgraph {

	namespace com {

		////////////////////
		//
		// Thread class
		//
		// Runs a function member T_func without params of a class T as a new thread
		// (main thread waits for its finish)
		//
		//////////////////
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

		};//end class Thread

	
		/**
		* @brief: Runs a member function of an object with generic params as a new thread and waits for its finish
		* returns 0 if successful, -1 otherwise (non-throwing interface)	
		**/
		template<typename func_t, typename obj_t, typename... Args>
		int run_thread(func_t func, obj_t& obj, Args&&... args) noexcept {

			try {
				std::thread t(func, std::ref(obj), std::forward<Args>(args)...);
				t.join();															// Waits for the thread to finish	
			}
			catch (...) {
				LOG_ERROR("Error during thread execution - run_thread");
				return -1;
			}

			return 0;
		}

		/**
		* @brief: Runs a callable object as a thread with generic params and waits for its finish
		* returns 0 if successful, -1 otherwise (non-throwing interface)
		**/
		template<typename callable_t, typename... Args>
		int run_thread(callable_t functor, Args&&... args) noexcept {
			
			try {
				std::thread t(functor, std::forward<Args>(args)...);
				t.join();															// Waits for the thread to finish
			}
			catch (...) {
				LOG_ERROR("Error during thread execution - run_thread");
				return -1;
			}

			return 0;
		}


	}//end namespace com
	
	using com::Thread;
	using com::run_thread;


}//end namespace bitgraph



#endif	