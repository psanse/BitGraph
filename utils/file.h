//file.h: a simple wrapper to read/write from a filestream safely 
//example: FILE_LOG("foo.txt",WRITE)<<"hello world"
//last update: 24/02/15
///////////////////////////////////

#ifndef __FILE_H__
#define __FILE_H__

#include <iostream>
#include <fstream>

using namespace std;

#define FILE_LOG(file,type)		File( (file), File::type).getFileStream()		

class File{
public:
	 enum mode_t {READ=0, WRITE, READ_WRITE, APPEND} ;	 

	 fstream& getFileStream(){
		return fs;
	}

	 File(const char* filename, mode_t mode=APPEND){
		 switch(mode){
		 case READ:
			 this->type=mode;
			 fs.open(filename,fstream::in);
			 if(fs.fail()){
				 cerr<<"unable to open file: "<<filename<<endl;
			 }
			 break;
		 case WRITE:
			 this->type=mode;
			 fs.open(filename,fstream::out);
			 if(fs.fail()){
				 cerr<<"unable to open file: "<<filename<<endl;
			 }
			 break;
		 case READ_WRITE:
			 this->type=mode;
			 fs.open(filename,fstream::in|fstream::out);
			 if(fs.fail()){
				 cerr<<"unable to open file: "<<filename<<endl;
			 }
			 break;
		 case APPEND:	//Considered only for writing purposes
			 this->type=mode;
			 fs.open(filename,fstream::out | fstream::app);
			 if(fs.fail()){
				 cerr<<"unable to open file: "<<filename<<endl;
			 }
			 break;

		 default:
			 cerr<<"incorret mode for FILE_LOG"<<endl;
		 }
	}

	~File(void){
		if(fs.is_open())
				fs.close();
	}

private:
	mode_t type;
	fstream fs;
};

#endif
