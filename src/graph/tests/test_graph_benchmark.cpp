/**
 * @file test_benchmark.cpp
 * @brief Unit tests for the Benchmark graph-dataset manager.
 *
 * These tests verify instance registration, reference-value lookup, path
 * construction, formatted output, and removal of benchmark instances.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#include "graph/graph_benchmark.h"
#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <utility>	                    // for std::move


using namespace bitgraph;

namespace {

    /**
     * @brief Concrete benchmark subclass used by the unit tests.
     *
     * GraphBenchmark is the public alias for
     * BasicGraphBenchmark<int>. Its constructor is protected because the
     * benchmark class is intended to serve as a base for concrete graph datasets.
     * This test subclass exposes that constructor for test setup.
     */
	class TestGraphBenchmark final : public bitgraph::GraphBenchmark {
	public:
		explicit TestGraphBenchmark(std::string path)
			: GraphBenchmark(std::move(path))
		{}
	};


	std::string expected_filename(
		const std::string& path,
		const std::string& filename)
	{
#ifdef _WIN32
		return path + '\\' + filename;
#else
		return path + '/' + filename;
#endif
	}

} // unnamed namespace



/**
 * @test Verifies instance registration and reference-value lookup.
 */
TEST(GraphBenchmarkTest, registers_instances_and_values)
{
    const std::string base_path{ "benchmark_path" };
    TestGraphBenchmark benchmark{ base_path };

    benchmark.add_test("file_1");
    benchmark.add_test("file_2");
    benchmark.add_test("file_3");
    benchmark.add_test("file_4", 4);
    benchmark.add_test("file_5", 5);

    EXPECT_EQ(5u, benchmark.number_of_instances());
    EXPECT_FALSE(benchmark.is_empty());

    const std::string file4 =
        expected_filename(base_path, "file_4");

    const auto& values = benchmark.values();
    const auto iterator = values.find(file4);

    ASSERT_NE(values.end(), iterator);
    EXPECT_EQ(4, iterator->second);
    EXPECT_EQ(4, benchmark.get_value(file4));

    const std::string file3 =
        expected_filename(base_path, "file_3");

    EXPECT_EQ(-1, benchmark.get_value(file3));
}

/**
 * @test Verifies that an empty base path leaves filenames unchanged.
 */
TEST(GraphBenchmarkTest, accepts_empty_base_path)
{
    TestGraphBenchmark benchmark{ "" };

    benchmark.add_test("file_1", 10);

    ASSERT_EQ(1u, benchmark.number_of_instances());
    EXPECT_EQ("file_1", benchmark.filenames().front());
    EXPECT_EQ(10, benchmark.get_value("file_1"));
}

/**
 * @test Verifies that clear_instances removes filenames and values while
 *       preserving the base path.
 */
TEST(GraphBenchmarkTest, clears_instances)
{
    const std::string base_path{ "benchmark_path" };
    TestGraphBenchmark benchmark{ base_path };

    benchmark.add_test("file_1");
    benchmark.add_test("file_2", 2);

    benchmark.clear_instances();

    EXPECT_TRUE(benchmark.is_empty());
    EXPECT_EQ(0u, benchmark.number_of_instances());
    EXPECT_TRUE(benchmark.filenames().empty());
    EXPECT_TRUE(benchmark.values().empty());
    EXPECT_EQ(base_path, benchmark.path());
}

/**
 * @test Verifies the textual representation of benchmark instances.
 */
TEST(GraphBenchmarkTest, prints_instances)
{
    const std::string base_path{ "benchmark_path" };
    TestGraphBenchmark benchmark{ base_path };

    benchmark.add_test("file_1");
    benchmark.add_test("file_2", 2);

    const std::string file1 =
        expected_filename(base_path, "file_1");

    const std::string file2 =
        expected_filename(base_path, "file_2");

    std::ostringstream output;
    output << benchmark;

    EXPECT_EQ(
        file1 + '\n' +
        file2 + ":2\n",
        output.str());
}