/**
* @file task.h
* @brief convenience functions and classed to run callable objects or member functions as async threads.
* 		 Application: algorithm test framework - algoritms are run sequentially  and main thread waits for each one to finish
* @details: created 2013, last_update 21/07/2025
* @dev: pss
**/ 

#ifndef __TASK_H__	
#define __TASK_H__	

#include <future>
#include <exception>
#include "logger.h"

namespace bitgraph {

	namespace com {


		/**
		* @brief: Runs a task according to a member function of an object with generic params and waits for its finish
		* @return: value determined by the task (member function) or a default value if an exception is caught
		* @details: The task MUST have a return value. The task, if required, must throw an exception derived from std::exception.
		**/
		template<typename func_t, typename obj_t, typename... Args>
		auto run_task(func_t func, obj_t& obj, Args&&... args) noexcept ->
							decltype((obj.*func)(std::forward<Args>(args)...))
		{
			using ret_t = decltype((obj.*func)(std::forward<Args>(args)...));
			ret_t value{};

			//future to hold the result of the async task
			decltype(std::async(func, std::ref(obj), std::forward<Args>(args)...)) fut;

			try {
				fut = std::async(func, std::ref(obj), std::forward<Args>(args)...); 
				value = fut.get();
			}
			catch (std::exception& e) {
				LOGG_ERROR("caught exception thrown by task: ", e.what(), "bitgraph::run_task");
			}

			return value;
		}

		/**
		* @brief: Runs a task according to a callable object as a thread with generic params and waits for its finish
		* @return: value determined by the task (callable object) or a default value if an exception is caught
		* @details: The task MUST have a return value. The task, if required, must throw an exception derived from std::exception.
		**/
		template<typename callable_t, typename... Args>
		auto run_task(callable_t obj, Args&&... args) noexcept
							-> decltype(obj(std::forward<Args>(args)...))
		{
			using ret_t = decltype(obj(std::forward<Args>(args)...));
			ret_t value{};

			//future to hold the result of the async task
			decltype(std::async(obj, std::forward<Args>(args)...)) fut;

			try {
				fut = std::async(obj, std::forward<Args>(args)...);
				value = fut.get();						
			}
			catch (std::exception & e) {
				LOGG_ERROR("caught exception thrown by task: ", e.what(), "bitgraph::run_task");
			}

			return value;
		}
				

		//////////////////////
		////
		//// Thread class (DEPRECATED - REMOVE)
		////
		//// Runs a function member T_func without params of a class T as a new thread
		//// (main thread waits for its finish)
		////
		////////////////////
		//template <class T, class T_func>
		//class Thread {

		//public:
		//	/**
		//	* @ brief Starts a thread with a function member of a class
		//	**/
		//	static void Start(T_func f, T* pthis) {
		//						
		//		/////////////////////////
		//		std::thread t(f, pthis);
		//		//////////////////////////

		//		//waits for the thread to finish
		//		t.join();
		//		
		//	}

		//};


	}//end namespace com
	
	using com::run_task;
	//using com::Thread;

}//end namespace bitgraph



#endif	