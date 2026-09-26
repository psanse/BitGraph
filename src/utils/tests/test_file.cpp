/**
* @file test_file.cpp
* @brief Tests for class File with a macro FILE_LOG to stream to a file conveniently
* @date ?
* @last_update 17/01/25
* @author done by copilot
*
**/

#include <iostream>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "utils/file_utils.h"

using namespace std;
using namespace bitgraph;


namespace {
    
    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
}

/**
 * @test Verifies that FILE_LOG supports writing and appending to a file.
 */
TEST(FileTest, file_log_macro_writes_and_appends)
{
    const char* filename = "file_log_macro_test.txt";

    // Remove any file left by an earlier interrupted test.
    std::remove(filename);

    FILE_LOG(filename, WRITE)
        << "hello world\n";

    FILE_LOG(filename, APPEND)
        << "appends hello world\n";

    std::string contents;

    {
        std::ifstream input(filename);
        ASSERT_TRUE(input.is_open());

        std::ostringstream buffer;
        buffer << input.rdbuf();
        contents = buffer.str();
    } // Close the input stream before removing the file on Windows.

    EXPECT_EQ(
        "hello world\n"
        "appends hello world\n",
        contents);

    EXPECT_EQ(0, std::remove(filename));
}


// Test case for writing to a file
TEST(FileTest, WriteToFile)
{
    const char* filename = "test_write.txt";
    std::remove(filename);

    {
        File file(filename, File::Mode::WRITE);
        std::fstream& stream = file.stream();

        stream << "Hello, world!";
        ASSERT_TRUE(stream.good());
    }

    std::string contents;

    {
        std::ifstream input(filename);
        ASSERT_TRUE(input.is_open());

        std::getline(input, contents);
    }

    EXPECT_EQ("Hello, world!", contents);
    EXPECT_EQ(0, std::remove(filename));
}

// Test case for reading from a file
TEST(FileTest, ReadFromFile)
{
    const char* filename = "test_read.txt";
    std::remove(filename);

    {
        std::ofstream output(filename);
        ASSERT_TRUE(output.is_open());

        output << "Hello, world!";
        ASSERT_TRUE(output.good());
    }

    std::string contents;

    {
        File file(filename, File::Mode::READ);
        std::fstream& stream = file.stream();

        std::getline(stream, contents);
    }

    EXPECT_EQ("Hello, world!", contents);
    EXPECT_EQ(0, std::remove(filename));
}

// Test case for appending to a file
TEST(FileTest, AppendToFile)
{
    const char* filename = "test_append.txt";
    std::remove(filename);

    {
        std::ofstream output(filename);
        ASSERT_TRUE(output.is_open());

        output << "Hello";
        ASSERT_TRUE(output.good());
    }

    {
        File file(filename, File::Mode::APPEND);
        std::fstream& stream = file.stream();

        stream << ", world!";
        ASSERT_TRUE(stream.good());
    }

    std::string contents;

    {
        std::ifstream input(filename);
        ASSERT_TRUE(input.is_open());

        std::getline(input, contents);
    }

    EXPECT_EQ("Hello, world!", contents);
    EXPECT_EQ(0, std::remove(filename));
}

// Test case for read/write mode
TEST(FileTest, ReadWriteFile)
{
    const char* filename = "test_read_write.txt";
    std::remove(filename);

    {
        std::ofstream output(filename);
        ASSERT_TRUE(output.is_open());

        output << "Hello";
        ASSERT_TRUE(output.good());
    }

    {
        File file(filename, File::Mode::READ_WRITE);
        std::fstream& stream = file.stream();

        stream.seekp(0, std::ios::end);
        ASSERT_TRUE(stream.good());

        stream << ", world!";
        ASSERT_TRUE(stream.good());
    }

    std::string contents;

    {
        std::ifstream input(filename);
        ASSERT_TRUE(input.is_open());

        std::getline(input, contents);
    }

    EXPECT_EQ("Hello, world!", contents);
    EXPECT_EQ(0, std::remove(filename));
}

// Test case for a missing input file
TEST(FileTest, FileNotFound)
{
    const char* filename = "non_existent_file.txt";
    std::remove(filename);

    EXPECT_THROW(
        File file(filename, File::Mode::READ),
        std::runtime_error);
}