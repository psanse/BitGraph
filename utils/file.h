//file.h: a simple wrapper to read/write from a filestream safely 
//example: FILE_LOG("foo.txt",WRITE)<<"hello world"
//last update: 24/02/15
///////////////////////////////////

#ifndef __FILE_H__
#define __FILE_H__

#include <iostream>
#include <fstream>
#include "logger.h"

#define FILE_LOG(file,type)		File( (file), File::type).getFileStream()		

class File{
public:
	 enum mode_t {READ=0, WRITE, READ_WRITE, APPEND} ;	 

	 //getter
	 std::fstream& getFileStream(){	return fs; }

	 //contructor
	 File(const char* filename, mode_t mode = APPEND)  
	 {
		 switch(mode){
		 case READ:			
			 fs.open(filename, std::fstream::in);
			 if(fs.fail()){
				 LOG_ERROR("unable to open reading file: " , filename, "-File::File ");
			 }
			 break;
		 case WRITE:			
			 fs.open(filename, std::fstream::out);
			 if(fs.fail()){
				 LOG_ERROR("unable to open writing file: ", filename, "-File::File ");				
			 }
			 break;
		 case READ_WRITE:			
			 fs.open(filename, std::fstream::in | std::fstream::out);
			 if(fs.fail()){
				 LOG_ERROR("unable to open reading/writing file: ", filename, "-File::File ");
			 }
			 break;
		 case APPEND:																//Considered only for writing purposes
			 fs.open(filename, std::fstream::out | std::fstream::app);
			 if(fs.fail()){
				 LOG_ERROR("unable to open appending file: ", filename, "-File::File ");
			 }
			 break;

		 default:
			 LOG_ERROR("unknonw mode for operating on a file -File::File ");			
		 }
	}

	 //destructor
	 ~File() { fs.close(); }
	

private:
	std::fstream fs;
};

#endif
