/**
 * @file logger.h
 * @brief Provides lightweight header-only logging utilities.
 *
 * This header is based on Logy v1.2 by Giovanni Squillero and provides
 * printf-style and stream-style logging at debug, informational, warning,
 * and error levels.
 *
 * Log levels can be selected at compile time. Warning and error messages are
 * enabled by default.
 *
 * @note Container expansion currently supports std::vector and
 *       std::initializer_list.
 * @note Logging operations are serialized and may be used concurrently from
 *       multiple threads.
 *
 * Original Logy implementation:
 * Giovanni Squillero, Politecnico di Torino, 2018.
 * https://github.com/squillero/logy
 *
 * The original implementation was dedicated to the public domain.
 *
 * @author Giovanni Squillero
 * @author Pablo San Segundo
 * @date Original implementation: 2018
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_UTILS_LOGGER_H
#define BITGRAPH_UTILS_LOGGER_H

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <initializer_list>
#include <mutex>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace bitgraph {
    namespace utils {
        namespace detail {

            /**
             * @brief Returns the mutex shared by all logging operations.
             */
            inline std::mutex& log_mutex()
            {
                static std::mutex mutex;
                return mutex;
            }

            /**
             * @brief Converts calendar time to local time using the safest available API.
             *
             * @param calendar_time Calendar time to convert.
             * @param local_time Destination structure.
             * @return `true` if the conversion succeeds; otherwise, `false`.
             */
            inline bool get_local_time(
                std::time_t calendar_time,
                std::tm& local_time) noexcept
            {
#if defined(_MSC_VER)
                return ::localtime_s(
                    &local_time,
                    &calendar_time) == 0;

#elif defined(__unix__) || defined(__APPLE__)
                return ::localtime_r(
                    &calendar_time,
                    &local_time) != nullptr;

#else
                /*
                 * This fallback is protected by the logger mutex when called from the
                 * logging functions. Other calls to std::localtime() elsewhere in the
                 * program may still require synchronization.
                 */
                const std::tm* result =
                    std::localtime(&calendar_time);

                if (result == nullptr) {
                    return false;
                }

                local_time = *result;
                return true;
#endif
            }

            /**
             * @brief Writes a timestamp and severity label.
             *
             * @param severity Severity label, including surrounding punctuation.
             *
             * @pre The logging mutex is locked by the caller.
             */
            inline void write_header_unlocked(const char* severity) noexcept
            {
                char timestamp[100] = "";

                const std::time_t calendar_time =
                    std::time(nullptr);

                std::tm local_time{};

                if (get_local_time(calendar_time, local_time)) {
                    std::strftime(
                        timestamp,
                        sizeof(timestamp),
                        "[%H:%M:%S]-[%d/%b/%Y]",
                        &local_time);
                }
                else {
                    std::snprintf(
                        timestamp,
                        sizeof(timestamp),
                        "[unknown time]");
                }

                std::fprintf(
                    stderr,
                    "%s%s",
                    timestamp,
                    severity);
            }

            /**
             * @brief Trait identifying containers supported by tag_expand().
             */
            template<typename T>
            struct is_expandable_range : std::false_type {};

            template<typename T, typename Allocator>
            struct is_expandable_range<
                std::vector<T, Allocator>> : std::true_type {
            };

            template<typename T>
            struct is_expandable_range<
                std::initializer_list<T>> : std::true_type {
            };

            /*
             * Forward declarations are needed because range expansion recursively invokes
             * tag_expand() for the contained elements.
             */

            template<typename T>
            typename std::enable_if<
                is_expandable_range<
                typename std::decay<T>::type>::value,
                std::string>::type
                tag_expand(const T& value);

            template<typename T>
            typename std::enable_if<
                !is_expandable_range<
                typename std::decay<T>::type>::value,
                std::string>::type
                tag_expand(const T& value);

            /**
             * @brief Converts a supported range to a printable string.
             */
            template<typename T>
            typename std::enable_if<
                is_expandable_range<
                typename std::decay<T>::type>::value,
                std::string>::type
                tag_expand(const T& values)
            {
                std::ostringstream stream;
                stream << '[';

                for (const auto& value : values) {
                    stream << ' ' << tag_expand(value);
                }

                stream << " ]";
                return stream.str();
            }

            /**
             * @brief Converts a stream-insertable value to a string.
             */
            template<typename T>
            typename std::enable_if<
                !is_expandable_range<
                typename std::decay<T>::type>::value,
                std::string>::type
                tag_expand(const T& value)
            {
                std::ostringstream stream;
                stream << value;
                return stream.str();
            }

            /**
             * @brief Terminates recursive stream-style argument expansion.
             */
            inline void append_arguments(std::ostringstream&)
            {}

            /**
             * @brief Appends stream-style logging arguments recursively.
             */
            template<typename First, typename... Rest>
            void append_arguments(
                std::ostringstream& stream,
                First&& first,
                Rest&&... rest)
            {
                stream << ' '
                    << tag_expand(
                        std::forward<First>(first));

                append_arguments(
                    stream,
                    std::forward<Rest>(rest)...);
            }

            /**
             * @brief Implements stream-style logging.
             */
            template<typename... Args>
            void log_stream(
                const char* severity,
                Args&&... args)
            {
                /*
                 * Build the complete message before acquiring the output lock so that the
                 * critical section remains short.
                 */
                std::ostringstream message;
                append_arguments(
                    message,
                    std::forward<Args>(args)...);

                const std::string text = message.str();

                std::lock_guard<std::mutex> lock{
                    log_mutex()
                };

                write_header_unlocked(severity);
                std::fputs(text.c_str(), stderr);
                std::fputc('\n', stderr);
                std::fflush(stderr);
            }

            /**
             * @brief Implements printf-style logging.
             */
            inline void log_printf(
                const char* severity,
                const char* format,
                std::va_list arguments)
            {
                std::lock_guard<std::mutex> lock{
                    log_mutex()
                };

                write_header_unlocked(severity);
                std::fputc(' ', stderr);
                std::vfprintf(stderr, format, arguments);
                std::fputc('\n', stderr);
                std::fflush(stderr);
            }

            /*
             * Portable printf-format checking.
             *
             * GCC, Clang, and Intel compilers validate the format string and arguments.
             * MSVC does not support this attribute.
             */
