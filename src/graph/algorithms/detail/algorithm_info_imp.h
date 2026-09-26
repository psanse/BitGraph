/**
 * @file algorithm_info_impl.h
 * @brief Template implementations for BasicAlgorithmInfo.
 *
 * Contains definitions of the BasicAlgorithmInfo member functions that must
 * remain visible to every translation unit instantiating the class template.
 *
 * This file is included at the end of algorithm_info.h and should not normally
 * be included directly by consumer code.
 *
 * @note All definitions in this file belong to the bitgraph namespace.
 *
 * @author Pablo San Segundo
 * @date Last updated: 26/09/2026
 */

#ifndef BITGRAPH_GRAPH_ALGORITHM_INFO_IMP_H
#define BITGRAPH_GRAPH_ALGORITHM_INFO_IMP_H

using namespace bitgraph;	


template <class ParametersT>
void BasicAlgorithmInfo<ParametersT>::clear(bool preserve_base_parameters) {
	
	// Clear timers belonging to algorithm execution.
	clear_execution_timers();

	// Polymorphic hook for algorithm-specific execution results.
	clear_results();
	
	// Always clear algorithm-specific parameters/results.
	parameters_.reset_derived();
	
	if (!preserve_base_parameters) {
		// This also clears the parsing timer.
		parameters_.reset_base();
	}
}


template<class ParametersT>
std::ostream& BasicAlgorithmInfo<ParametersT>::print_report(
	std::ostream& out,
	report_t format,
	bool trailing_newline) const
{
	const std::ios::fmtflags previous_flags = out.flags();
	const std::streamsize previous_precision = out.precision();

	out << std::fixed << std::setprecision(4);

	switch (format) {
	case report_t::TABLE:
		out << parameters_.name << '\t'
			<< parameters_.N << '\t'
			<< parameters_.M << '\t'
			<< parameters_.time_out << '\t'
			<< parameters_.heuristic_time_out << '\t'
			<< parameters_.parsing_time << '\t'
			<< preproc_time_ << '\t'
			<< incumbent_time_ << '\t'
			<< search_time_ << '\t';
		break;

	case report_t::VERBOSE:
		out << "*****************************"
			<< "\nNAME: " << parameters_.name
			<< "\nVERTICES: " << parameters_.N
			<< "\nEDGES: " << parameters_.M
			<< "\nTIME_OUT(s): " << parameters_.time_out
			<< "\nTIME_OUT_HEUR(s): " << parameters_.heuristic_time_out
			<< "\nTIME_PARSE(s): " << parameters_.parsing_time
			<< "\nTIME_PREPROC(s): " << preproc_time_
			<< "\nTIME_INCUMBENT(s): " << incumbent_time_
			<< "\nTIME_SEARCH(s): " << search_time_;
		break;

	default:
		LOGG_ERROR(
			"Invalid report format in "
			"BasicAlgorithmInfo::printReport: ",
			static_cast<int>(format));

		std::terminate();
	}

	if (trailing_newline) {
		out << '\n';
	}

	out.flags(previous_flags);
	out.precision(previous_precision);

	return out;
}


template<class ParametersT>
std::ostream& BasicAlgorithmInfo<ParametersT>::print_params(
	std::ostream& out) const
{
	const std::ios::fmtflags previous_flags = out.flags();
	const std::streamsize previous_precision = out.precision();

	out << std::fixed << std::setprecision(4);

	double density = 0.0;

	if (parameters_.N >= 2) {
		const double max_arcs =
			static_cast<double>(parameters_.N) *
			static_cast<double>(parameters_.N - 1);

		density =
			2.0 * static_cast<double>(parameters_.M) / max_arcs;
	}

	out << "\n*****************************"
		<< "\nNAME: " << parameters_.name
		<< "\tVERTICES: " << parameters_.N
		<< "\tEDGES: " << parameters_.M
		<< "\tDENSITY: " << density
		<< "\nTIME_LIMIT: " << parameters_.time_out
		<< "\nTIME_LIMIT_HEUR: " << parameters_.heuristic_time_out
		<< "\nNUM_THREADS: " << parameters_.num_threads
		<< "\n*****************************\n";

	out.flags(previous_flags);
	out.precision(previous_precision);

	return out;
}

