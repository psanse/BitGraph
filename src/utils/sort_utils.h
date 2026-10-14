/**
 * @file sort_utils.h
 * @brief Comparison functors for sorting values using external criteria.
 *
 * The comparators in this header order elements according to criterion
 * collections indexed by the elements being compared. Additional comparators
 * support derived scores, tie-breaking criteria, and collection sizes.
 *
 * Comparator objects store references to their criterion collections. Those
 * collections must remain valid while the comparators are in use.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_UTILS_SORT_UTILS_H
#define BITGRAPH_UTILS_SORT_UTILS_H


#include <cstddef>
#include <cmath>
#include <cassert>

namespace bitgraph {

    namespace utils {


        /**
         * @brief Compares elements according to values in a criterion collection.
         *
         * @tparam T Element type used to index the criterion collection.
         * @tparam CriterionCollection Criterion collection type.
         * @tparam Greater If `true`, orders by decreasing criterion value; otherwise,
         *                 orders by increasing criterion value.
         * @tparam Enable Reserved for specialization.
         */
        template<
            class T,
            class CriterionCollection,
            bool Greater>
        class has_val {
        public:
            /**
             * @brief Constructs a comparator associated with a criterion collection.
             * @param criterion Criterion collection.
             */
            explicit has_val(const CriterionCollection& criterion)
                : criterion_(criterion)
            {
            }

            /**
             * @brief Compares two elements using their criterion values.
             * @param lhs Left element.
             * @param rhs Right element.
             * @return Whether @p lhs precedes @p rhs.
             */
            bool operator()(const T& lhs, const T& rhs) const
            {
                return Greater
                    ? criterion_[lhs] > criterion_[rhs]
                    : criterion_[lhs] < criterion_[rhs];
            }

        private:
            const CriterionCollection& criterion_;
        };

        /**
         * @brief Pointer specialization of has_val.
         *
         * The pointed-to values are used to index the criterion collection.
         *
         * @tparam T Pointed-to element type.
         * @tparam CriterionCollection Criterion collection type.
         * @tparam Greater Ordering direction.
         */
        template<class T, class CriterionCollection, bool Greater>
        class has_val<T*, CriterionCollection, Greater> {
        public:
            explicit has_val(const CriterionCollection& criterion)
                : criterion_(criterion)
            {
            }

            bool operator()(const T* lhs, const T* rhs) const
            {
                return Greater
                    ? criterion_[*lhs] > criterion_[*rhs]
                    : criterion_[*lhs] < criterion_[*rhs];
            }

        private:
            const CriterionCollection& criterion_;
        };

        /**
         * @brief Compares elements by the product of their value and criterion.
         *
         * The score associated with an element @c x is `criterion[x] * x`.
         *
         * @tparam T Element type.
         * @tparam CriterionCollection Criterion collection type.
         * @tparam Greater Ordering direction.
         */
        template<class T, class CriterionCollection, bool Greater>
        class has_val_prod {
        public:
            explicit has_val_prod(const CriterionCollection& criterion)
                : criterion_(criterion)
            {
            }

            bool operator()(const T& lhs, const T& rhs) const
            {
                const auto lhs_score = criterion_[lhs] * lhs;
                const auto rhs_score = criterion_[rhs] * rhs;

                return Greater
                    ? lhs_score > rhs_score
                    : lhs_score < rhs_score;
            }

        private:
            const CriterionCollection& criterion_;
        };

        /**
         * @brief Compares elements by the difference between criterion and value.
         *
         * The score associated with an element @c x is `criterion[x] - x`.
         *
         * @tparam T Element type.
         * @tparam CriterionCollection Criterion collection type.
         * @tparam Greater Ordering direction.
         */
        template<class T, class CriterionCollection, bool Greater>
        class has_val_diff {
        public:
            explicit has_val_diff(const CriterionCollection& criterion)
                : criterion_(criterion)
            {
            }

            bool operator()(const T& lhs, const T& rhs) const
            {
                const auto lhs_score = criterion_[lhs] - lhs;
                const auto rhs_score = criterion_[rhs] - rhs;

                return Greater
                    ? lhs_score > rhs_score
                    : lhs_score < rhs_score;
            }

        private:
            const CriterionCollection& criterion_;
        };

        // Criterion-value comparator aliases.
        template<class T, class CriterionCollection>
        using has_greater_val =
            has_val<T, CriterionCollection, true>;

        template<class T, class CriterionCollection>
        using has_smaller_val =
            has_val<T, CriterionCollection, false>;

        // Product-score comparator aliases.
        template<class T, class CriterionCollection>
        using has_greater_val_prod =
            has_val_prod<T, CriterionCollection, true>;

        template<class T, class CriterionCollection>
        using has_smaller_val_prod =
            has_val_prod<T, CriterionCollection, false>;

        // Difference-score comparator aliases.
        template<class T, class CriterionCollection>
        using has_greater_val_diff =
            has_val_diff<T, CriterionCollection, true>;

        template<class T, class CriterionCollection>
        using has_smaller_val_diff =
            has_val_diff<T, CriterionCollection, false>;

        /**
         * @brief Compares elements using a primary and a tie-breaking criterion.
         *
         * The secondary criterion is examined only when the primary criterion values
         * compare equal.
         *
         * @tparam T Element type used to index both criterion collections.
         * @tparam CriterionCollection Criterion collection type.
         * @tparam Greater Ordering direction for both criteria.
         */
        template<class T, class CriterionCollection, bool Greater>
        class has_val_with_tb {
        public:
            /**
             * @brief Constructs a two-criterion comparator.
             * @param primary Primary criterion collection.
             * @param tie_break Tie-breaking criterion collection.
             */
            has_val_with_tb(
                const CriterionCollection& primary,
                const CriterionCollection& tie_break)
                : primary_(primary),
                tie_break_(tie_break)
            {
            }

            bool operator()(const T& lhs, const T& rhs) const
            {
                if (Greater
                    ? primary_[lhs] > primary_[rhs]
                    : primary_[lhs] < primary_[rhs]) {
                    return true;
                }

                if (Greater
                    ? primary_[lhs] < primary_[rhs]
                    : primary_[lhs] > primary_[rhs]) {
                    return false;
                }

                return Greater
                    ? tie_break_[lhs] > tie_break_[rhs]
                    : tie_break_[lhs] < tie_break_[rhs];
            }

        private:
            const CriterionCollection& primary_;
            const CriterionCollection& tie_break_;
        };

        template<class T, class CriterionCollection>
        using has_smaller_val_with_tb =
            has_val_with_tb<T, CriterionCollection, false>;

        template<class T, class CriterionCollection>
        using has_greater_val_with_tb =
            has_val_with_tb<T, CriterionCollection, true>;

        /**
         * @brief Orders collections by decreasing size.
         *
         * @tparam Collection Collection type providing size().
         */
        template<class Collection>
        struct has_bigger_size {
            bool operator()(
                const Collection& lhs,
                const Collection& rhs) const
            {
                return lhs.size() > rhs.size();
            }
        };

        /**
         * @brief Orders collections by increasing size.
         *
         * @tparam Collection Collection type providing size().
         */
        template<class Collection>
        struct has_smaller_size {
            bool operator()(
                const Collection& lhs,
                const Collection& rhs) const
            {
                return lhs.size() < rhs.size();
            }
        };


        /**
        * @brief Inserts an item into arrays ordered by non-increasing score.
        *
        * Before the call, the first @p size - 1 positions of @p items and @p scores
        * must contain corresponding entries ordered by non-increasing score. The
        * function inserts @p target and @p target_score while preserving that order.
        *
        * Entries with equal scores retain their relative order, and the new entry is
        * inserted after existing entries with the same score.
        *
        * @tparam Item Item type.
        * @tparam Score Score type supporting comparison with operator>.
        * @param items Array of items with capacity for at least @p size elements.
        * @param scores Parallel array of scores with capacity for at least @p size
        *               elements.
        * @param size Number of entries after insertion; must be at least one.
        * @param target Item to insert.
        * @param target_score Score associated with @p target.
        * @return Zero-based position at which @p target was inserted.
        *
        * @pre @p items and @p scores are not null.
        * @pre @p size is greater than zero.
        * @pre Positions `[0, size - 1)` are ordered by non-increasing score.
        */
        template<class Item, class Score>
        int insert_ordered_non_increasing(
            Item* items,
            Score* scores,
            int size,
            const Item& target,
            const Score& target_score)
        {
            assert(items != nullptr);
            assert(scores != nullptr);
            assert(size > 0);

            int position = size - 1;

            while (position > 0 &&
                target_score > scores[position - 1]) {
                items[position] = items[position - 1];
                scores[position] = scores[position - 1];
                --position;
            }

            items[position] = target;
            scores[position] = target_score;

            return position;
        }

        // old API backwards compatibility
        template<class T>
        int INSERT_ORDERED_SORT_NON_INCR(
            T* items,
            T* scores,
            int size,
            T target,
            T target_score)
        {
            return insert_ordered_non_increasing(
                items,
                scores,
                size,
                target,
                target_score);
        }


        namespace detail {

            /**
              * @brief Sorts items by non-increasing score.
              *
              * Reorders the first @p size elements of the parallel @p items and @p scores
              * arrays so that the scores appear in non-increasing order. Each item remains
              * associated with its original score.
              *
              * @param items Array of integer items.
              * @param scores Parallel array containing the item scores.
              * @param size Number of elements in each array.
              *
              * @pre @p size is non-negative.
              * @pre If @p size is positive, @p items and @p scores point to arrays
              *      containing at least @p size elements.
              *
              * @author Fabio Furini
              * @date 20/07/2017
              */
            void sort_non_increasing(int* item, double* score, int size);


            /**
            * @brief Sorts items by non-decreasing score.
            *
            * Reorders the first @p size elements of the parallel @p items and @p scores
            * arrays so that the scores appear in non-decreasing order. Each item remains
            * associated with its original score.
            *
            * @param items Array of integer items.
            * @param scores Parallel array containing the item scores.
            * @param size Number of elements in each array.
            *
            * @pre @p size is non-negative.
            * @pre If @p size is positive, @p items and @p scores point to arrays
            *      containing at least @p size elements.
            *
            * @author Fabio Furini
            * @date 20/07/2017
            */
            void sort_non_decreasing(int* item, double* score, int size);



		}  // namespace detail

	} // namespace utils

} // namespace bitgraph

#endif // BITGRAPH_UTILS_SORT_UTILS_H	