/**
* @file file.h
* @brief A basic wrapper to read/write from a filestream safely
*		 Uses macro FILE_LOG for easy logging to file
* @date: created 24/02/15, last_update 01/12/25
* @details: will throw if the file cannot be opened
* @dev pss
**/

#ifndef __FILE_LOG_H__
#define __FILE_LOG_H__

#include <iostream>
#include <fstream>
#include "utils/logger.h"

#define FILE_LOG(file,type)		bitgraph::File( (file), bitgraph::File::type).stream()	


namespace bitgraph {

	namespace _impl {
		
		class File {
		public:
			enum mode_t { READ = 0, WRITE, READ_WRITE, APPEND };

			//getter
			std::fstream& stream() { return fs; }

			//contructor
			explicit File(const char* filename, mode_t mode = APPEND)
			{
				switch (mode) {
				case READ:
					fs.open(filename, std::ios::in);					
					break;
				case WRITE:
					fs.open(filename, std::ios::out);					
					break;
				case READ_WRITE:
					fs.open(filename, std::ios::in | std::ios::out);

					//creates a new file if it does not exist
					if (!fs.is_open()) { 
						fs.clear();
						fs.open(filename, std::ios::out); 
						fs.close();
						fs.open(filename, std::ios::in | std::ios::out);
					}					
					break;
				case APPEND:																//Considered only for writing purposes
					fs.open(filename, std::ios::out | std::ios::app);
					break;

				default:
					LOG_ERROR("unknown mode for operating on a file -File::File ");
				}

				//check if file is open
				if (!fs.is_open()) {
					LOGG_ERROR("unable to open file: ", filename, " -File::File");
					throw("unable to open file");
				}
			}

			//copy semantics forbidden
			File(const File&) = delete;
			File& operator=(const File&) = delete;

			//move semantics allowed 
			File(File&& other) noexcept : fs(std::move(other.fs)) {}
			File& operator=(File&& other) noexcept {
				if (this != &other) fs = std::move(other.fs);
				return *this;
			}

			//destructor
			~File() { fs.close(); }

			//useful methods
			bool is_open() const { return fs.is_open(); }
			bool good() const { return fs.good(); }


		private:
			std::fstream fs;
		};
	}//end namespace _impl	

	using _impl::File;	
	

}//end namespace bitgraph

#endif