template<class ParametersT>
std::ostream& BasicAlgorithmInfo<ParametersT>::print_timers(
	std::ostream& out) const
{
	const std::ios::fmtflags previous_flags = out.flags();
	const std::streamsize previous_precision = out.precision();

	out << std::fixed << std::setprecision(4);

	out << "*****************************"
		<< "\nTIME_PARSE(s): " << parameters_.parsing_time
		<< "\nTIME_PREPROC(s): " << preproc_time_
		<< "\nTIME_INCUMBENT(s): " << incumbent_time_
		<< "\nTIME_SEARCH(s): " << search_time_
		<< "\nTIME_OUT(s): " << parameters_.time_out
		<< "\nTIME_OUT_HEUR(s): " << parameters_.heuristic_time_out;

	out.flags(previous_flags);
	out.precision(previous_precision);

	return out;
}

template<class ParametersT>
void BasicAlgorithmInfo<ParametersT>::start_timer(
	phase_t phase) noexcept
{
	switch (phase)
	{
	case phase_t::SEARCH:
		search_start_time_ = clock_type::now();
		break;

	case phase_t::PREPROC:
		preproc_start_time_ = clock_type::now();
		break;

	case phase_t::LAST_INCUMBENT:
		incumbent_start_time_ = clock_type::now();
		break;

	case phase_t::PARSE:
		parameters_.start_parsing_timer();
		break;

	default:
		LOGG_ERROR(
			"Invalid timer phase in "
			"BasicAlgorithmInfo::start_timer: ",
			static_cast<int>(phase));

		std::terminate();
	}
}


template<class ParametersT>
double BasicAlgorithmInfo<ParametersT>::read_timer(
	phase_t phase) noexcept
{
	const time_point_type end_time = clock_type::now();

	switch (phase)
	{
	case phase_t::SEARCH:
		search_time_ =
			utils::to_seconds(end_time - search_start_time_);

		return search_time_;

	case phase_t::PREPROC:
		preproc_time_ =
			utils::to_seconds(end_time - preproc_start_time_);

		return preproc_time_;

	case phase_t::LAST_INCUMBENT:
		incumbent_time_ =
			utils::to_seconds(end_time - incumbent_start_time_);

		return incumbent_time_;

	case phase_t::PARSE:
		parameters_.parsing_time =
			utils::to_seconds(end_time - parameters_.parsing_start_time);

		return parameters_.parsing_time;

	default:
		LOGG_ERROR(
			"Invalid timer phase in "
			"BasicAlgorithmInfo::readTimer: ",
			static_cast<int>(phase));

		std::terminate();
	}
}

template<class ParametersT>
void BasicAlgorithmInfo<ParametersT>::clear_timer(
	phase_t phase) noexcept
{
	switch (phase)
	{
	case phase_t::SEARCH:
		search_time_ = 0.0;
		break;

	case phase_t::PREPROC:
		preproc_time_ = 0.0;
		break;

	case phase_t::LAST_INCUMBENT:
		incumbent_time_ = 0.0;
		break;

	case phase_t::PARSE:
		parameters_.clear_parsing_timer();
		break;

	default:
		LOGG_ERROR(
			"Invalid timer phase in "
			"BasicAlgorithmInfo::clearTimer: ",
			static_cast<int>(phase));

		std::terminate();
	}
}


template <class ParametersT>
void BasicAlgorithmInfo<ParametersT>::clear_execution_timers()  noexcept
{	
	clear_timer(phase_t::PREPROC);
	clear_timer(phase_t::LAST_INCUMBENT);
	clear_timer(phase_t::SEARCH);
}


#endif // BITGRAPH_GRAPH_ALGORITHM_INFO_IMP_H