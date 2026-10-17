/**
 * @file test_edge_weight_matrix_view.cpp
 * @brief Unit tests for the flat weight-matrix views and `flatten_matrix`.
 */

#include "graph/detail/edge_weight_matrix_view.h"
#include "gtest/gtest.h"

using namespace bitgraph::detail;

namespace {
	using nested = std::vector<std::vector<int>>;
	const nested M = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8} };
}

TEST(WeightMatrixView, flatten_is_row_major) {
	const std::vector<int> flat = flatten_matrix(M);
	ASSERT_EQ(9u, flat.size());
	for (int i = 0; i < 9; ++i) EXPECT_EQ(i, flat[i]);
	EXPECT_TRUE(flatten_matrix(nested{}).empty());
}

TEST(WeightMatrixView, indexing_size_and_rows) {
	const std::vector<int> flat = flatten_matrix(M);
	const weight_matrix_view<const int> v(flat.data(), 3);
	EXPECT_EQ(3u, v.size());
	EXPECT_FALSE(v.empty());
	EXPECT_EQ(5, v[1][2]);
	EXPECT_EQ(6, v[2][0]);
	EXPECT_EQ(3u, v[1].size());
	EXPECT_EQ(flat.data() + 3, v[1].data());				// contiguous rows
	int sum = 0;
	for (int x : v.row(2)) sum += x;
	EXPECT_EQ(21, sum);
}

TEST(WeightMatrixView, default_is_empty) {
	const weight_matrix_view<const int> v;
	EXPECT_TRUE(v.empty());
	EXPECT_EQ(0u, v.size());
	EXPECT_EQ(nested{}, v);
}

TEST(WeightMatrixView, equality_with_nested_and_views) {
	const std::vector<int> flat = flatten_matrix(M);
	const weight_matrix_view<const int> v(flat.data(), 3);
	EXPECT_EQ(M, v);
	EXPECT_EQ(v, M);
	EXPECT_TRUE(v == v);

	nested other = M;
	other[2][1] = -1;
	EXPECT_NE(other, v);
	EXPECT_TRUE(v != other);
	EXPECT_NE(nested(2, std::vector<int>(2, 0)), v);		// different size

	std::vector<int> flat2 = flat;
	const weight_matrix_view<const int> w(flat2.data(), 3);
	EXPECT_TRUE(v == w);
	flat2[4] = 99;
	EXPECT_TRUE(v != w);
}

TEST(WeightMatrixView, mutable_view_writes_through) {
	std::vector<int> flat = flatten_matrix(M);
	weight_matrix_view<int> v(flat.data(), 3);
	v[1][1] = 40;
	EXPECT_EQ(40, flat[4]);
	const weight_matrix_view<const int> c(flat.data(), 3);
	EXPECT_EQ(40, c[1][1]);
}

TEST(WeightMatrixView, to_nested_roundtrip) {
	const std::vector<int> flat = flatten_matrix(M);
	const weight_matrix_view<const int> v(flat.data(), 3);
	EXPECT_EQ(M, v.to_nested());
	EXPECT_TRUE(weight_matrix_view<const int>().to_nested().empty());
}

TEST(WeightMatrixStorage, assign_clear_and_indexing) {
	weight_matrix_storage<int> s;
	EXPECT_TRUE(s.empty());
	s.assign(3, -1);
	EXPECT_EQ(3u, s.size());
	EXPECT_EQ(-1, s[2][2]);
	s[1][2] = 7;
	const weight_matrix_storage<int>& c = s;
	EXPECT_EQ(7, c[1][2]);
	EXPECT_EQ(s.data() + 5, &s[1][2]);						// row-major
	EXPECT_EQ(3u, s[0].size());
	s.clear();
	EXPECT_TRUE(s.empty());
	EXPECT_EQ(0u, s.size());
}

TEST(WeightMatrixStorage, assign_from_and_to_nested) {
	weight_matrix_storage<int> s;
	s.assign_from(M);
	EXPECT_EQ(M, s.to_nested());
	EXPECT_EQ(M, s.view());
	s.assign_from(nested{});
	EXPECT_TRUE(s.empty());
}

TEST(WeightMatrixStorage, copy_is_deep_and_move_transfers) {
	weight_matrix_storage<int> s;
	s.assign_from(M);
	weight_matrix_storage<int> c(s);
	c[0][0] = 100;
	EXPECT_EQ(0, s[0][0]);
	EXPECT_NE(s.data(), c.data());

	weight_matrix_storage<int> m(std::move(c));
	EXPECT_EQ(100, m[0][0]);
	EXPECT_EQ(3u, m.size());
}

TEST(WeightMatrixStorage, assign_overflow_throws) {
	weight_matrix_storage<int> s;
	s.assign(2, 0);
	EXPECT_THROW(s.assign(static_cast<std::size_t>(-1), 0), std::length_error);
	EXPECT_EQ(2u, s.size());								// unchanged
}
