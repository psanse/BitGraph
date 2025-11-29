#include "bitscan/bbexpr.h"
#include "bitscan/bbscan.h"
#include "bitscan/bbalgorithm.h"

#include "gtest/gtest.h"

#include <iostream>
#include <chrono>
#include <vector>

using namespace bitgraph;

TEST(BBExpr, basic) {
	const int N = 301;
	BitSet a(N);
	BitSet b(N);
	std::vector<int> baseline_a;
	std::vector<int> baseline_b;
	std::vector<int> baseline_a_and_b;

	std::mt19937 rng{0};
	std::bernoulli_distribution dist;
	for(int i = 0; i <= N; ++i){
		const bool val_a = dist(rng);
		const bool val_b = dist(rng);
		if (val_a) {
			a.set_bit(i);
			baseline_a.emplace_back(i);
		}
		if (val_b) {
			b.set_bit(i);
			baseline_b.emplace_back(i);
		}
		if (val_a && val_b) {
			baseline_a_and_b.emplace_back(i);
		}
	}

	std::vector<int> test_a;
	for (int i : a)
		test_a.emplace_back(i);
	EXPECT_EQ(baseline_a, test_a);

	std::vector<int> test_b;
	for (int i : b)
		test_b.emplace_back(i);
	EXPECT_EQ(baseline_b, test_b);

	std::vector<int> test_a_and_b;
	for (int i : a & b)
		test_a_and_b.emplace_back(i);
	EXPECT_EQ(baseline_a_and_b, test_a_and_b);

	EXPECT_EQ(baseline_a, std::vector<int>(begin(a), end(a)));
	EXPECT_EQ(baseline_b, std::vector<int>(begin(b), end(b)));
	EXPECT_EQ(baseline_a_and_b, std::vector<int>(begin(a & b), end(a & b)));
}

template <typename BitSetT, typename RngT>
static std::vector<BitSetT> random_graph(int N, double p, RngT &rng) {
	std::vector<BitSetT> g(N, BitSetT(N));
	std::bernoulli_distribution dist{p};
	for (int i=0; i<N; ++i) {
		for (int j=0; j<i; ++j) {
			if (dist(rng)) {
				g[i].set_bit(j);
				g[j].set_bit(i);
			}
		}
	}
	return g;
}

// Copied/simplified from clq_func.h so that this module doesn't need to depend
// on src/graph.
template <typename BitSetT>
static int iseq_bbscan(const std::vector<BitSetT> &g, std::vector<int> &ub) {
	const int N = g.size();
	ub.assign(N, 0);
	int pc = N;
	int col = 1, v = bbo::noBit;
	BBScan bb_unsel(N, true);
	BBScan bb_sel(N);
	while (true) {
		bb_sel = bb_unsel;
		bb_sel.init_scan(bbo::DESTRUCTIVE);
		// FIXME using countr_zero in _BitScanForward64 makes this 10% faster
		while ((v = bb_sel.next_bit_del(bb_unsel)) != bbo::noBit) {
			ub[v] = col;
			if ((--pc) == 0) { return col; }
			bb_sel.erase_block(WDIV(v), -1, g[v]);
		}
		++col;
	}
}

// Just like iseq_bbscan but using iterators for the inner loop.
template <typename BitSetT>
static int iseq_bbexpr_v1(const std::vector<BitSetT> &g, std::vector<int> &ub) {
	const int N = g.size();
	ub.assign(N, 0);
	int pc = N;
	int col = 1;
	BitSet bb_unsel(N, true);
	BitSet bb_sel(N);
	while (true) {
		bb_sel = bb_unsel;
		for (int v : bb_sel) {
			bb_unsel.erase_bit(v);
			ub[v] = col;
			if ((--pc) == 0) { return col; }
			bb_sel.erase_block(WDIV(v), -1, g[v]);
		}
		++col;
	}
}

// Using block and bit indices from the iterator.
template <typename BitSetT>
static int iseq_bbexpr_v2(const std::vector<BitSetT> &g, std::vector<int> &ub) {
	const int N = g.size();
	ub.assign(N, 0);
	int pc = N;
	int col = 1;
	BitSet bb_unsel(N, true);
	BitSet bb_sel(N);
	while (true) {
		bb_sel = bb_unsel;
		for (auto it=begin(bb_sel), itEnd=end(bb_sel); it != itEnd; ++it) {
			int v = *it;
			//bb_unsel.erase_bit(v);
			bb_unsel.bitset().data()[it.block_index()] &= ~(BITBOARD{1} << it.bit_index());
			ub[v] = col;
			if ((--pc) == 0) { return col; }
			bb_sel.erase_block(it.block_index(), -1, g[v]);
		}
		++col;
	}
}

// Using a boolean expression as the object of the for loop.
// This is the fastest version.
template <typename BitSetT>
static int iseq_bbexpr_v3(const std::vector<BitSetT> &g, std::vector<int> &ub) {
	const int N = g.size();
	ub.assign(N, 0);
	int pc = N;
	int col = 1;
	BitSet bb_unsel(N, true);
	BitSet bb_neighbors(N);
	while (true) {
		bb_neighbors.erase_bit();
		for (int v : (bb_unsel & ~bb_neighbors)) {
			bb_unsel.erase_bit(v);
			bb_neighbors |= g[v];
			ub[v] = col;
			if ((--pc) == 0) { return col; }
		}
		++col;
	}
}

