/**
* @file bench_bitset.cpp
* @brief Micro-benchmarks of the common API of Bitset (dense) and BitsetSp (sparse).
* @details Self-contained (std::chrono only). Build in Release.
*          Usage: bench_bitset [nPop=100000] [min_ms_per_case=100]
*          Workload: a fraction ("occupancy") of the 64-bit blocks is populated, each with
*          BITS_PER_BLOCK random bits. Mutating operations include a copy of the left operand,
*          whose cost is measured separately and subtracted ("net").
*          Output: ns/op per operation, bitset type and occupancy.
**/

#include "bitscan/bitscan.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

using namespace bitgraph;
using clk = std::chrono::steady_clock;

namespace {

	constexpr int BITS_PER_BLOCK = 8;
	volatile long long g_sink = 0;				// defeats dead-code elimination

	/** @brief random list of bits: 'occupancy' of the blocks populated with BITS_PER_BLOCK bits each */
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

	/** @brief runs f() repeatedly for at least min_ms, returns the best-of-3 ns per call */
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
		std::printf("%-8s occ=%4.0f%%  %-26s %12.1f ns/op\n", type, occ * 100, op, ns);
	}

	template<class BS>
	void run(const char* type, int nPop, double occ, int min_ms) {
		std::mt19937_64 rng(12345);
		const std::vector<int> la = make_bits(nPop, occ, rng);
		const std::vector<int> lb = make_bits(nPop, occ, rng);

		BS a(nPop), b(nPop);
		for (int x : la) a.set_bit(x);
		for (int x : lb) b.set_bit(x);

		BS tmp(a);
		const double copy_ns = time_ns([&] { tmp = a; g_sink += tmp.count(); }, min_ms)
							 - time_ns([&] { g_sink += tmp.count(); }, min_ms);

		auto net = [&](double ns) { return ns > copy_ns ? ns - copy_ns : 0.0; };

		report(type, occ, "copy (baseline, net)", copy_ns);
		report(type, occ, "set_bit (build from list)", time_ns([&] {
			BS x(nPop); for (int v : la) x.set_bit(v); g_sink += x.count(); }, min_ms));
		report(type, occ, "is_bit (all of list)", time_ns([&] {
			long long c = 0; for (int v : la) c += a.is_bit(v); g_sink += c; }, min_ms));
		report(type, occ, "count", time_ns([&] { g_sink += a.count(); }, min_ms));
		report(type, occ, "&=  (net)", net(time_ns([&] { tmp = a; tmp &= b; g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "|=  (net)", net(time_ns([&] { tmp = a; tmp |= b; g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "erase_bit(rhs) (net)", net(time_ns([&] { tmp = a; tmp.erase_bit(b); g_sink += tmp.count(); }, min_ms)));
		report(type, occ, "^=  (net)", net(time_ns([&] { tmp = a; tmp ^= b; g_sink += tmp.count(); }, min_ms)));
		{
			BS res(nPop);
			report(type, occ, "AND(a,b,res)", time_ns([&] { AND(a, b, res); g_sink += res.count(); }, min_ms));
			report(type, occ, "OR(a,b,res)", time_ns([&] { OR(a, b, res); g_sink += res.count(); }, min_ms));
		}
		report(type, occ, "is_disjoint", time_ns([&] { g_sink += a.is_disjoint(b); }, min_ms));
		report(type, occ, "next_bit loop", time_ns([&] {
			long long c = 0;
			for (int v = a.next_bit(BBObject::noBit); v != BBObject::noBit; v = a.next_bit(v)) c += v;
			g_sink += c; }, min_ms));
	}

	/** @brief persistent (cursor in the bitset) and independent (view) scanners; BS is BBScan or BBScanSp */
	template<class BS, class FwdView, class RevView>
	void run_scan(const char* type, int nPop, double occ, int min_ms) {
		std::mt19937_64 rng(12345);
		const std::vector<int> la = make_bits(nPop, occ, rng);

		BS a(nPop);
		for (int x : la) a.set_bit(x);
		BS tmp(a);

		report(type, occ, "scan: persistent fwd", time_ns([&] {
			long long c = 0; typename BS::scan s(a);
			for (int v = s.next_bit(); v != BBObject::noBit; v = s.next_bit()) c += v;
			g_sink += c; }, min_ms));
		report(type, occ, "scan: persistent rev", time_ns([&] {
			long long c = 0; typename BS::scanR s(a);
			for (int v = s.next_bit(); v != BBObject::noBit; v = s.next_bit()) c += v;
			g_sink += c; }, min_ms));
		report(type, occ, "scan: view fwd", time_ns([&] {
			long long c = 0; FwdView s(a);
			for (int v = s.next_bit(); v != BBObject::noBit; v = s.next_bit()) c += v;
			g_sink += c; }, min_ms));
		report(type, occ, "scan: view rev", time_ns([&] {
			long long c = 0; RevView s(a);
			for (int v = s.next_bit(); v != BBObject::noBit; v = s.next_bit()) c += v;
			g_sink += c; }, min_ms));
		const double copy_ns = time_ns([&] { tmp = a; g_sink += tmp.count(); }, min_ms)
							 - time_ns([&] { g_sink += tmp.count(); }, min_ms);
		const double d = time_ns([&] {
			tmp = a;
			long long c = 0; typename BS::scanD s(tmp);
			for (int v = s.next_bit(); v != BBObject::noBit; v = s.next_bit()) c += v;
			g_sink += c; }, min_ms);
		report(type, occ, "scan: destructive (net)", d > copy_ns ? d - copy_ns : 0.0);
	}

}

int main(int argc, char** argv) {
	const int nPop   = argc > 1 ? std::atoi(argv[1]) : 100000;
	const int min_ms = argc > 2 ? std::atoi(argv[2]) : 100;
	if (nPop <= 0 || min_ms <= 0) { std::fputs("usage: bench_bitset [nPop] [min_ms]\n", stderr); return 1; }

	std::printf("nPop=%d  bits/occupied block=%d  min_ms/case=%d\n\n", nPop, BITS_PER_BLOCK, min_ms);
	for (double occ : {0.01, 0.10, 0.50}) {
		run<Bitset>("dense", nPop, occ, min_ms);
		run<BitsetSp>("sparse", nPop, occ, min_ms);
		run_scan<BBScan, view::Forward, view::Reverse>("dense", nPop, occ, min_ms);
		run_scan<BBScanSp, view::ForwardSp, view::ReverseSp>("sparse", nPop, occ, min_ms);
		std::printf("\n");
	}
	return 0;
}
