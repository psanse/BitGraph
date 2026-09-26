#include "algorithm_info.h"
#include <iomanip>


using namespace bitgraph;	

void AlgorithmParameters::reset_base()
{
    name.clear();
    N = 0;
    M = 0;

    time_out = std::numeric_limits<double>::max();
    heuristic_time_out = std::numeric_limits<double>::max();

    num_threads = 1;
    unrolled = false;

	clear_parsing_timer();
}

//I/O		
std::ostream& AlgorithmParameters::print(
	std::ostream& out,
	bool trailing_newline) const
{
	out << "NAME: " << name
		<< "\nN: " << N
		<< "\nM: " << M
		<< "\nTOUT(s): " << time_out
		<< "\nTOUT_HEUR(s): " << heuristic_time_out
		<< "\nnTHREADS: " << num_threads
		<< "\nTIME_PARSE(s): " << parsing_time;
	//  << "\nunrolled: " << std::boolalpha << unrolled << std::noboolalpha;

	if (trailing_newline) {
		out << '\n';
	}
	return out;
}