// Local bit vector variables are on stack with constexpr size.  The hope was
// this would allow vectorizing the loop in `bb_neighbors |= g[v]` using AVX
// instructions.  But for some reason it's slower.
// i7-1370P no-turbo benchmark: 200ms
template <typename BitSetT>
static int iseq_bbexpr_v4(const std::vector<BitSetT> &g, std::vector<int> &ub) {
	const int N = g.size();
	ub.assign(N, 0);
	int pc = N;
	int col = 1;
	assert(N <= 512);
	BBStatic<512> bb_unsel;
	bb_unsel.set_bit(0, N-1);
	BBStatic<512> bb_neighbors;
	while (true) {
		bb_neighbors.erase_bit();
		for (int v : (bb_unsel & ~bb_neighbors)) {
			bb_unsel.erase_bit(v);
			bb_neighbors |= g[v];
			ub[v] = col;
			if ((--pc) == 0) { return col; }
		}
		++col;
	}
}

// Returns nanoseconds per iteration.
template <typename Callback>
static int microBenchmark(int niter, Callback const &cb) {
	using ClockT = std::chrono::high_resolution_clock;
	auto t0 = ClockT::now();
	for (int iter=0; iter<niter; ++iter)
		cb();
	auto t1 = ClockT::now();
	return std::chrono::duration<double, std::nano>(t1 - t0).count() / niter;
}

TEST(BBExpr, benchmark_iset) {
	std::mt19937 rng{0}; // constant seed for benchmark timing consistency
	const int N = 500;
	const std::vector<BBScan> g = random_graph<BBScan>(N, 0.5, rng);

	// Compare that the results match.
	std::vector<int> colors_baseline(N);
	const int nc_baseline = iseq_bbscan(g, colors_baseline);
	{
		std::vector<int> colors(N);
		const int nc = iseq_bbexpr_v1(g, colors);
		EXPECT_EQ(colors, colors_baseline);
		EXPECT_EQ(nc, nc_baseline);
	}
	{
		std::vector<int> colors(N);
		const int nc = iseq_bbexpr_v2(g, colors);
		EXPECT_EQ(colors, colors_baseline);
		EXPECT_EQ(nc, nc_baseline);
	}
	{
		std::vector<int> colors(N);
		const int nc = iseq_bbexpr_v3(g, colors);
		EXPECT_EQ(colors, colors_baseline);
		EXPECT_EQ(nc, nc_baseline);
	}
	{
		std::vector<int> colors(N);
		const int nc = iseq_bbexpr_v4(g, colors);
		EXPECT_EQ(colors, colors_baseline);
		EXPECT_EQ(nc, nc_baseline);
	}

	std::vector<int> colors(N);
	double totalTimeBBScan = 0;
	double totalTimeBBExpr1 = 0;
	double totalTimeBBExpr2 = 0;
	double totalTimeBBExpr3 = 0;
	double totalTimeBBExpr4 = 0;
	// Run all versions round-robin several times so they will all be equally
	// affected if CPU frequency changes.
	const int numOuterIter = 20;
	for (int iter=0; iter<numOuterIter; ++iter) {
		totalTimeBBScan += microBenchmark(1000, [&] {
			const int nc = iseq_bbscan(g, colors);
			EXPECT_EQ(nc, 73);
		});
		totalTimeBBExpr1 += microBenchmark(1000, [&] {
			const int nc = iseq_bbexpr_v1(g, colors);
			EXPECT_EQ(nc, 73);
		});
		totalTimeBBExpr2 += microBenchmark(1000, [&] {
			const int nc = iseq_bbexpr_v2(g, colors);
			EXPECT_EQ(nc, 73);
		});
		totalTimeBBExpr3 += microBenchmark(1000, [&] {
			const int nc = iseq_bbexpr_v3(g, colors);
			EXPECT_EQ(nc, 73);
		});
		totalTimeBBExpr4 += microBenchmark(1000, [&] {
			const int nc = iseq_bbexpr_v4(g, colors);
			EXPECT_EQ(nc, 73);
		});
	}

	// i9-13900KF no-turbo times: (tends to vary from run to run)
	// timeBBScan  = 7713.4 ns/iter
	// timeBBExpr1 = 7777.59 ns/iter
	// timeBBExpr2 = 7805.15 ns/iter
	// timeBBExpr3 = 6357.06 ns/iter
	// timeBBExpr4 = 6712.91 ns/iter
	std::cout << "timeBBScan  = " << (totalTimeBBScan  / numOuterIter) << " ns/iter" << std::endl;
	std::cout << "timeBBExpr1 = " << (totalTimeBBExpr1 / numOuterIter) << " ns/iter" << std::endl;
	std::cout << "timeBBExpr2 = " << (totalTimeBBExpr2 / numOuterIter) << " ns/iter" << std::endl;
	std::cout << "timeBBExpr3 = " << (totalTimeBBExpr3 / numOuterIter) << " ns/iter" << std::endl;
	std::cout << "timeBBExpr4 = " << (totalTimeBBExpr4 / numOuterIter) << " ns/iter" << std::endl;
}
