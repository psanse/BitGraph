/**
* @file bench_masking.cpp
* @brief Why is Bitset::operator&= slower than std::bitset::operator&=? Variants of the same loop.
* @details Usage: bench_masking [nBits=100000] [min_ms=200]
*          Variants (all compute a &= b over the 64-bit words):
*           1. Bitset::operator&=            (library, bbset.cpp, vector accessed through the object)
*           2. loop on std::vector<BITBOARD>  inlined in this TU, indexed (same code as 1)
*           3. raw pointers                   a[i] &= b[i], n in a local
*           4. raw pointers + __restrict
*           5. std::bitset<N>                 (reference)
*          Prints ns per operation (the copy of the left operand is not included here: the variants
*          write in place, the operand is refilled by a cheap pass which is also measured and subtracted).
**/

#include "bitscan/bitscan.h"
#include <bitset>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>

using namespace bitgraph;
using clk = std::chrono::steady_clock;

namespace {

	volatile long long g_sink = 0;
	constexpr int NBITS_STATIC = 100000;

	template<class F>
	double time_ns(F&& f, int min_ms) {
		double best = 1e300;
		for (int round = 0; round < 3; ++round) {
			long long iters = 0;
			auto t0 = clk::now();
			double ms = 0;
			do {
				for (int i = 0; i < 64; ++i) f();
				iters += 64;
				ms = std::chrono::duration<double, std::milli>(clk::now() - t0).count();
			} while (ms < min_ms / 3.0);
			best = ms * 1e6 / iters < best ? ms * 1e6 / iters : best;
		}
		return best;
	}

	void and_indexed(std::vector<BITBOARD>& a, const std::vector<BITBOARD>& b, int n) {
		for (int i = 0; i < n; ++i) a[i] &= b[i];
	}

	void and_ptr(BITBOARD* a, const BITBOARD* b, int n) {
		for (int i = 0; i < n; ++i) a[i] &= b[i];
	}

	void and_restrict(BITBOARD* __restrict a, const BITBOARD* __restrict b, int n) {
		for (int i = 0; i < n; ++i) a[i] &= b[i];
	}
}

int main(int argc, char** argv) {
	const int nBits = argc > 1 ? std::atoi(argv[1]) : NBITS_STATIC;
	const int min_ms = argc > 2 ? std::atoi(argv[2]) : 200;
	if (nBits <= 0 || min_ms <= 0) { std::fputs("usage: bench_masking [nBits] [min_ms]\n", stderr); return 1; }
	const int n = (nBits + 63) / 64;

	std::mt19937_64 rng(1);
	Bitset a(nBits), b(nBits);
	for (int i = 0; i < n; ++i) { a.block(i) = rng(); b.block(i) = rng(); }
	std::vector<BITBOARD> va(n), vb(n);
	for (int i = 0; i < n; ++i) { va[i] = a.block(i); vb[i] = b.block(i); }

	// operand is not refilled: &= is idempotent after the first call, and the cost of the loop does not depend on the data
	std::printf("nBits=%d  words=%d\n\n", nBits, n);
	std::printf("%-40s %10.1f ns\n", "1. Bitset::operator&= (library)", time_ns([&] { a &= b; g_sink += a.block(0); }, min_ms));
	std::printf("%-40s %10.1f ns\n", "2. vector<BITBOARD>, indexed, inline", time_ns([&] { and_indexed(va, vb, n); g_sink += static_cast<long long>(va[0]); }, min_ms));
	std::printf("%-40s %10.1f ns\n", "3. raw pointers", time_ns([&] { and_ptr(va.data(), vb.data(), n); g_sink += static_cast<long long>(va[0]); }, min_ms));
	std::printf("%-40s %10.1f ns\n", "4. raw pointers + __restrict", time_ns([&] { and_restrict(va.data(), vb.data(), n); g_sink += static_cast<long long>(va[0]); }, min_ms));

	static std::bitset<NBITS_STATIC> sa, sb;
	for (int i = 0; i < NBITS_STATIC; i += 3) { sa.set(i); sb.set(NBITS_STATIC - 1 - i); }
	std::printf("%-40s %10.1f ns\n", "5. std::bitset<100000> (static size)", time_ns([&] { sa &= sb; g_sink += static_cast<long long>(sa.count()); }, min_ms) -
		time_ns([&] { g_sink += static_cast<long long>(sa.count()); }, min_ms));
	return 0;
}
