/**
* @file task.h
* @brief convenience functions and classed to run callable objects or member functions as async threads.
* 		 Application: algorithm test framework - algoritms are run sequentially  and main thread waits for each one to finish
* @details: created 2013, last_update 22/07/2025
* @dev: pss
**/ 

#ifndef __TASK_H__	
#define __TASK_H__	

#include <future>
#include <exception>
#include <type_traits>
#include "logger.h"

namespace bitgraph {

	namespace com {
		
		/**
		* @brief:  Runs a task  asynchronously (different thread)  defined by a member function of an object 
		*         WITH a return value as a thread with generic params and waits for it to finish to return
		*		  the value.
		* @return: task's return value or a default value if an exception is caught
		* @details: the task MUST have a return value. The task, if required, must throw an exception derived from std::exception.
		**/
		template<typename func_t, typename obj_t, typename... Args>
		inline
		typename std::enable_if<
			!std::is_void<typename std::result_of<func_t(obj_t&, Args...)>::type>::value,
			typename std::result_of<func_t(obj_t&, Args...)>::type
		>::type
		//typename std::result_of<func_t(obj_t&, Args...)>::type
		run_task_async(func_t func, obj_t& obj, Args&&... args) noexcept
		{	
			using ret_t = typename std::result_of<func_t(obj_t&, Args...)>::type;
			ret_t value{};

			//future to hold the result of the async task
			std::future<ret_t> fut = std::async(std::launch::async, func, std::ref(obj), std::forward<Args>(args)...);
			
			try {
				value = fut.get();		//can throw			
			}
			catch (std::exception& e) {
				LOGG_ERROR("caught exception thrown by task: ", e.what(), "bitgraph::run_task_async");				
			}

			return value;
		}

		/**
		* @brief:  Runs a task  asynchronously (different thread)  defined by a member function of an object
		*         WITHOUT a return value (void) as a thread with generic params and waits for it to finish.
		* @return: task's return value or a default value if an exception is caught
		* @details: - the task MUST return void. 
		*			- the task, if required, must throw an exception derived from std::exception.
		**/
		template<typename func_t, typename obj_t, typename... Args>
		inline
			typename std::enable_if<
			std::is_void<typename std::result_of<func_t(obj_t&, Args...)>::type>::value,
			void
			>::type
			run_task_async(func_t func, obj_t& obj, Args&&... args) noexcept
		{
			//future to hold the result of the async task
			std::future<void> fut = std::async(std::launch::async, func, std::ref(obj), std::forward<Args>(args)...);

			try {
				fut.get();		//can throw
			}
			catch (std::exception& e) {
				LOGG_ERROR("caught exception thrown by task: ", e.what(), "bitgraph::run_task_async");
			}
		}


		/**
		* @brief:  Runs a task  asynchronously (different thread)  defined by a callable object 
		*         WITH a return value as a thread with generic params and waits for it to finish to return
		*		  the value.
		* @return: task's return value or a default value if an exception is caught
		* @details: the task MUST have a return value. The task, if required, must throw an exception derived from std::exception.
		**/
		template<typename callable_t, typename... Args>
		inline
		typename std::enable_if<
			!std::is_void<typename std::result_of<callable_t(Args...)>::type>::value,
			typename std::result_of<callable_t(Args...)>::type
		>::type	
		run_task_async(callable_t&& obj, Args&&... args) noexcept
		{		

			using ret_t = typename std::result_of<callable_t(Args...)>::type;
			ret_t value{};

			//future to hold the result of the async task
			std::future<ret_t> fut = std::async(std::launch::async, obj, std::forward<Args>(args)...);;
		
			try {
				value = fut.get();			//can throw				
			}
			catch (std::exception & e) {
				LOGG_ERROR("caught exception thrown by task: ", e.what(), "bitgraph::run_task_async");
			}

			return value;
		}

		/**
		* @brief: Runs a task  asynchronously (different thread)  defined by a callable object 
		*         WITHOUT return value as a thread with generic params and waits for it to finish.
		* @return: task's return value or a default value if an exception is caught
		* @details: - the task MUST return void.
		*			- the task, if required, must throw an exception derived from std::exception.
		**/
		template<typename callable_t, typename... Args>
		inline
			typename std::enable_if<
			std::is_void<typename std::result_of<callable_t(Args...)>::type>::value,
			void
			>::type
		run_task_async(callable_t&& obj, Args&&... args) noexcept
		{
			//future to hold the result of the async task
			std::future<void> fut = std::async(std::launch::async, obj, std::forward<Args>(args)...);

			try {
				fut.get();			//can throw
			}
			catch (std::exception& e) {
				LOGG_ERROR("caught exception thrown by task: ", e.what(), "bitgraph::run_task_async");
			}
		}		

	}//end namespace com
	
	using com::run_task_async;		

}//end namespace bitgraph



#endif	