/**
* @file file..h
* @brief A basic wrapper to read/write from a filestream safely
*		 Has a macr FILE_LOG for easy logging
* @date 24/02/15
* @last_update 19/01/25
* @author pss
*
**/

#ifndef __FILE_LOG_H__
#define __FILE_LOG_H__

#include <iostream>
#include <fstream>
#include "utils/logger.h"

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
				 LOGG_ERROR("unable to open reading file: " , filename, "-File::File ");
			 }
			 break;
		 case WRITE:			
			 fs.open(filename, std::fstream::out);
			 if(fs.fail()){
				 LOGG_ERROR("unable to open writing file: ", filename, "-File::File ");				
			 }
			 break;
		 case READ_WRITE:			
			 fs.open(filename, std::fstream::in | std::fstream::out);
			 if(fs.fail()){
				 LOGG_ERROR("unable to open reading/writing file: ", filename, "-File::File ");
			 }
			 break;
		 case APPEND:																//Considered only for writing purposes
			 fs.open(filename, std::fstream::out | std::fstream::app);
			 if(fs.fail()){
				 LOGG_ERROR("unable to open appending file: ", filename, "-File::File ");
			 }
			 break;

		 default:
			 LOG_ERROR("unknown mode for operating on a file -File::File ");			
		 }
	}

	 //destructor
	 ~File() { fs.close(); }
	

private:
	std::fstream fs;
};

#endif