#if defined(__GNUC__) || defined(__clang__) || \
    defined(__INTEL_COMPILER)
#define BITGRAPH_PRINTF_ATTRIBUTE(format_index, argument_index) \
    __attribute__((format(printf, format_index, argument_index)))
#else
#define BITGRAPH_PRINTF_ATTRIBUTE(format_index, argument_index)
#endif

            inline void debug_printf(
                const char* format,
                ...) BITGRAPH_PRINTF_ATTRIBUTE(1, 2);

            inline void info_printf(
                const char* format,
                ...) BITGRAPH_PRINTF_ATTRIBUTE(1, 2);

            inline void warning_printf(
                const char* format,
                ...) BITGRAPH_PRINTF_ATTRIBUTE(1, 2);

            inline void error_printf(
                const char* format,
                ...) BITGRAPH_PRINTF_ATTRIBUTE(1, 2);

            /**
             * @brief Writes a printf-style debug message.
             */
            inline void debug_printf(const char* format, ...)
            {
                std::va_list arguments;
                va_start(arguments, format);
                log_printf(" DEBUG:", format, arguments);
                va_end(arguments);
            }

            /**
             * @brief Writes a printf-style informational message.
             */
            inline void info_printf(const char* format, ...)
            {
                std::va_list arguments;
                va_start(arguments, format);
                log_printf(" INFO:", format, arguments);
                va_end(arguments);
            }

            /**
             * @brief Writes a printf-style warning message.
             */
            inline void warning_printf(const char* format, ...)
            {
                std::va_list arguments;
                va_start(arguments, format);
                log_printf(" WARNING:", format, arguments);
                va_end(arguments);
            }

            /**
             * @brief Writes a printf-style error message.
             */
            inline void error_printf(const char* format, ...)
            {
                std::va_list arguments;
                va_start(arguments, format);
                log_printf(" ERROR:", format, arguments);
                va_end(arguments);
            }

            /**
             * @brief Writes a stream-style debug message.
             */
            template<typename... Args>
            void debug_stream(Args&&... args)
            {
                log_stream(
                    " DEBUG:",
                    std::forward<Args>(args)...);
            }

            /**
             * @brief Writes a stream-style informational message.
             */
            template<typename... Args>
            void info_stream(Args&&... args)
            {
                log_stream(
                    " INFO:",
                    std::forward<Args>(args)...);
            }

            /**
             * @brief Writes a stream-style warning message.
             */
            template<typename... Args>
            void warning_stream(Args&&... args)
            {
                log_stream(
                    " WARNING:",
                    std::forward<Args>(args)...);
            }

            /**
             * @brief Writes a stream-style error message.
             */
            template<typename... Args>
            void error_stream(Args&&... args)
            {
                log_stream(
                    " ERROR:",
                    std::forward<Args>(args)...);
            }

#undef BITGRAPH_PRINTF_ATTRIBUTE

        } // namespace detail
    } // namespace utils
} // namespace bitgraph

