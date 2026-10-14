/**
 * @file test_algorithm_info.cpp
 * @brief Unit tests for AlgorithmParameters and BasicAlgorithmInfo.
 *
 * Tests parameter initialization, concrete derived parameter storage,
 * execution timers, selective clearing, the clearResults() customization
 * hook, and report generation.
 *
 * @author Pablo San Segundo
 * @date Last updated: 26/09/2026
 */

#include "gtest/gtest.h"

#include "graph/algorithms/algorithm_info.h"

#include <chrono>
#include <cstddef>
#include <limits>
#include <sstream>
#include <string>
#include <thread>

namespace {

    /**
     * @brief Algorithm-specific parameter type used by the tests.
     */
    struct TestParameters : bitgraph::AlgorithmParameters {
        int algorithm_option = 7;
        bool use_reduction = true;

        /**
         * @brief Restores only the test-specific parameters.
         */
        void reset_derived() override
        {
            algorithm_option = 0;
            use_reduction = false;
        }
    };

    /**
     * @brief Concrete information type used to test derived result clearing.
     */
    class TestAlgorithmInfo final :
        public bitgraph::BasicAlgorithmInfo<TestParameters> {
    public:
        using Base =
            bitgraph::BasicAlgorithmInfo<TestParameters>;

        using Base::Base;

        void result_value(int value) noexcept
        {
            result_value_ = value;
        }

        int result_value() const noexcept
        {
            return result_value_;
        }

    protected:
        /**
         * @brief Clears the test-specific result fields.
         */
        void clear_results() noexcept override
        {
            result_value_ = 0;
        }

    private:
        int result_value_ = 0;
    };

    class AlgorithmInfoTest : public ::testing::Test {
    protected:
        using Info = TestAlgorithmInfo;
        using Phase = Info::phase_t;

        TestParameters make_parameters() const
        {
            TestParameters parameters;

            parameters.name = "test-instance";
            parameters.N = 100;
            parameters.M = 250;
            parameters.time_out = 60.0;
            parameters.heuristic_time_out = 5.0;
            parameters.num_threads = 4;
            parameters.algorithm_option = 42;
            parameters.use_reduction = true;

            return parameters;
        }
    };

} // unnamed namespace

/**
 * @test Verifies the default values of AlgorithmParameters.
 */
TEST(AlgorithmParametersTest, default_construction)
{
    bitgraph::AlgorithmParameters parameters;

    EXPECT_TRUE(parameters.name.empty());
    EXPECT_EQ(0u, parameters.N);
    EXPECT_EQ(0u, parameters.M);

    EXPECT_EQ(
        std::numeric_limits<double>::max(),
        parameters.time_out);

    EXPECT_EQ(
        std::numeric_limits<double>::max(),
        parameters.heuristic_time_out);

    EXPECT_EQ(1, parameters.num_threads);
    EXPECT_FALSE(parameters.unrolled);
    EXPECT_DOUBLE_EQ(0.0, parameters.parsing_time);
}

/**
 * @test Verifies that reset_base() restores only the common parameters.
 */
TEST(AlgorithmParametersTest, reset_base_parameters)
{
    TestParameters parameters;

    parameters.name = "instance";
    parameters.N = 10;
    parameters.M = 20;
    parameters.time_out = 100.0;
    parameters.heuristic_time_out = 10.0;
    parameters.num_threads = 8;
    parameters.unrolled = true;
    parameters.parsing_time = 1.5;

    parameters.algorithm_option = 99;
    parameters.use_reduction = true;

    parameters.reset_base();

    EXPECT_TRUE(parameters.name.empty());
    EXPECT_EQ(0u, parameters.N);
    EXPECT_EQ(0u, parameters.M);

    EXPECT_EQ(
        std::numeric_limits<double>::max(),
        parameters.time_out);

    EXPECT_EQ(
        std::numeric_limits<double>::max(),
        parameters.heuristic_time_out);

    EXPECT_EQ(1, parameters.num_threads);
    EXPECT_FALSE(parameters.unrolled);
    EXPECT_DOUBLE_EQ(0.0, parameters.parsing_time);

    // Derived parameters are deliberately unchanged.
    EXPECT_EQ(99, parameters.algorithm_option);
    EXPECT_TRUE(parameters.use_reduction);
}

/**
 * @test Verifies that reset_derived() modifies only derived parameters.
 */
TEST(AlgorithmParametersTest, reset_derived_parameters)
{
    TestParameters parameters;

    parameters.name = "instance";
    parameters.N = 10;
    parameters.M = 20;
    parameters.algorithm_option = 99;
    parameters.use_reduction = true;

    parameters.reset_derived();

    EXPECT_EQ("instance", parameters.name);
    EXPECT_EQ(10u, parameters.N);
    EXPECT_EQ(20u, parameters.M);

    EXPECT_EQ(0, parameters.algorithm_option);
    EXPECT_FALSE(parameters.use_reduction);
}

