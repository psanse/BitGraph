/**
 * @file random.h
 * @brief Public random-number utilities for the UTILS library.
 *
 * This header provides lightweight random operations for consumer code while
 * keeping the generator implementation in the internal detail namespace.
 *
 * Random generators maintain shared state for each distribution and engine
 * specialization. Consequently, calls affect a common random sequence and are
 * not thread-safe unless access is externally synchronized.
 *
 * @note The default real-valued generator used by uniform_dist() must produce
 *       values in the half-open interval [0, 1).
 *
 * @author Pablo San Segundo
 * @date Last updated: 22/09/2026
 */

#ifndef BITGRAPH_UTILS_RANDOM_H
#define BITGRAPH_UTILS_RANDOM_H

#include "utils/detail/random_uniform_gen.h"
#include <cassert>

namespace bitgraph {

    namespace utils {
		      
			/**
			 * @brief Performs a Bernoulli trial with success probability @p p.
			 *
			 * Generates a uniformly distributed value in [0, 1) and returns whether it
			 * is less than @p p. The implementation reuses the shared real-valued
			 * generator and does not construct a distribution on each call.
			 *
			 * @param p Probability of success, in the closed interval [0, 1].
			 * @return `true` with probability @p p; otherwise, `false`.
			 *
			 * @pre @p p must satisfy `0.0 <= p && p <= 1.0`.
			 *
			 * @note This function is intended for hot paths.
			 * @note The shared generator is not thread-safe.
			 */
			inline
				bool uniform_dist(double p)
			{
				assert(p >= 0.0 && p <= 1.0); // Rejects NaN too.						

				// The real-valued generator produces values in [0, 1).
				return detail::g_rugen() < p;

				// comments

				//B) if distribution range is not guaranteed to be in [0, 1)
				//const double sample = std::generate_canonical<
				//	double,
				//	std::numeric_limits<double>::digits		// typically 53 for double
				//>(g_rugen.engine());
				//return sample < p;

				//C) generates a distribution each time - not for hot paths
				//std::bernoulli_distribution trial(p);
				//return trial(g_rugen.engine());					

				//D) windows generator (deprecated)
			   /* double n_01=std::rand()/(double)RAND_MAX;
				return (n_01<=p);*/

			}

	} // namespace utils

} // namespace bitgraph

#endif // BITGRAPH_UTILS_RANDOM_H