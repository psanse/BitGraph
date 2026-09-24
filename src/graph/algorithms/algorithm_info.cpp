#include "algorithm_info.h"


using namespace bitgraph;	

void AlgorithmParameters::reset()
{
    name.clear();

    N = 0;
    M = 0;

    time_out = std::numeric_limits<double>::max();
    heuristic_time_out = std::numeric_limits<double>::max();

    num_threads = 1;
    unrolled = false;

    start_time = time_point_type{};
    elapsed_time = 0.0;
}

//I/O		
std::ostream& AlgorithmParameters::print(
	std::ostream& o,
	bool trailing_newline) const
{
	o << "NAME: " << name
		<< "\nN: " << N
		<< "\nM: " << M
		<< "\nTOUT(s): " << time_out
		<< "\nTOUT_HEUR(s): " << heuristic_time_out
		<< "\nnTHREADS: " << num_threads
		<< "\nTIME_PARSE(s): " << elapsed_time;
	//  << "\nunrolled: " << std::boolalpha << unrolled << std::noboolalpha;

	if (trailing_newline) {
		o << '\n';
	}
	return o;
}