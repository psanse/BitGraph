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
#include "gtest/gtest.h"
#include "utils/file.h"

using namespace std;

//no real test, just to show how to use the macro
TEST(FileTest, example_macro){
	const char* FILENAME = "log.txt";   

    /////////////////////////////////////////////////////////////////
	FILE_LOG(FILENAME, WRITE)	<< "hello world"		<< endl;
	FILE_LOG(FILENAME, APPEND)	<< "appends hello world"<< endl;
    /////////////////////////////////////////////////////////////////

    std::remove(FILENAME);
}

// Helper function to check if a file exists
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Test case for writing to a file
TEST(FileTest, WriteToFile) {

    const char* filename = "test_write.txt";

    {
        File file(filename, File::WRITE);
        std::fstream& fs = file.getFileStream();
        fs << "Hello, world!";
    }

	//reads the file and ckecks the content
    std::ifstream file(filename);
    std::string content;
    std::getline(file, content);

    ////////////////////////////////////////
    EXPECT_EQ(content, "Hello, world!");
    ///////////////////////////////////////
    
    file.close();
    std::remove(filename);
}

// Test case for reading from a file
TEST(FileTest, ReadFromFile) {

    //write to a file
    const char* filename = "test_read.txt";
    {
        std::ofstream file(filename);
        file << "Hello, world!";
        file.close();
    }

    //reads the file and checks the content
    {
        File file(filename, File::READ);
        std::fstream& fs = file.getFileStream();
        std::string content;
        std::getline(fs, content);

        ////////////////////////////////////////
        EXPECT_EQ(content, "Hello, world!");
        ////////////////////////////////////////
    }

    std::remove(filename);
}

// Test case for appending to a file
TEST(FileTest, AppendToFile) {

    const char* filename = "test_append.txt";
    {
        std::ofstream file(filename);
        file << "Hello";
        file.close();
    }

    //appends to the file
    {
        File file(filename, File::APPEND);
        std::fstream& fs = file.getFileStream();
        fs << ", world!";
    }

	//checks the content of the file
    std::ifstream file(filename);
    std::string content;
    std::getline(file, content);

	////////////////////////////////////////
    EXPECT_EQ(content, "Hello, world!");
	////////////////////////////////////////
    
    file.close();
    std::remove(filename);
}

// Test case for read/write mode
TEST(FileTest, ReadWriteFile) {

    const char* filename = "test_read_write.txt";
    {
        std::ofstream file(filename);
        file << "Hello";
        file.close();
    }

    {
        File file(filename, File::READ_WRITE);
        std::fstream& fs = file.getFileStream();
        fs.seekp(0, std::ios::end);
        fs << ", world!";
    }


    //checks the content of the file
    std::ifstream file(filename);
    std::string content;
    std::getline(file, content);

    ////////////////////////////////////////
    EXPECT_EQ(content, "Hello, world!");
    ////////////////////////////////////////

    file.close();
    std::remove(filename);
}

// Test case for file not found
TEST(FileTest, FileNotFound) {
    const char* filename = "non_existent_file.txt";
    
    File file(filename, File::READ);
    std::fstream& fs = file.getFileStream();   
    EXPECT_TRUE(fs.fail());
}
