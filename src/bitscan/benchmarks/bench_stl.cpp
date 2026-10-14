/**
* @file bench_stl.cpp
* @brief Micro-benchmarks: STL bit containers vs the dense (Bitset / BBScan) and sparse (BitsetSp / BBScanSp)
*        bitsets of BitGraph.
* @details Self-contained (std::chrono only). Build in Release.
*          Usage: bench_stl [min_ms_per_case=100]
*          Fixed population NPOP bits (std::bitset needs a compile-time size).
*          Workload as in bench_bitset.cpp: a fraction ("occupancy") of the 64-bit blocks is populated,
*          each with BITS_PER_BLOCK random bits.
*
*          Containers:
*           - std::bitset<NPOP>: bitwise operators, count(), test(); no portable find-next, so scanning
*             is the usual loop over test(i)
*           - std::vector<bool>: no bitwise operators (not measured), no find-next, count via std::count
*           - Bitset / BBScan (dense), BitsetSp / BBScanSp (sparse)
*          Mutating operations include a copy of the left operand, whose cost is measured separately
*          and subtracted ("net"). Output: ns/op.
**/

#include "bitscan/bitscan.h"
#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

using namespace bitgraph;
using clk = std::chrono::steady_clock;

namespace {

	constexpr int NPOP = 100000;
	constexpr int BITS_PER_BLOCK = 8;
	volatile long long g_sink = 0;

	std::vector<int> make_bits(int nPop, double occupancy, std::mt19937_64& rng) {
		std::vector<int> v;
		const int nBB = (nPop + 63) / 64;
		std::bernoulli_distribution occupied(occupancy);
		std::uniform_int_distribution<int> pos(0, 63);
		for (int bb = 0; bb < nBB; ++bb) {
			if (!occupied(rng)) continue;
			for (int k = 0; k < BITS_PER_BLOCK; ++k) {
				const int bit = bb * 64 + pos(rng);
				if (bit < nPop) v.push_back(bit);
			}
		}
		return v;
	}

	template<class F>
	double time_ns(F&& f, int min_ms) {
		double best = 1e300;
		for (int round = 0; round < 3; ++round) {
			long long iters = 0;
			auto t0 = clk::now();
			double elapsed_ms = 0;
			do {
				for (int i = 0; i < 16; ++i) f();
				iters += 16;
				elapsed_ms = std::chrono::duration<double, std::milli>(clk::now() - t0).count();
			} while (elapsed_ms < min_ms / 3.0);
			best = std::min(best, elapsed_ms * 1e6 / iters);
		}
		return best;
	}

	void report(const char* type, double occ, const char* op, double ns) {
		std::printf("%-12s occ=%4.0f%%  %-26s %12.1f ns/op\n", type, occ * 100, op, ns);
	}

	/** @brief std::bitset<NPOP> and any type with the same operators / count / test / set */
	template<class BS>
	void run_ops(const char* type, double occ, int min_ms, const std::vector<int>& la, const std::vector<int>& lb) {

		auto fill = [](BS& b, const std::vector<int>& l) { for (int x : l) b.set(static_cast<std::size_t>(x)); };
		BS a, b;
		fill(a, la);
		fill(b, lb);
		BS tmp(a);

		const double copy_ns = time_ns([&] { tmp = a; g_sink += static_cast<long long>(tmp.count()); }, min_ms)
							 - time_ns([&] { g_sink += static_cast<long long>(tmp.count()); }, min_ms);
		auto net = [&](double ns) { return ns > copy_ns ? ns - copy_ns : 0.0; };

		report(type, occ, "set_bit (build from list)", time_ns([&] { BS x; fill(x, la); g_sink += static_cast<long long>(x.count()); }, min_ms));
		report(type, occ, "is_bit (all of list)", time_ns([&] { long long c = 0; for (int v : la) c += a.test(static_cast<std::size_t>(v)); g_sink += c; }, min_ms));
		report(type, occ, "count", time_ns([&] { g_sink += static_cast<long long>(a.count()); }, min_ms));
		report(type, occ, "&=  (net)", net(time_ns([&] { tmp = a; tmp &= b; g_sink += static_cast<long long>(tmp.count()); }, min_ms)));
		report(type, occ, "|=  (net)", net(time_ns([&] { tmp = a; tmp |= b; g_sink += static_cast<long long>(tmp.count()); }, min_ms)));
		report(type, occ, "^=  (net)", net(time_ns([&] { tmp = a; tmp ^= b; g_sink += static_cast<long long>(tmp.count()); }, min_ms)));
		report(type, occ, "andnot (a &= ~b) (net)", net(time_ns([&] { tmp = a; tmp &= ~b; g_sink += static_cast<long long>(tmp.count()); }, min_ms)));
		report(type, occ, "is_disjoint ((a&b).none())", time_ns([&] { g_sink += (a & b).none(); }, min_ms));
		report(type, occ, "scan (loop over test(i))", time_ns([&] {
			long long c = 0;
			for (std::size_t i = 0; i < a.size(); ++i) if (a.test(i)) c += static_cast<long long>(i);
			g_sink += c; }, min_ms));
#if defined(__GLIBCXX__)
		// libstdc++ extension (non-portable): the fastest scan std::bitset offers on GCC
		report(type, occ, "scan (libstdc++ _Find_first/_Find_next)", time_ns([&] {
			long long c = 0;
			for (std::size_t i = a._Find_first(); i < a.size(); i = a._Find_next(i)) c += static_cast<long long>(i);
			g_sink += c; }, min_ms));
#endif
	}

