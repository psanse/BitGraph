/**
 * @file test_common.cpp
 * @brief Unit tests for general-purpose utility components.
 *
 * Tests mathematical, string, path, collection, sorting, timing, and
 * fixed-capacity container utilities.
 *
 * @author Pablo San Segundo
 * @date Last updated: 26/09/2026
 */

#include "gtest/gtest.h"

#include "utils/collection_utils.h"
#include "utils/fixed_stack.h"
#include "utils/math_utils.h"
#include "utils/path_utils.h"
#include "utils/sort_utils.h"
#include "utils/string_utils.h"
#include "utils/time_utils.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using bitgraph::utils::FixedStack;

/**
 * @test Verifies that move construction transfers all stored elements.
 */
TEST(FixedStackTest, MoveConstructionTransfersElements)
{
    FixedStack<int> source(3);
    source.push(10);
    source.push(20);

    FixedStack<int> destination = std::move(source);

    EXPECT_EQ(2u, destination.size());
    EXPECT_EQ(20, destination.top());
    EXPECT_EQ(10, destination.bottom());

    EXPECT_TRUE(source.empty());
    EXPECT_EQ(0u, source.capacity());
}

/**
 * @test Verifies forward iteration from bottom to top.
 */
TEST(FixedStackTest, ForwardIteration)
{
    FixedStack<int> stack(3);
    stack.push(10);
    stack.push(20);
    stack.push(30);

    std::vector<int> elements;

    for (int element : stack) {
        elements.push_back(element);
    }

    EXPECT_EQ(
        (std::vector<int>{10, 20, 30}),
        elements);
}

/**
 * @test Verifies reverse iteration from top to bottom.
 */
TEST(FixedStackTest, ReverseIteration)
{
    FixedStack<int> stack(3);
    stack.push(10);
    stack.push(20);
    stack.push(30);

    std::vector<int> elements;

    for (auto iterator = stack.rbegin();
        iterator != stack.rend();
        ++iterator) {
        elements.push_back(*iterator);
    }

    EXPECT_EQ(
        (std::vector<int>{30, 20, 10}),
        elements);
}

/**
 * @test Verifies that reverse iteration over an empty stack is safe.
 */
TEST(FixedStackTest, ReverseIterationOverEmptyStack)
{
    FixedStack<int> stack(3);

    EXPECT_EQ(stack.rbegin(), stack.rend());

    stack.push(10);
    stack.clear();

    EXPECT_EQ(stack.rbegin(), stack.rend());
}

/**
 * @test Verifies basic push, pop, top, bottom, and clear operations.
 */
TEST(FixedStackTest, BasicOperations)
{
    FixedStack<int> stack(10);

    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(0u, stack.size());
    EXPECT_EQ(10u, stack.capacity());

    stack.push(10);
    stack.push(20);
    stack.push(30);

    EXPECT_EQ(3u, stack.size());
    EXPECT_EQ(10, stack.bottom());
    EXPECT_EQ(30, stack.top());

    stack.pop();
    EXPECT_EQ(20, stack.top());

    stack.pop();
    EXPECT_EQ(10, stack.top());

    stack.pop();
    EXPECT_TRUE(stack.empty());

    stack.push(40);
    stack.clear();

    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(10u, stack.capacity());
}

/**
 * @test Verifies the mean and population-standard-deviation functors.
 */
TEST(MathUtilsTest, MeanAndStandardDeviation)
{
    const std::vector<double> values{
        2.7, 2.8, 2.9, 3.0, 3.1, 3.2, 3.3
    };

    const double mean = std::for_each(
        values.begin(),
        values.end(),
        bitgraph::utils::MeanValue{});

    EXPECT_DOUBLE_EQ(3.0, mean);

    const double standard_deviation = std::for_each(
        values.begin(),
        values.end(),
        bitgraph::utils::StdDevValue{ mean });

    double expected = 0.0;

    for (double value : values) {
        const double difference = mean - value;
        expected += difference * difference;
    }

    expected /= static_cast<double>(values.size());
    expected = std::sqrt(expected);

    EXPECT_DOUBLE_EQ(expected, standard_deviation);
}

/**
 * @test Verifies word counting for strings containing letters and numbers.
 */
TEST(StringUtilsTest, CountsWords)
{
    EXPECT_EQ(
        5,
        bitgraph::utils::number_of_words(
            "hello my 2 3 4"));

    EXPECT_EQ(
        4,
        bitgraph::utils::number_of_words(
            "e 1 2 25"));

    EXPECT_EQ(
        0,
        bitgraph::utils::number_of_words(""));
}

/**
 * @test Verifies that append_slash() preserves an existing trailing slash.
 */
TEST(PathUtilsTest, PreservesExistingTrailingSlash)
{
    std::string path = "c:/kk/";

    bitgraph::utils::append_slash(path);

    EXPECT_EQ("c:/kk/", path);
}

#ifdef _WIN32

/**
 * @test Verifies that append_slash() appends the Windows path separator.
 */
TEST(PathUtilsTest, AppendsWindowsSeparator)
{
    std::string absolute_path = "c:\\kk";
    bitgraph::utils::append_slash(absolute_path);

    EXPECT_EQ("c:\\kk\\", absolute_path);

    std::string relative_path = ".\\kk";
    bitgraph::utils::append_slash(relative_path);

    EXPECT_EQ(".\\kk\\", relative_path);
}

#endif

/**
 * @test Verifies detection of equal and unequal collection elements.
 */
TEST(CollectionUtilsTest, DetectsEqualElements)
{
    std::vector<int> values(10, 1);

    EXPECT_TRUE(bitgraph::utils::all_equal(values));

    values.push_back(2);

    EXPECT_FALSE(bitgraph::utils::all_equal(values));
}

/**
 * @test Verifies that an empty collection satisfies all_equal().
 */
TEST(CollectionUtilsTest, EmptyCollectionIsAllEqual)
{
    const std::vector<int> values;

    EXPECT_TRUE(bitgraph::utils::all_equal(values));
}

/**
 * @test Verifies elapsed monotonic-time measurement.
 */
TEST(TimeUtilsTest, MeasuresElapsedTime)
{
    const auto start_time =
        std::chrono::steady_clock::now();

    std::this_thread::sleep_for(
        std::chrono::milliseconds(50));

    const double elapsed =
        bitgraph::utils::elapsed_time(start_time);

    EXPECT_GE(elapsed, 0.04);

    // Generous bound to avoid failures caused by scheduling delays.
    EXPECT_LT(elapsed, 2.0);
}

/**
 * @test Verifies insertion into arrays sorted by non-increasing scores.
 */
TEST(SortUtilsTest, DISABLED_InsertsByNonIncreasingScore)
{
    constexpr int size = 4;

    int items[size]{ 0, 1, 2, 0 };
    int scores[size]{ 30, 20, 5, 0 };

    const int position =
        bitgraph::utils::insert_ordered_non_increasing(
            items,
            scores,
            size,
            3,
            21);

    EXPECT_EQ(1, position);

    EXPECT_EQ(0, items[0]);
    EXPECT_EQ(3, items[1]);
    EXPECT_EQ(1, items[2]);
    EXPECT_EQ(2, items[3]);

    EXPECT_EQ(30, scores[0]);
    EXPECT_EQ(21, scores[1]);
    EXPECT_EQ(20, scores[2]);
    EXPECT_EQ(5, scores[3]);
}