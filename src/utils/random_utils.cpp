/**
 * @file random_utils.cpp
 * @brief Defines the shared random generators used by the UTILS library.
 *
 * This translation unit provides the single program-wide definitions of the
 * random generator objects declared in random_uniform_gen.h.
 *
 * The generators are default-constructed so that they preserve the initial
 * fixed seed established by their corresponding RandomUniformGen
 * specializations.
 *
 * @note The shared generators are not thread-safe. Concurrent access requires
 *       external synchronization.
 *
 * @author Pablo San Segundo
 * @date Last updated: 22/09/2026
 */

#include "random_utils.h"

namespace bitgraph {

	namespace utils {

		namespace detail {

			rugen g_rugen;
			iugen g_iugen;

		} // namespace detail

	} // namespace utils

} // namespace bitgraph