/**
* @file logger.h
* @brief  A simplistic, light-weight, single-header C++ logger  "logy v1.2"
*		  Dev: Giovanni Squillero <giovanni.squillero@polito.it> (Summer 2018)
*		  URL: https://github.com/squillero/logy
*		  This code has been dedicated to the public domain
* @date 03/11/2024
* @last_update 19/01/25
* 
* @comments: 
* 1.vectors & initializer_list are supported using SFINAE
* 2.C++11 compliant
* 3.Automatic tuning with the help of copilot (06/11/2024)
* 
* TODO - Change to a professional logger in the future (22/09/2025)
**/

#ifndef	__LOGY_H__
#define __LOGY_H__

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <type_traits>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <initializer_list>
#include <cstdarg>

namespace bitgraph{

//////////////////////////////////////////////
//logger level manual setting - [Warning and ERROR only - default] 
//
// (uncomment one to the options to change default, alternatively use CMake GUI)
//
//#define LOGGER_ERROR_LEVEL		//only ERROR (minimum priority)
//#define LOGGER_VERBOSE_LEVEL		//all except DEBUG (second priority)
//#define LOGGER_DEBUG_LEVEL		//all - top priority
//
//all undef	 WARNING and ERROR

//////////////////////////////////////////
//legacy logger macros compatibility

#define LOG_ERROR(...)			Error(__VA_ARGS__)
#define LOG_WARNING(...)		Warning(__VA_ARGS__)
#define LOG_INFO(...)			Info(__VA_ARGS__)
#define LOG_PRINT(...)			Info(__VA_ARGS__)
#define LOG_DEBUG(...)			Debug(__VA_ARGS__)

//////////////////////
#define LOG_PAK()				Info("press any key to continue")
#define LOG_LINE()				Info("-------------------------")


// Type trait to check for range loop support
template <typename T> struct is_rangeloop_supported : std::false_type {};
template <typename T> struct is_rangeloop_supported<std::vector<T>> : std::true_type {};
template <typename T> struct is_rangeloop_supported<std::initializer_list<T>> : std::true_type {};

// Tag expansion function for supported range-loop types
template<typename T>
typename std::enable_if<is_rangeloop_supported<T>::value, std::string>::type tag_expand(const T& arg) {
	std::ostringstream ss;
	ss << "[";
	for (const auto& e : arg)
		ss << " " << tag_expand(e);
	ss << " ]";
	return ss.str();
}

// Tag expansion function for non-range-loop types
template<typename T>
typename std::enable_if<!is_rangeloop_supported<T>::value, std::string>::type tag_expand(const T& arg) {
	std::ostringstream ss;
	ss << arg;
	return ss.str();
}

// Log header with timestamp
static inline void logy_header(const char* tag) {
	char timestamp[100] = "";
	std::time_t t = std::time(nullptr);
	std::strftime(timestamp, sizeof(timestamp), "[%H:%M:%S]-[%d/%b/%Y]", std::localtime(&t));
	std::fprintf(stderr, "%s%s", timestamp, tag);
}

// Log helper functions for variadic templates
static inline void logy_helper() {
	std::cerr << std::endl;
}

template<typename F, typename... R>
static inline void logy_helper(const F& first, R&&... rest) {
	std::cerr << " " << tag_expand(first);
	logy_helper(std::forward<R>(rest)...);
}

// Atributo portable para comprobación de formato printf.
// GCC/Clang/ICC: aplica __attribute__((format(printf,...)))
// MSVC: vacío (no soportado)
#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
#define LOGY_PRINTF_ATTR(fmt_idx, arg_idx) __attribute__((format(printf, fmt_idx, arg_idx)))
#else
#define LOGY_PRINTF_ATTR(fmt_idx, arg_idx)
#endif


// Declaraciones con atributo
inline void _Debug(const char* format, ...) LOGY_PRINTF_ATTR(1, 2);
inline void _Info(const char* format, ...) LOGY_PRINTF_ATTR(1, 2);
inline void _Warning(const char* format, ...) LOGY_PRINTF_ATTR(1, 2);
inline void _Error(const char* format, ...) LOGY_PRINTF_ATTR(1, 2);

// Definiciones sin atributo
inline void _Debug(const char* format, ...) {
	logy_header(" DEBUG: ");
	va_list ap;
	va_start(ap, format);
	std::vfprintf(stderr, format, ap);   // OJO: vfprintf
	va_end(ap);
	std::fprintf(stderr, "\n");
	std::fflush(stderr);
}

inline void _Info(const char* format, ...) {
	logy_header(" INFO: ");
	va_list ap;
	va_start(ap, format);
	std::vfprintf(stderr, format, ap);   // OJO: vfprintf
	va_end(ap);
	std::fprintf(stderr, "\n");
	std::fflush(stderr);
}

inline void _Warning(const char* format, ...) {
	logy_header(" WARNING: ");
	va_list ap;
	va_start(ap, format);
	std::vfprintf(stderr, format, ap);   // OJO: vfprintf
	va_end(ap);
	std::fprintf(stderr, "\n");
	std::fflush(stderr);
}

inline void _Error(const char* format, ...) {
	logy_header(" ERROR: ");
	va_list ap;
	va_start(ap, format);
	std::vfprintf(stderr, format, ap);   // ya lo tenías bien aquí
	va_end(ap);
	std::fprintf(stderr, "\n");
	std::fflush(stderr);
}


// Direct print logging without format strings
template<typename... T>
void _Debug2(T&&... args) {
	logy_header(" DEBUG:");
	logy_helper(std::forward<T>(args)...);
}

template<typename... T>
void _Info2(T&&... args) {
	logy_header(" INFO:");
	logy_helper(std::forward<T>(args)...);
}

template<typename... T>
void _Warning2(T&&... args) {
	logy_header(" WARNING:");
	logy_helper(std::forward<T>(args)...);
}

template<typename... T>
void _Error2(T&&... args) {
	logy_header(" ERROR:");
	logy_helper(std::forward<T>(args)...);
}

#if defined(DEBUG) || defined(LOGGER_DEBUG_LEVEL)  // All messages enabled

#define Debug(...) bitgraph::_Debug(__VA_ARGS__)
#define Info(...) bitgraph::_Info(__VA_ARGS__)
#define Warning(...) bitgraph::_Warning(__VA_ARGS__)
#define Error(...) bitgraph::_Error(__VA_ARGS__)
#define LOGG_DEBUG(...) bitgraph::_Debug2(__VA_ARGS__)
#define LOGG_INFO(...) bitgraph::_Info2(__VA_ARGS__)
#define LOGG_WARNING(...) bitgraph::_Warning2(__VA_ARGS__)
#define LOGG_ERROR(...) bitgraph::_Error2(__VA_ARGS__)

#elif defined(VERBOSE) || defined(LOGGER_VERBOSE_LEVEL)  // All except debug

#define Debug(...) ((void)0)
#define Info(...) bitgraph::_Info(__VA_ARGS__)
#define Warning(...) bitgraph::_Warning(__VA_ARGS__)
#define Error(...) bitgraph::_Error(__VA_ARGS__)
#define LOGG_DEBUG(...) ((void)0)
#define LOGG_INFO(...) bitgraph::_Info2(__VA_ARGS__)
#define LOGG_WARNING(...) bitgraph::_Warning2(__VA_ARGS__)
#define LOGG_ERROR(...) bitgraph::_Error2(__VA_ARGS__)

#elif defined(ERROR) || defined(LOGGER_ERROR_LEVEL)  // Only errors

#define Debug(...) ((void)0)
#define Info(...) ((void)0)
#define Warning(...) ((void)0)
#define Error(...) bitgraph::_Error(__VA_ARGS__)
#define LOGG_DEBUG(...) ((void)0)
#define LOGG_INFO(...) ((void)0)
#define LOGG_WARNING(...) ((void)0)
#define LOGG_ERROR(...) bitgraph::_Error2(__VA_ARGS__)

#else  // Only warnings and errors by default

#define Debug(...) ((void)0)
#define Info(...) ((void)0)
#define Warning(...) _Warning(__VA_ARGS__)
#define Error(...) bitgraph::_Error(__VA_ARGS__)
#define LOGG_DEBUG(...) ((void)0)
#define LOGG_INFO(...) ((void)0)
#define LOGG_WARNING(...) bitgraph::_Warning2(__VA_ARGS__)
#define LOGG_ERROR(...) bitgraph::_Error2(__VA_ARGS__)

#endif

#define Logy(...) bitgraph::_Silent(__VA_ARGS__)
#define LOGY(...) bitgraph::_Silent2(__VA_ARGS__)

} // namespace bitgraph	

#endif
