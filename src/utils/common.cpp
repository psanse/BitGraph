/**
 * @file common.cpp
 * @brief implementation of common utilities in common.h
 * @date 31/10/2024 (before the implementation was in the header file)
 * @last_update 17/01/25
 * @author pss
 **/

#include "common.h"

using namespace std;

//One and only global generators
namespace bitgraph {
	namespace com {
		namespace _rand {   
			iugen g_iugen(iugen::UseFixedSeed{});
			rugen g_rugen(rugen::UseFixedSeed{});
		}
	}
}


//gloabl template variables
namespace bitgraph {

	/*namespace com {
		namespace _rand {
			template<typename D, typename RE>
			std::size_t RandomUniformGen<D, RE>::seed_ = RandomUniformGen<D, RE>::FIXED_RANDOM_SEED;
		}
	}*/

	
	namespace com {
		namespace _sort {

			/********************************************************************************************************************************
			*
			*		 Sorting of n items (integer) according to non-increasing values of their scores (double)
			*
			********************************************************************************************************************************/
			void SORT_NON_INCR(int* item, double* score, int n)
			{
				int salto, i, j, tempItem;
				double tempScore;

				for (salto = n / 2; salto > 0; salto /= 2)
					for (i = salto; i < n; i++)
						for (j = i - salto; j >= 0; j -= salto) {
							if (score[j] >= score[j + salto]) break;
							tempScore = score[j]; score[j] = score[j + salto]; score[j + salto] = tempScore;
							tempItem = item[j]; item[j] = item[j + salto]; item[j + salto] = tempItem;
						}
			}

			/********************************************************************************************************************************
			*
			*  Sorting of n items (integer) according to non-increasing values of their scores (double)
			*
			********************************************************************************************************************************/
			void SORT_NON_DECR(int* item, double* score, int n)

			{
				int salto, i, j, tempItem;
				double tempScore;

				for (salto = n / 2; salto > 0; salto /= 2)
					for (i = salto; i < n; i++)
						for (j = i - salto; j >= 0; j -= salto) {
							if (score[j] <= score[j + salto]) break;
							tempScore = score[j]; score[j] = score[j + salto]; score[j + salto] = tempScore;
							tempItem = item[j]; item[j] = item[j + salto]; item[j + salto] = tempItem;
						}
			}


		}
	}


	namespace com {
		namespace _dir {

			void append_slash(std::string& path) {

				std::size_t pos;

				pos = path.find_last_of("\\/");
				if (pos == path.length() - 1) {
#ifdef  __GNUC__
					if (path[pos] == '\\')
						path.replace(pos, path.length(), "/");
#elif	_MSC_VER
					//Windows accepts both slashes so no changes are required
					/*  if(path[pos]=='/')
						  path.replace(pos,path.length(),"\\");*/
#endif
				}
				else { //no slash at the ends
#ifdef _MSC_VER
					//default option for windows
					path += '\\';

#elif  __GNUC__
					path += '/';
#endif
				}
			}

			string remove_path(const std::string& filename) {

				size_t pos = filename.find_last_of("\\/");

				if (pos == string::npos) {
					return filename;
				}
				else { return filename.substr(pos + 1); }
			}
		}
	}


	namespace com {
		namespace _file {

			int READ_SET_OF_INTERDICTED_VERTICES(const char* filename, vector<int>& interdicted_nodes) noexcept {

				//opens a file in binary mode
				std::ifstream f(filename, ios::binary | ios::in);
				if (!f) {
					LOGG_ERROR("File could not be opened: ", filename, "_file::READ_SET_OF_INTERDICTED_NODES");
					return -1;
				}

				//reads the file and stores the positions of the 0s
				interdicted_nodes.clear();
				std::size_t index = 0;
				char c;
				while (f.get(c)) {

					if (c == '0') {
						/////////////////////////////////////
						interdicted_nodes.push_back(index);
						////////////////////////////////////
					}

					index++;
				}

				f.close();
				return 0;
			}
		}
	}

	namespace com {
		namespace _time {
			//////////////////////////////////////
			//
			// makeTimePoint(...)
			// 
			// converts calendar time to timepoint of system clock
			//
			/////////////////////////////////////


			std::chrono::system_clock::time_point
				makeTimePoint(int year, int mon, int day,
					int hour, int min, int sec)
			{
				struct std::tm t;
				t.tm_sec = sec;						// second of minute (0 .. 59 and 60 for leap seconds)
				t.tm_min = min;						// minute of hour (0 .. 59)
				t.tm_hour = hour;					// hour of day (0 .. 23)
				t.tm_mday = day;					// day of month (0 .. 31)
				t.tm_mon = mon - 1;					// month of year (0 .. 11)
				t.tm_year = year - 1900;			// year since 1900
				t.tm_isdst = -1;					// determine whether daylight saving time

				std::time_t tt = std::mktime(&t);
				if (tt == -1) {
					throw "no valid system time";
				}
				return std::chrono::system_clock::from_time_t(tt);
			}
		}
	}

} //end namespace bitgraph