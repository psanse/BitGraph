// thread.h: interface for the Thread class. 
// Runs a function member with prototype (R)(func)() as thread with timeout. 
// 
//////////////////////////////////////////////////////////////////////

#pragma once


#ifdef _MSC_VER
	#include <windows.h>
	#include <process.h>
#else
	#define _GNU_SOURCE             /* See feature_test_macros(7) */
	#include <pthread.h>
	#include <signal.h>
	typedef unsigned long DWORD;
	typedef unsigned short WORD;
	typedef unsigned int UNINT32;
	#include <unistd.h>

	extern "C"{
		static void handler(int signum)	{
			LOG_INFO("Exiting thread via handler....");
			sleep(0.5);
			pthread_exit(NULL);
		}
	}

#endif

#include "logger.h"

#define NO_THREAD_TIME_OUT 0

template <class T, class R>
class Thread {
	typedef R (T::*T_function)(); 
	struct thread_data_t{
		T_function function;
		T* object;
	};

	enum result_t {TIMEOUT=-1, OK};

public:	

	static int Start(T_function f, T* pthis, DWORD tout = NO_THREAD_TIME_OUT /*ms*/) {
		/////////////////////
		// Controlled thread launcher: Returns -1 if tout, 0 if OK

		try {
			/////////////////////////
			std::thread t(f, pthis);
			//////////////////////////

			t.join();

		}
		catch (...) {
			LOG_ERROR("Error during thread execution - Thread::Start");
			return -1;
		}


		return 0;
	}


//
//
//		//Copy the data into the structure
//		result_t result=OK;
//		thread_data_t data={f, pthis};
//		
//		
//	/*	thread_data_t* data=new thread_data_t;
//		data->function=f;
//		data->object=pthis;*/
//				
//		#ifdef _MSC_VER	
//			HANDLE hThread;
//			DWORD dw;
//			hThread=(HANDLE)_beginthread(thread, 0, &data);
//			LOG_PRINT("Thread running");
//			dw=WaitForSingleObject( hThread, (tout==NO_THREAD_TIME_OUT)? INFINITE : tout );
//			if(dw==WAIT_TIMEOUT){
//				result=TIMEOUT;
//				LOG_INFO("Time out: Killing thread");
//				if(TerminateThread(hThread,0)){								//Seems to deallocate memory adequately (CHECK)
//					CloseHandle(hThread);
//					LOG_INFO("Thread killed satisfactorily");
//				}
//			}else{
//				//Mensajeria	
//				result=OK;
//				LOG_PRINT("Thread finishes normally");
//			}
//
//		#else
//		
//			LOG_PRINT("Thread creation....");
//			pthread_t thid;
//			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
//           	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
//			pthread_create(&thid,NULL,thread, (void *) &data);
//
//			LOG_PRINT("Thread running");
//			if (tout == NO_THREAD_TIME_OUT){
//				LOG_PRINT("unlimited run time");
//				pthread_join(thid,NULL);
//				//Mensajeria
//				result=OK;
//				LOG_PRINT("Thread finishes normally");
//			}else{
//				
//				int s;
//				#if defined(__CYGWIN__)||defined(__MINGW64__)
//					s = 1;
//				#else
//					struct timespec ts;
//					
//					if (clock_gettime(CLOCK_REALTIME, &ts) == -1){
//						LOG_ERROR("No clock");
//						/* handle error */
//					}else{
//							
//						ts.tv_sec+=(tout/1000);
//						LOG_PRINT("set max run time: "<<tout/1000);
//						s = pthread_timedjoin_np(thid, NULL,&ts);
//
//						if (s != 0){
//							result=TIMEOUT;
//							LOG_INFO("Time out: Killing thread");
//						
//							pthread_kill(thid, SIGUSR1);
//							pthread_join(thid, NULL);
//							LOG_INFO("Time out: test thread correctly terminated and joined ");
//						}else{
//							//Mensajeria
//							result=OK;
//							LOG_PRINT("Thread finishes normally");
//
//						}
//					}
//				#endif
//
//			}
//			LOG_PRINT("Thread end");
//
//		#endif
//					
//	return result;
//	}
//private:
//
////the thread
//#ifdef _MSC_VER
//	static void thread(void *p)
//#else
//	static void* thread(void *p)
//#endif
//	{
//#ifndef _MSC_VER
//	//is this necessary (already in main thread)?
//		/*pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
//        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);*/
//		if (signal(SIGUSR1, handler) == SIG_ERR) {
//			LOG_ERROR("Thread unable to handle SIGUSR1");
//			return NULL;
//		}
//
//		
//#endif
//		thread_data_t* data = (thread_data_t *)p;
//		T_function function=data->function;
//		T* object=data->object;
//		//delete data;
//
//		//runs member function (output is of type R)
//		(object->*function)();
//
//		
//#ifndef _MSC_VER
//		return NULL;
//#endif
//	}
};