/**
 * @test Verifies parsing-time measurement before AlgorithmInfo construction.
 */
TEST(AlgorithmParametersTest, parsing_timer)
{
    TestParameters parameters;

    parameters.start_parsing_timer();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(30));

    const double elapsed =
        parameters.read_parsing_timer();

    EXPECT_GE(elapsed, 0.02);
    EXPECT_LT(elapsed, 2.0);
    EXPECT_DOUBLE_EQ(elapsed, parameters.parsing_time);
}

/**
 * @test Verifies that the concrete parameter type is stored without slicing.
 */
TEST_F(AlgorithmInfoTest, preserves_concrete_parameter_type)
{
    const TestParameters parameters = make_parameters();
    const Info info(parameters);

    EXPECT_EQ("test-instance", info.name());
    EXPECT_EQ(100u, info.num_vertices());
    EXPECT_EQ(250u, info.num_edges());
    EXPECT_DOUBLE_EQ(60.0, info.time_out());
    EXPECT_DOUBLE_EQ(5.0, info.heuristic_time_out());
    EXPECT_EQ(4, info.number_of_threads());

    EXPECT_EQ(42, info.parameters().algorithm_option);
    EXPECT_TRUE(info.parameters().use_reduction);
}

/**
 * @test Verifies measurement of the search phase.
 */
TEST_F(AlgorithmInfoTest, search_timer)
{
    Info info;

    info.start_timer(Phase::SEARCH);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(30));

    const double elapsed =
        info.read_timer(Phase::SEARCH);

    EXPECT_GE(elapsed, 0.02);
    EXPECT_LT(elapsed, 2.0);
    EXPECT_DOUBLE_EQ(elapsed, info.search_time());
}

/**
 * @test Verifies measurement of the preprocessing phase.
 */
TEST_F(AlgorithmInfoTest, preprocessing_timer)
{
    Info info;

    info.start_timer(Phase::PREPROC);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(30));

    const double elapsed =
        info.read_timer(Phase::PREPROC);

    EXPECT_GE(elapsed, 0.02);
    EXPECT_LT(elapsed, 2.0);
    EXPECT_DOUBLE_EQ(elapsed, info.preprocessing_time());
}

/**
 * @test Verifies measurement of the last-incumbent phase.
 */
TEST_F(AlgorithmInfoTest, incumbent_timer)
{
    Info info;

    info.start_timer(Phase::LAST_INCUMBENT);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(30));

    const double elapsed =
        info.read_timer(Phase::LAST_INCUMBENT);

    EXPECT_GE(elapsed, 0.02);
    EXPECT_LT(elapsed, 2.0);
    EXPECT_DOUBLE_EQ(elapsed, info.incumbent_time());
}

/**
 * @test Verifies that the parsing timer is delegated to the parameter object.
 */
TEST_F(AlgorithmInfoTest, parsing_timer)
{
    Info info;

    info.start_timer(Phase::PARSE);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(30));

    const double elapsed =
        info.read_timer(Phase::PARSE);

    EXPECT_GE(elapsed, 0.02);
    EXPECT_LT(elapsed, 2.0);
    EXPECT_DOUBLE_EQ(elapsed, info.parsing_time());
}

/**
 * @test Verifies that an individual execution timer can be cleared.
 */
TEST_F(AlgorithmInfoTest, clear_individual_timer)
{
    Info info;

    info.start_timer(Phase::SEARCH);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(10));
    info.read_timer(Phase::SEARCH);

    ASSERT_GT(info.search_time(), 0.0);

    info.clear_timer(Phase::SEARCH);

    EXPECT_DOUBLE_EQ(0.0, info.search_time());
}

/**
 * @test Verifies that execution timers are cleared without clearing parsing
 *       time.
 */
TEST_F(AlgorithmInfoTest, clear_execution_timers_preserves_parsing)
{
    Info info;

    info.start_timer(Phase::PARSE);
    info.start_timer(Phase::PREPROC);
    info.start_timer(Phase::SEARCH);
    info.start_timer(Phase::LAST_INCUMBENT);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(10));

    info.read_timer(Phase::PARSE);
    info.read_timer(Phase::PREPROC);
    info.read_timer(Phase::SEARCH);
    info.read_timer(Phase::LAST_INCUMBENT);

    ASSERT_GT(info.parsing_time(), 0.0);

    info.clear_execution_timers();

    EXPECT_DOUBLE_EQ(0.0, info.preprocessing_time());
    EXPECT_DOUBLE_EQ(0.0, info.search_time());
    EXPECT_DOUBLE_EQ(0.0, info.incumbent_time());

    EXPECT_GT(info.parsing_time(), 0.0);
}

