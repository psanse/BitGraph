/**
 * @file edge_weight_matrix_view.h
 * @brief Non-owning views over a square, row-major (flat) weight matrix.
 *
 * `weight_row_view<T>` is one row and `weight_matrix_view<T>` the whole
 * matrix. Both expose the read interface of `std::vector<std::vector<>>`
 * that edge-weighted graphs have always offered: `m[v][w]`, `size()`,
 * `empty()`, iteration and `==` (also against nested vectors).
 *
 * `T` may be const-qualified; use `const weight_t` for read-only views.
 *
 * @note Internal component used by `Base_Graph_EW`. Views are invalidated by
 *       any operation that reallocates the underlying storage.
 *
 * @author Pablo San Segundo
 */

#ifndef BITGRAPH_GRAPH_DETAIL_EDGE_WEIGHT_MATRIX_VIEW_H
#define BITGRAPH_GRAPH_DETAIL_EDGE_WEIGHT_MATRIX_VIEW_H

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace bitgraph {

	namespace detail {

		/**
		 * @brief Non-owning view of one row of a flat weight matrix.
		 */
		template<class T>
		class weight_row_view {
		public:
			using value_type = typename std::remove_const<T>::type;
			using iterator = T*;
			using const_iterator = const T*;

			weight_row_view() noexcept = default;
			weight_row_view(T* data, std::size_t n) noexcept : data_(data), n_(n) {}

			T& operator[](std::size_t i) const noexcept { return data_[i]; }
			T* data() const noexcept { return data_; }
			std::size_t size() const noexcept { return n_; }
			bool empty() const noexcept { return n_ == 0; }
			T* begin() const noexcept { return data_; }
			T* end() const noexcept { return data_ + n_; }

		private:
			T* data_ = nullptr;
			std::size_t n_ = 0;
		};

		/**
		 * @brief Non-owning view of a square `n x n` row-major weight matrix.
		 */
		template<class T>
		class weight_matrix_view {
		public:
			using value_type = typename std::remove_const<T>::type;
			using row_type = weight_row_view<T>;
			using nested_t = std::vector<std::vector<value_type>>;

			weight_matrix_view() noexcept = default;
			weight_matrix_view(T* data, std::size_t n) noexcept : data_(data), n_(n) {}

			/** @brief Row @p v (no bounds checking). */
			row_type operator[](std::size_t v) const noexcept {
				return row_type(data_ + v * n_, n_);
			}
			row_type row(std::size_t v) const noexcept { return (*this)[v]; }

			/** @brief Number of rows (= number of columns). */
			std::size_t size() const noexcept { return n_; }
			bool empty() const noexcept { return n_ == 0; }
			/** @brief Pointer to the `n*n` contiguous entries. */
			T* data() const noexcept { return data_; }

			/** @brief Copies the view into a nested vector. */
			nested_t to_nested() const {
				nested_t m;
				m.reserve(n_);
				for (std::size_t v = 0; v < n_; ++v) {
					const row_type r = (*this)[v];
					m.emplace_back(r.begin(), r.end());
				}
				return m;
			}

		private:
			T* data_ = nullptr;
			std::size_t n_ = 0;
		};

		/**
		 * @brief Flattens a nested matrix into row-major storage.
		 * @pre @p m is square (every row has `m.size()` entries); not checked here.
		 */
		template<class T>
		std::vector<T> flatten_matrix(const std::vector<std::vector<T>>& m)
		{
			std::vector<T> flat;
			flat.reserve(m.size() * m.size());
			for (const auto& row : m) {
				flat.insert(flat.end(), row.begin(), row.end());
			}
			return flat;
		}

		/**
		 * @brief Owning square `n x n` weight matrix stored contiguously, row-major.
		 *
		 * `m[v][w]` indexing (mutable and const) is kept, so code written for a
		 * nested matrix compiles unchanged. Copy and move are the defaults.
		 */
		template<class T>
		class weight_matrix_storage {
		public:
			using value_type = T;
			using row_type = weight_row_view<T>;
			using const_row_type = weight_row_view<const T>;
			using view_type = weight_matrix_view<const T>;
			using nested_t = std::vector<std::vector<T>>;

			std::size_t size() const noexcept { return n_; }
			bool empty() const noexcept { return n_ == 0; }
			T* data() noexcept { return buf_.data(); }
			const T* data() const noexcept { return buf_.data(); }

			row_type operator[](std::size_t v) noexcept {
				return row_type(buf_.data() + v * n_, n_);
			}
			const_row_type operator[](std::size_t v) const noexcept {
				return const_row_type(buf_.data() + v * n_, n_);
			}

			view_type view() const noexcept { return view_type(buf_.data(), n_); }

			void clear() noexcept { buf_.clear(); n_ = 0; }

			/** @brief Resizes to `n x n`, every entry set to @p fill. */
			void assign(std::size_t n, const T& fill) {
				if (n != 0 && n > buf_.max_size() / n) {
					throw std::length_error("weight_matrix_storage: n*n too large");
				}
				buf_.assign(n * n, fill);
				n_ = n;
			}

			/** @brief Copies a nested matrix. @pre @p m is square. */
			void assign_from(const nested_t& m) {
				std::vector<T> flat = flatten_matrix(m);
				buf_.swap(flat);
				n_ = m.size();
			}

			nested_t to_nested() const { return view().to_nested(); }

		private:
			std::vector<T> buf_;
			std::size_t n_ = 0;
		};

		template<class T, class U>
		bool operator==(const weight_matrix_view<T>& a,
			const weight_matrix_view<U>& b) noexcept
		{
			return a.size() == b.size() &&
				std::equal(a.data(), a.data() + a.size() * a.size(), b.data());
		}

		template<class T, class U>
		bool operator!=(const weight_matrix_view<T>& a,
			const weight_matrix_view<U>& b) noexcept
		{
			return !(a == b);
		}

		/** @brief Compares a view with a nested matrix (square and equal entries). */
		template<class T>
		bool operator==(const weight_matrix_view<T>& a,
			const std::vector<std::vector<typename weight_matrix_view<T>::value_type>>& b)
		{
			if (a.size() != b.size()) return false;
			for (std::size_t v = 0; v < a.size(); ++v) {
				const auto r = a[v];
				if (r.size() != b[v].size() ||
					!std::equal(r.begin(), r.end(), b[v].begin())) return false;
			}
			return true;
		}

		template<class T>
		bool operator==(
			const std::vector<std::vector<typename weight_matrix_view<T>::value_type>>& b,
			const weight_matrix_view<T>& a)
		{
			return a == b;
		}

		template<class T>
		bool operator!=(const weight_matrix_view<T>& a,
			const std::vector<std::vector<typename weight_matrix_view<T>::value_type>>& b)
		{
			return !(a == b);
		}

		template<class T>
		bool operator!=(
			const std::vector<std::vector<typename weight_matrix_view<T>::value_type>>& b,
			const weight_matrix_view<T>& a)
		{
			return !(a == b);
		}


	} // namespace detail

} // namespace bitgraph

#endif // BITGRAPH_GRAPH_DETAIL_EDGE_WEIGHT_MATRIX_VIEW_H