/*
 * Compile-time logging-level selection.
 *
 * Preferred configuration macros:
 *
 *   BITGRAPH_LOG_DEBUG
 *   BITGRAPH_LOG_VERBOSE
 *   BITGRAPH_LOG_ERROR_ONLY
 *
 * Historical LOGGER_* macros are retained for compatibility.
 */

#if defined(BITGRAPH_LOG_DEBUG) || \
    defined(LOGGER_DEBUG_LEVEL)

 /* All messages enabled. */

#define Debug(...)                                                   \
    ::bitgraph::utils::detail::debug_printf(__VA_ARGS__)

#define Info(...)                                                    \
    ::bitgraph::utils::detail::info_printf(__VA_ARGS__)

#define Warning(...)                                                 \
    ::bitgraph::utils::detail::warning_printf(__VA_ARGS__)

#define Error(...)                                                   \
    ::bitgraph::utils::detail::error_printf(__VA_ARGS__)

#define LOGG_DEBUG(...)                                              \
    ::bitgraph::utils::detail::debug_stream(__VA_ARGS__)

#define LOGG_INFO(...)                                               \
    ::bitgraph::utils::detail::info_stream(__VA_ARGS__)

#define LOGG_WARNING(...)                                            \
    ::bitgraph::utils::detail::warning_stream(__VA_ARGS__)

#define LOGG_ERROR(...)                                              \
    ::bitgraph::utils::detail::error_stream(__VA_ARGS__)

#elif defined(BITGRAPH_LOG_VERBOSE) || \
      defined(LOGGER_VERBOSE_LEVEL)

 /* Informational, warning, and error messages enabled. */

#define Debug(...) ((void)0)

#define Info(...)                                                    \
    ::bitgraph::utils::detail::info_printf(__VA_ARGS__)

#define Warning(...)                                                 \
    ::bitgraph::utils::detail::warning_printf(__VA_ARGS__)

#define Error(...)                                                   \
    ::bitgraph::utils::detail::error_printf(__VA_ARGS__)

#define LOGG_DEBUG(...) ((void)0)

#define LOGG_INFO(...)                                               \
    ::bitgraph::utils::detail::info_stream(__VA_ARGS__)

#define LOGG_WARNING(...)                                            \
    ::bitgraph::utils::detail::warning_stream(__VA_ARGS__)

#define LOGG_ERROR(...)                                              \
    ::bitgraph::utils::detail::error_stream(__VA_ARGS__)

#elif defined(BITGRAPH_LOG_ERROR_ONLY) || \
      defined(LOGGER_ERROR_LEVEL)

 /* Only error messages enabled. */

#define Debug(...)   ((void)0)
#define Info(...)    ((void)0)
#define Warning(...) ((void)0)

#define Error(...)                                                   \
    ::bitgraph::utils::detail::error_printf(__VA_ARGS__)

#define LOGG_DEBUG(...)   ((void)0)
#define LOGG_INFO(...)    ((void)0)
#define LOGG_WARNING(...) ((void)0)

#define LOGG_ERROR(...)                                              \
    ::bitgraph::utils::detail::error_stream(__VA_ARGS__)

#else

 /* Warning and error messages enabled by default. */

#define Debug(...) ((void)0)
#define Info(...)  ((void)0)

#define Warning(...)                                                 \
    ::bitgraph::utils::detail::warning_printf(__VA_ARGS__)

#define Error(...)                                                   \
    ::bitgraph::utils::detail::error_printf(__VA_ARGS__)

#define LOGG_DEBUG(...) ((void)0)
#define LOGG_INFO(...)  ((void)0)

#define LOGG_WARNING(...)                                            \
    ::bitgraph::utils::detail::warning_stream(__VA_ARGS__)

#define LOGG_ERROR(...)                                              \
    ::bitgraph::utils::detail::error_stream(__VA_ARGS__)

#endif

/*
 * Legacy logger macro compatibility.
 *
 * LOG_* uses printf-style formatting.
 * LOGG_* uses stream-style argument expansion.
 */

#define LOG_ERROR(...)   Error(__VA_ARGS__)
#define LOG_WARNING(...) Warning(__VA_ARGS__)
#define LOG_INFO(...)    Info(__VA_ARGS__)
#define LOG_PRINT(...)   Info(__VA_ARGS__)
#define LOG_DEBUG(...)   Debug(__VA_ARGS__)

#define LOG_PAK()                                                    \
    Info("press any key to continue")

#define LOG_LINE()                                                   \
    Info("-------------------------")

 /*
  * Historical silent logger macros. Arguments are intentionally not evaluated.
  */
#define Logy(...) ((void)0)
#define LOGY(...) ((void)0)

#endif // BITGRAPH_UTILS_LOGGER_H