/**
 * @test Verifies that clear(true) preserves base parameters while clearing
 *       derived parameters, execution timers, and derived results.
 */
TEST_F(AlgorithmInfoTest, selective_clear_preserves_base_parameters)
{
    TestParameters parameters = make_parameters();

    parameters.parsing_time = 1.25;

    Info info(parameters);
    info.result_value(100);

    info.start_timer(Phase::SEARCH);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(10));
    info.read_timer(Phase::SEARCH);

    info.clear(true);

    // Base parameters and parsing information are preserved.
    EXPECT_EQ("test-instance", info.name());
    EXPECT_EQ(100u, info.num_vertices());
    EXPECT_EQ(250u, info.num_edges());
    EXPECT_DOUBLE_EQ(60.0, info.time_out());
    EXPECT_DOUBLE_EQ(5.0, info.heuristic_time_out());
    EXPECT_EQ(4, info.number_of_threads());
    EXPECT_DOUBLE_EQ(1.25, info.parsing_time());

    // Derived parameters are cleared.
    EXPECT_EQ(0, info.parameters().algorithm_option);
    EXPECT_FALSE(info.parameters().use_reduction);

    // Execution timers and derived results are cleared.
    EXPECT_DOUBLE_EQ(0.0, info.search_time());
    EXPECT_EQ(0, info.result_value());
}

/**
 * @test Verifies that clear(false) restores the complete object state.
 */
TEST_F(AlgorithmInfoTest, complete_clear)
{
    TestParameters parameters = make_parameters();
    parameters.parsing_time = 1.25;

    Info info(parameters);
    info.result_value(100);

    info.clear(false);

    EXPECT_TRUE(info.name().empty());
    EXPECT_EQ(0u, info.num_vertices());
    EXPECT_EQ(0u, info.num_edges());

    EXPECT_EQ(
        std::numeric_limits<double>::max(),
        info.time_out());

    EXPECT_EQ(
        std::numeric_limits<double>::max(),
        info.heuristic_time_out());

    EXPECT_EQ(1, info.number_of_threads());
    EXPECT_DOUBLE_EQ(0.0, info.parsing_time());

    EXPECT_EQ(0, info.parameters().algorithm_option);
    EXPECT_FALSE(info.parameters().use_reduction);
    EXPECT_EQ(0, info.result_value());
}

/**
 * @test Verifies configuration of the timeout-check interval.
 */
TEST_F(AlgorithmInfoTest, timeout_check_interval)
{
    Info info;

    EXPECT_EQ(
        100u,
        info.recursion_calls_per_timeout_check());

    info.recursion_calls_per_timeout_check(250);

    EXPECT_EQ(
        250u,
        info.recursion_calls_per_timeout_check());
}

/**
 * @test Verifies parameter output.
 */
TEST_F(AlgorithmInfoTest, print_parameters)
{
    const Info info(make_parameters());

    std::ostringstream output;
    info.print_params(output);

    EXPECT_NE(
        std::string::npos,
        output.str().find("test-instance"));

    EXPECT_NE(
        std::string::npos,
        output.str().find("NUM_THREADS"));
}

/**
 * @test Verifies timer output.
 */
TEST_F(AlgorithmInfoTest, print_timers)
{
    const Info info(make_parameters());

    std::ostringstream output;
    info.print_timers(output);

    EXPECT_NE(
        std::string::npos,
        output.str().find("TIME_PARSE"));

    EXPECT_NE(
        std::string::npos,
        output.str().find("TIME_SEARCH"));
}

/**
 * @test Verifies table-format report output.
 */
TEST_F(AlgorithmInfoTest, print_table_report)
{
    const Info info(make_parameters());

    std::ostringstream output;

    info.print_report(
        output,
        Info::report_t::TABLE,
        false);

    EXPECT_NE(
        std::string::npos,
        output.str().find("test-instance"));

    EXPECT_NE(
        std::string::npos,
        output.str().find('\t'));
}

/**
 * @test Verifies verbose-format report output.
 */
TEST_F(AlgorithmInfoTest, print_verbose_report)
{
    const Info info(make_parameters());

    std::ostringstream output;

    info.print_report(
        output,
        Info::report_t::VERBOSE,
        false);

    EXPECT_NE(
        std::string::npos,
        output.str().find("NAME:"));

    EXPECT_NE(
        std::string::npos,
        output.str().find("TIME_SEARCH"));
}