	void run_vector_bool(double occ, int min_ms, const std::vector<int>& la) {
		const char* type = "vector<bool>";
		std::vector<bool> a(NPOP);
		for (int x : la) a[static_cast<std::size_t>(x)] = true;

		report(type, occ, "set_bit (build from list)", time_ns([&] {
			std::vector<bool> x(NPOP); for (int v : la) x[static_cast<std::size_t>(v)] = true; g_sink += x[1]; }, min_ms));
		report(type, occ, "is_bit (all of list)", time_ns([&] { long long c = 0; for (int v : la) c += a[static_cast<std::size_t>(v)]; g_sink += c; }, min_ms));
		report(type, occ, "count", time_ns([&] { g_sink += static_cast<long long>(std::count(a.begin(), a.end(), true)); }, min_ms));
		report(type, occ, "scan (loop over a[i])", time_ns([&] {
			long long c = 0;
			for (std::size_t i = 0; i < a.size(); ++i) if (a[i]) c += static_cast<long long>(i);
			g_sink += c; }, min_ms));
	}

	/** @brief BitGraph bitsets: shared API plus the persistent scanner of the scanning class */
	template<class BS, class SC>
	void run_bitgraph(const char* type, double occ, int min_ms, const std::vector<int>& la, const std::vector<int>& lb) {

		SC a(NPOP);
		BS b(NPOP);
		for (int x : la) a.set_bit(x);
		for (int x : lb) b.set_bit(x);
		SC tmp(a);
		const BS& bb = b;

		const double copy_ns = time_ns([&] { tmp = a; g_sink += tmp.count(); }, min_ms) - time_ns([&] { g_sink += tmp.count(); }, min_ms);
		auto net = [&](double ns) { return ns > copy_ns ? ns - copy_ns : 0.0; };
		(void)bb;

		report(type, occ, "set_bit (build from list)", time_ns([&] { SC x(NPOP); for (int v : la) x.set_bit(v); g_sink += x.count(); }, min_ms));
		report(type, occ, "is_bit (all of list)", time_ns([&] { long long c = 0; for (int v : la) c += a.is_bit(v); g_sink += c; }, min_ms));
		report(type, occ, "count", time_ns([&] { g_sink += a.count(); }, min_ms));

		SC sb(NPOP);
		for (int x : lb) sb.set_bit(x);
		report(type, occ, "&=  (net)", net(time_ns([&] { tmp = a; tmp &= sb; g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "|=  (net)", net(time_ns([&] { tmp = a; tmp |= sb; g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "^=  (net)", net(time_ns([&] { tmp = a; tmp ^= sb; g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "andnot (erase_bit(rhs)) (net)", net(time_ns([&] { tmp = a; tmp.erase_bit(sb); g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "is_disjoint", time_ns([&] { g_sink += a.is_disjoint(sb); }, min_ms));
		report(type, occ, "scan (persistent scanner)", time_ns([&] {
			long long c = 0; typename SC::scan s(a);
			for (int v = s.next_bit(); v != BBObject::noBit; v = s.next_bit()) c += v;
			g_sink += c; }, min_ms));
	}
}

int main(int argc, char** argv) {
	const int min_ms = argc > 1 ? std::atoi(argv[1]) : 100;
	if (min_ms <= 0) { std::fputs("usage: bench_stl [min_ms]\n", stderr); return 1; }

	std::printf("nPop=%d  bits/occupied block=%d  min_ms/case=%d\n\n", NPOP, BITS_PER_BLOCK, min_ms);
	for (double occ : { 0.01, 0.10, 0.50 }) {
		std::mt19937_64 rng(12345);
		const std::vector<int> la = make_bits(NPOP, occ, rng);
		const std::vector<int> lb = make_bits(NPOP, occ, rng);

		run_ops<std::bitset<NPOP>>("std::bitset", occ, min_ms, la, lb);
		run_vector_bool(occ, min_ms, la);
		run_bitgraph<BBScan, BBScan>("BG dense", occ, min_ms, la, lb);
		run_bitgraph<BBScanSp, BBScanSp>("BG sparse", occ, min_ms, la, lb);
		std::printf("\n");
	}
	return 0;
}
