/**
 * @file task_utils.h
 * @brief Utilities for executing callable objects asynchronously.
 *
 * This header provides synchronous-wait wrappers around std::async. Each task
 * is launched on a separate thread, and the calling thread waits until the
 * task finishes.
 *
 * Exceptions thrown by the task or while launching it are propagated to the
 * caller.
 *
 * @author Pablo San Segundo
 * @date Created: 2013
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_UTILS_TASK_UTILS_H	
#define BITGRAPH_UTILS_TASK_UTILS_H	

#include <future>
#include <functional>
#include <type_traits>
#include <utility>

namespace bitgraph {

	namespace utils {

		/**
		 * @brief Runs a callable asynchronously and waits for its result.
		 *
		 * The callable is launched using std::launch::async, ensuring execution on a
		 * separate thread. The calling thread waits until the task finishes.
		 *
		 * @tparam Callable Callable type.
		 * @tparam Args Argument types.
		 * @param callable Callable object or function.
		 * @param args Arguments forwarded to the callable.
		 * @return The task result. For a task returning void, this function also
		 *         returns void.
		 *
		 * @throws std::system_error If the asynchronous task cannot be launched.
		 * @throws Any exception propagated by the callable.
		 */
		template<class Callable, class... Args>
		auto run_task_async(
			Callable&& callable,
			Args&&... args)
			-> typename std::result_of<
			Callable && (Args&&...)>::type
		{
			return std::async(
				std::launch::async,
				std::forward<Callable>(callable),
				std::forward<Args>(args)...).get();
		}

		/**
		 * @brief Runs an object's member function asynchronously and waits for it.
		 *
		 * The object is passed by reference and is not copied.
		 *
		 * @tparam MemberFunction Member-function pointer type.
		 * @tparam Object Object type.
		 * @tparam Args Argument types.
		 * @param function Member function to execute.
		 * @param object Object on which to invoke the function.
		 * @param args Arguments forwarded to the member function.
		 * @return The task result, or void if the member function returns void.
		 *
		 * @throws std::system_error If the asynchronous task cannot be launched.
		 * @throws Any exception propagated by the member function.
		 */
		template<class MemberFunction, class Object, class... Args>
		auto run_member_task_async(
			MemberFunction function,
			Object& object,
			Args&&... args)
			-> typename std::result_of<
			MemberFunction(Object&, Args&&...)>::type
		{
			return std::async(
				std::launch::async,
				function,
				std::ref(object),
				std::forward<Args>(args)...).get();
		}		
		

	}//end namespace utils
	
	using utils::run_task_async;		

}//end namespace bitgraph


#endif	// BITGRAPH_UTILS_TASK_UTILS_H