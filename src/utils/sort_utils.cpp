/**
 * @file sort_utils.cpp
 * @brief Implements non-template sorting utilities for parallel arrays.
 *
 * This translation unit provides low-level sorting operations that reorder
 * item and score arrays while preserving the association between each item
 * and its corresponding score.
 *
 * Template comparison functors remain implemented in sort_utils.h.
 *
 * @author Pablo San Segundo, Fabio Furini
 * @date Last updated: 23/09/2026
 */

#include "sort_utils.h"

namespace bitgraph {

    namespace utils {
		    

		/********************************************************************************************************************************
		*
		*		 Sorting of n items (integer) according to non-increasing values of their scores (double)
		*
		********************************************************************************************************************************/
		void sort_non_increasing(int* item, double* score, int n)
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
		void sort_non_decreasing(int* item, double* score, int n)

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


		
	} // namespace utils

} // namespace bitgraph

