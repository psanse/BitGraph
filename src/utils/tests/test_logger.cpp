/**
 * @file test_logger.cpp
 * @brief Unit tests for the lightweight BitGraph logger.
 *
 * These tests capture the standard error stream and verify printf-style,
 * stream-style, container, and public macro logging.
 *
 * The logger is based on Logy v1.2 by Giovanni Squillero.
 *
 * @author Pablo San Segundo
 * @date Created: 06/11/2024
 * @date Last updated: 23/09/2026
 */

#include "utils/logger.h"

#include "gtest/gtest.h"

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace {

    template<class Callable>
    std::string capture_stderr(Callable&& callable)
    {
        testing::internal::CaptureStderr();

        std::forward<Callable>(callable)();

        return testing::internal::GetCapturedStderr();
    }

    void expect_contains(
        const std::string& output,
        const std::string& text)
    {
        EXPECT_NE(
            std::string::npos,
            output.find(text))
            << "Expected text: " << text
            << "\nCaptured output: " << output;
    }

} // unnamed namespace

/**
 * @test Verifies printf-style debug logging.
 */
TEST(LoggerTest, writes_printf_debug_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::debug_printf(
            "%s: %d",
            "debug message",
            123);
        });

    expect_contains(output, "DEBUG:");
    expect_contains(output, "debug message: 123");
}

/**
 * @test Verifies printf-style informational logging.
 */
TEST(LoggerTest, writes_printf_info_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::info_printf(
            "%s: %d",
            "info message",
            456);
        });

    expect_contains(output, "INFO:");
    expect_contains(output, "info message: 456");
}

/**
 * @test Verifies printf-style warning logging.
 */
TEST(LoggerTest, writes_printf_warning_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::warning_printf(
            "%s: %d",
            "warning message",
            789);
        });

    expect_contains(output, "WARNING:");
    expect_contains(output, "warning message: 789");
}

/**
 * @test Verifies printf-style error logging.
 */
TEST(LoggerTest, writes_printf_error_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::error_printf(
            "%s: %d",
            "error message",
            199);
        });

    expect_contains(output, "ERROR:");
    expect_contains(output, "error message: 199");
}

/**
 * @test Verifies stream-style debug logging with heterogeneous arguments.
 */
TEST(LoggerTest, writes_stream_debug_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::debug_stream(
            "debug message",
            123,
            4.5);
        });

    expect_contains(output, "DEBUG:");
    expect_contains(output, "debug message");
    expect_contains(output, "123");
    expect_contains(output, "4.5");
}

/**
 * @test Verifies stream-style informational logging.
 */
TEST(LoggerTest, writes_stream_info_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::info_stream(
            "info message",
            456);
        });

    expect_contains(output, "INFO:");
    expect_contains(output, "info message");
    expect_contains(output, "456");
}

/**
 * @test Verifies stream-style warning logging.
 */
TEST(LoggerTest, writes_stream_warning_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::warning_stream(
            "warning message",
            789);
        });

    expect_contains(output, "WARNING:");
    expect_contains(output, "warning message");
    expect_contains(output, "789");
}

/**
 * @test Verifies stream-style error logging.
 */
TEST(LoggerTest, writes_stream_error_message)
{
    const std::string output = capture_stderr([] {
        bitgraph::utils::detail::error_stream(
            "error message",
            199);
        });

    expect_contains(output, "ERROR:");
    expect_contains(output, "error message");
    expect_contains(output, "199");
}

/**
 * @test Verifies expansion of supported collection types.
 */
TEST(LoggerTest, expands_supported_collections)
{
    const std::vector<int> values{ 1, 2, 3 };

    const std::string output = capture_stderr([&values] {
        bitgraph::utils::detail::info_stream(
            "values:",
            values,
            std::initializer_list<int>{4, 5});
        });

    expect_contains(output, "INFO:");
    expect_contains(output, "values:");
    expect_contains(output, "[ 1 2 3 ]");
    expect_contains(output, "[ 4 5 ]");
}

/**
 * @test Verifies the public printf-style error macro.
 */
TEST(LoggerTest, public_printf_error_macro)
{
    const std::string output = capture_stderr([] {
        LOG_ERROR(
            "public error: %d",
            42);
        });

    expect_contains(output, "ERROR:");
    expect_contains(output, "public error: 42");
}

/**
 * @test Verifies the public stream-style error macro.
 */
TEST(LoggerTest, public_stream_error_macro)
{
    const std::string output = capture_stderr([] {
        LOGG_ERROR(
            "public error:",
            42);
        });

    expect_contains(output, "ERROR:");
    expect_contains(output, "public error:");
    expect_contains(output, "42");
}

/**
 * @test Verifies the public printf-style debug macro.
 *
 * Debug output is produced only when the debug logging level is enabled.
 */
TEST(LoggerTest, public_printf_debug_macro)
{
    const std::string output = capture_stderr([] {
        LOG_DEBUG("public debug: %d", 42);
    });

#if defined(BITGRAPH_LOG_DEBUG) || \
    defined(LOGGER_DEBUG_LEVEL)

    expect_contains(output, "DEBUG:");
    expect_contains(output, "public debug: 42");
#else
    EXPECT_TRUE(output.empty());
#endif
}

/**
 * @test Verifies the public printf-style informational macro.
 *
 * Informational output is produced at verbose and debug levels.
 */
TEST(LoggerTest, public_printf_info_macro)
{
    const std::string output = capture_stderr([] {
        LOG_INFO("public info: %d", 42);
    });

#if defined(BITGRAPH_LOG_DEBUG) ||          \
    defined(LOGGER_DEBUG_LEVEL) ||          \
    defined(BITGRAPH_LOG_VERBOSE) ||        \
    defined(LOGGER_VERBOSE_LEVEL)

    expect_contains(output, "INFO:");
    expect_contains(output, "public info: 42");
#else
    EXPECT_TRUE(output.empty());
#endif
}

/**
 * @test Verifies the public printf-style warning macro.
 *
 * Warning output is disabled only when the error-only level is selected.
 */
TEST(LoggerTest, public_printf_warning_macro)
{
    const std::string output = capture_stderr([] {
        LOG_WARNING("public warning: %d", 42);
    });

#if defined(BITGRAPH_LOG_ERROR_ONLY) || \
    defined(LOGGER_ERROR_LEVEL)

    EXPECT_TRUE(output.empty());
#else
    expect_contains(output, "WARNING:");
    expect_contains(output, "public warning: 42");
#endif
}



