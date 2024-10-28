//dimacs_reader.h: header of stateless class DIMACS_READER which implements a number of utilities to read dimacs format
//
//date: 30/09/2018
//author: pss

#include <fstream>
#include <sstream>
#include <iostream>

#include "utils/file.h"
#include "utils/logger.h"
#include "utils/common.h"

using namespace std;

#ifndef	__DIMACS_READER_H_
#define __DIMACS_READER_H_

class DIMACS_READER{
public:
	static int read_dimacs_header(fstream& f, int& n, int& m);
	static int read_empty_lines(fstream& f);
	//...
};


inline 
int DIMACS_READER::read_empty_lines(fstream& f){
//////////////
// TODO@experimental (now also in common.h)

	char line[250], c;
	do{
		c=f.peek();
		if(c=='\n'){
			f.getline(line, 250);
			continue;		
		}
		break;
	}while(true);

	return 0;
}

inline
int DIMACS_READER::read_dimacs_header(fstream& f, int& n, int& m){
///////////////////
// header p edges line
//
// RETURNS n or -1 if ERROR

	char token[10], line[250], c;
	bool loop_finding_header=true;
	do{
		c=f.peek();
		if(c=='\n' || c=='c' || c=='\r'){
			f.getline(line, 250);
			continue;		
		}
		switch(c){
		case 'p':  //header p edge <nV> <nE>
			f>>token>>token>>n>>m;
			if(!f.good() || strstr(token, "edge")==0){
				LOG_ERROR("read_RTN_weights(string filename)-bad header 'p' line");
				//f.close();
				return -1;
			}
			loop_finding_header=false;
			f.getline(line, 250);			//remove remaining part of the line
			break;
		case 'c':
			break;						//comment			
		default:									
			//f.close();
			LOG_ERROR("read_RTN_weights(string filename)-wrong DIMACS protocol");
			LOG_ERROR("first character of new line is: "<<c);				
			return -1;
		}
	}while(loop_finding_header);

	return n;
}

#endif