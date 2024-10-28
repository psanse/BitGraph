#include <iostream>
#include <sstream>
#include "gtest/gtest.h"
#include "../file.h"

using namespace std;

#define FILENAME "log.txt"

TEST(FILE, example){
	FILE_LOG(FILENAME, WRITE)<<"hello world"<<endl;
	FILE_LOG(FILENAME, APPEND)<<"appends hello world"<<endl;
}