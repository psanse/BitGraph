/**
 * @file benchmark.cpp
 * @brief Implements the GraphBenchmark class for graph test datasets.
 *
 * This translation unit implements benchmark construction, graph-instance
 * registration, reference-value lookup, path handling, and formatted output.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#include "graph_benchmark.h"
#include "utils/logger.h"
#include <string>
//#include "utils/common.h"
#include "utils/string_utils.h"
#include "utils/path_utils.h"


using namespace bitgraph;

template<class ValueT>
BasicGraphBenchmark<ValueT>::BasicGraphBenchmark(std::string path_name)
	:path_(std::move(path_name))
{
	if (path_.empty()) {
		LOG_DEBUG(
			"GraphBenchmark base path is empty; filenames will be used as supplied. - GraphBenchmark::GraphBenchmark");
	}
}

template<class ValueT>
void BasicGraphBenchmark<ValueT>::add_test(const std::string& filename)
{
	std::string full_filename = path_;

	if (!full_filename.empty()) {
		utils::append_slash(full_filename);
	}

	full_filename += filename;
	filenames_.push_back(std::move(full_filename));
}

template<class ValueT>
void BasicGraphBenchmark<ValueT>::add_test(
	const std::string& filename,
	ValueT value)
{
	std::string full_filename = path_;

	if (!full_filename.empty()) {
		utils::append_slash(full_filename);
	}

	full_filename += filename;

	filenames_.push_back(full_filename);
	values_[full_filename] = value;
}

template<class ValueT>
ValueT BasicGraphBenchmark<ValueT>::get_value(const std::string& filename) const
{
	const auto iterator = values_.find(filename);

	return iterator != values_.end()
		? iterator->second
		: -1;
}

///////////////////
// I/O

template<class ValueT>
std::ostream& BasicGraphBenchmark<ValueT>::print(std::ostream& out) const
{
	for (const auto& filename : filenames_) {
		out << filename;

		const auto iterator = values_.find(filename);

		if (iterator != values_.end()) {
			out << ':' << iterator->second;
		}

		out << '\n';
	}

	return out;
}
