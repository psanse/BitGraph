#include "algorithm_info.h"
#include "utils/logger.h"


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


template <class ParametersT>
void BasicAlgorithmInfo<ParametersT>::clear(bool lazy) {
	clearAllTimers();
	if (!lazy) {
		clearGeneralInfo();					
	}
}

template <class ParametersT>
std::ostream& BasicAlgorithmInfo<ParametersT>::printReport(
	std::ostream& o,
	report_t r, 
	bool trailing_newline) const
{
	o.setf(std::ios::fixed); // fixed notation for floating point values	
	o.precision(4);				// 4 decimal places


	if (r == report_t::TABLE) {
		o << data_.name << "\t" << data_.N << "\t" << data_.M << "\t"
			<< data_.time_out << "\t"
			<< data_.heuristic_time_out << "\t"
			<< data_.elapsed_time << "\t"
			<< timePreproc_ << "\t"
			<< timeIncumbent_ << "\t"
			<< timeSearch_ << "\t";				//to concatenate with other info if needed
	}
	if (r == report_t::VERBOSE) {
		o << "\n*****************************";
		o << "\nNAME:" << data_.name
			<< "\nN:" << data_.N
			<< "\nM:" << data_.M
			<< "\nTIME_OUT(s):" << data_.time_out
			<< "\nTIME_OUT_HEUR(s):" << data_.heuristic_time_out
			<< "\nTIME_PARSE(s):" << data_.elapsed_time
			<< "\nTIME_PREPROC(s):" << timePreproc_
			<< "\nTIME_INCUMBENT(s):" << timeIncumbent_
			<< "\nTIME_SEARCH(s)" << timeSearch_;
	}

	if (trailing_newline) {
		o << '\n'; 
	}

	return o;
}

template <class ParametersT>
std::ostream& BasicAlgorithmInfo<ParametersT>::printParams(std::ostream& o) const
{
	o.setf(std::ios::fixed);	// fixed notation for floating point values	
	o.precision(4);				// 4 decimal places

	o << "\n*****************************";
	o << "\nNAME:" << data_.name << "\t N:" << data_.N << "\t M:" << data_.M << "\t D:" << 2 * data_.M / (float)((data_.N - 1) * data_.N) << endl;
	o << "\nTIME_LIMIT:" << data_.time_out;
	o << "\nTIME_LIMIT_HEUR:" << data_.heuristic_time_out;
	o << "*****************************" << '\n';

	return o;
}

template <class ParametersT>
std::ostream& BasicAlgorithmInfo<ParametersT>::printTimers(std::ostream& o) const
{
	o.setf(std::ios::fixed);	// fixed notation for floating point values	
	o.precision(4);				// 4 decimal places

	o << "\n*****************************";
	o << "\ntime_parse:" << data_.elapsed_time
		<< "\ntime_preproc:" << timePreproc_
		<< "\ntime_incumbent:" << timeIncumbent_
		<< "\ntime_search:" << timeSearch_
		<< "\nTIME_OUT(s):" << data_.time_out
		<< "\nTIME_OUT_HEUR(s):" << data_.heuristic_time_out;

	return o;
}

template <class ParametersT>
void BasicAlgorithmInfo<ParametersT>::startTimer(phase_t t) noexcept
{
	switch (t) {
	case phase_t::SEARCH:
		startTimeSearch_ = PrecisionTimer::clock_t::now();
		//resetSearchInfo();					//CHECK()?
		break;
	case phase_t::PREPROC:
		startTimePreproc_ = PrecisionTimer::clock_t::now();
		//resetPreprocInfo();					//CHECK()?
		break;
	case phase_t::LAST_INCUMBENT:
		startTimeIncumbent_ = PrecisionTimer::clock_t::now();
		break;
	default:
		LOG_ERROR("bizarre timer type, exiting... - utils::infoBase::start_timer");
		LOGG_ERROR("timer type: ", (int)t, " - utils::infoBase::start_timer");
		std::terminate();
	}
}


template <class ParametersT>
double BasicAlgorithmInfo<ParametersT>::readTimer(phase_t t) const noexcept
{
	double elapsed_time;
	time_point_type endTime = PrecisionTimer::clock_t::now();

	switch (t) {
	case phase_t::SEARCH:
		timeSearch_ = utils::to_seconds(endTime - startTimeSearch_);
		elapsed_time = timeSearch_;
		break;
	case phase_t::PREPROC:
		timePreproc_ = utils::to_seconds(endTime - startTimePreproc_);
		elapsed_time = timePreproc_;
		break;
	case phase_t::LAST_INCUMBENT:
		timeIncumbent_ = utils::to_seconds(endTime - startTimeIncumbent_);
		elapsed_time = timeIncumbent_;
		break;

	default:
		LOG_ERROR("bizarre timer type, exiting... - infoBase::read_time");
		LOGG_ERROR("timer type : ", (int)t);
		std::terminate();
	}

	return elapsedTime;
}

template <class ParametersT>
void BasicAlgorithmInfo<ParametersT>::clearTimer(phase_t t) noexcept
{
	switch (t) {
	case phase_t::SEARCH:
		timeSearch_ = 0.0;
		break;
	case phase_t::PREPROC:
		timePreproc_ = 0.0;
		break;
	case phase_t::LAST_INCUMBENT:
		timeIncumbent_ = 0.0;
		break;
	default:
		LOG_ERROR("bizarre timer type, exiting... - utils::infoBase::clearTimer");
		LOGG_ERROR("timer type: ", (int)t, " - utils::infoBase::clearTimer");
		std::terminate();
	}
}

template <class ParametersT>
void BasicAlgorithmInfo<ParametersT>::clearAllTimers()  noexcept
{
	//clearTimer(phase_t::PARSE);
	clearTimer(phase_t::PREPROC);
	clearTimer(phase_t::LAST_INCUMBENT);
	clearTimer(phase_t::SEARCH);
}

