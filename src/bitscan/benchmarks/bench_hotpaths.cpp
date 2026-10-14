/**
* @file bench_hotpaths.cpp
* @brief Where the scanning bitsets (BBScan) are meant to be fast: scanning a dense bitset in different ways,
*        and the scan-while-modifying loop of bit-parallel graph algorithms, against std::bitset.
* @details Self-contained (std::chrono only). Build in Release.
*          Usage: bench_hotpaths [min_ms_per_case=200]
*
*          Part A - scanning a dense bitset of 100,000 bits: the ways BITSCAN offers (persistent scanner, direct
*                   cursor calls, independent view, stateless next_bit(bit), hand-written block loop) against
*                   std::bitset (libstdc++ _Find_first/_Find_next when available, portable loop otherwise).
*
*          Part B - bit-parallel greedy sequential colouring of a random graph. This is the classic hot path of
*                   exact algorithms (BBMC and relatives): repeatedly take the lowest vertex of a candidate set,
*                   remove its neighbours from the candidate set, and continue from there. The candidate set is
*                   modified while it is scanned. Both libraries run the same algorithm and must give the same
*                   number of colours (checked); only the bitset operations differ:
*                     std full width    : Q &= ~adj[v]            (std::bitset has no range-restricted operation)
*                     std precomputed   : Q &= nadj[v]            (non-adjacency rows precomputed: the best std can do,
*                                                                  at twice the memory)
*                     BG destructive    : Q.next_bit_del() + Q.andnot_eq(adj[v])
*                     BG destructive+rng: Q.next_bit_del() + Q.erase_block(block(v), npos, adj[v])
*                                         (only the blocks from the cursor on are touched: the lower ones are empty)
*
*          Part C - reverse scan: BITSCAN scanR against the only portable std::bitset alternative (a descending loop).
*
*          Output: ns per operation, or ns per bit scanned (Part A), or microseconds per full colouring (Part B).
**/

#include "bitscan/bitscan.h"
#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

#if defined(__has_include)
#if __has_include(<boost/dynamic_bitset.hpp>)
#include <boost/dynamic_bitset.hpp>
#define BENCH_HAS_BOOST 1
#endif
#endif

using namespace bitgraph;
using clk = std::chrono::steady_clock;

namespace {

	volatile long long g_sink = 0;

	template<class F>
	double time_ns(F&& f, int min_ms) {
		double best = 1e300;
		for (int round = 0; round < 3; ++round) {
			long long iters = 0;
			auto t0 = clk::now();
			double elapsed_ms = 0;
			do {
				f();
				++iters;
				elapsed_ms = std::chrono::duration<double, std::milli>(clk::now() - t0).count();
			} while (elapsed_ms < min_ms / 3.0);
			best = std::min(best, elapsed_ms * 1e6 / static_cast<double>(iters));
		}
		return best;
	}

	// ------------------------------------------------------------------ Part A

	constexpr int NPOP = 100000;
	constexpr int BITS_PER_BLOCK = 8;

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

	void row(const char* what, double occ, double ns, long long nbits) {
		std::printf("  occ=%3.0f%%  %-44s %10.1f ns  (%6.2f ns/bit)\n", occ * 100, what, ns, nbits ? ns / static_cast<double>(nbits) : 0.0);
	}

	void part_a(int min_ms) {
		std::printf("== Part A: scanning a dense bitset of %d bits (all set bits are visited) ==\n", NPOP);
		for (double occ : { 0.01, 0.10, 0.50 }) {
			std::mt19937_64 rng(12345);
			const std::vector<int> la = make_bits(NPOP, occ, rng);

			std::bitset<NPOP> sa;
			bitarray a(NPOP);
			for (int x : la) { sa.set(static_cast<std::size_t>(x)); a.set_bit(x); }
			const long long nbits = static_cast<long long>(a.count());

#if defined(__GLIBCXX__)
			row("std::bitset _Find_first/_Find_next (GCC only)", occ, time_ns([&] {
				long long c = 0;
				for (std::size_t i = sa._Find_first(); i < sa.size(); i = sa._Find_next(i)) c += static_cast<long long>(i);
				g_sink += c; }, min_ms), nbits);
#endif
#if defined(BENCH_HAS_BOOST)
			{
				boost::dynamic_bitset<> da(NPOP);
				for (int x : la) da.set(static_cast<std::size_t>(x));
				row("boost::dynamic_bitset find_first/find_next", occ, time_ns([&] {
					long long c = 0;
					for (std::size_t i = da.find_first(); i != boost::dynamic_bitset<>::npos; i = da.find_next(i)) c += static_cast<long long>(i);
					g_sink += c; }, min_ms), nbits);
			}
#endif
			row("std::bitset portable loop test(i)", occ, time_ns([&] {
				long long c = 0;
				for (std::size_t i = 0; i < sa.size(); ++i) if (sa.test(i)) c += static_cast<long long>(i);
				g_sink += c; }, min_ms), nbits);

			row("BITSCAN bitarray::scan (persistent scanner)", occ, time_ns([&] {
				long long c = 0;
				bitarray::scan s(a);
				for (int b = s.next_bit(); b != BBObject::noBit; b = s.next_bit()) c += b;
				g_sink += c; }, min_ms), nbits);

			row("BITSCAN init_scan + next_bit() on the bitset", occ, time_ns([&] {
				long long c = 0;
				a.init_scan(BBObject::NON_DESTRUCTIVE);
				for (int b = a.next_bit(); b != BBObject::noBit; b = a.next_bit()) c += b;
				g_sink += c; }, min_ms), nbits);

			row("BITSCAN view::scan (independent cursor)", occ, time_ns([&] {
				long long c = 0;
				view::scan v(a);
				for (int b = v.next_bit(); b != BBObject::noBit; b = v.next_bit()) c += b;
				g_sink += c; }, min_ms), nbits);

			row("BITSCAN stateless next_bit(bit)", occ, time_ns([&] {
				long long c = 0;
				for (int b = a.next_bit(BBObject::noBit); b != BBObject::noBit; b = a.next_bit(b)) c += b;
				g_sink += c; }, min_ms), nbits);

			row("hand-written block loop with bblock::lsb", occ, time_ns([&] {
				long long c = 0;
				const int nb = a.num_blocks();
				for (int i = 0; i < nb; ++i) {
					BITBOARD w = a.block(i);
					while (w) { c += i * 64 + bblock::lsb(w); w &= w - 1; }
				}
				g_sink += c; }, min_ms), nbits);
			std::printf("\n");
		}
	}

	// ------------------------------------------------------------------ Part B

	struct Edges { int n; std::vector<std::pair<int, int>> e; };

	Edges random_graph(int n, double p, unsigned long long seed) {
		Edges g{ n, {} };
		std::mt19937_64 rng(seed);
		std::uniform_real_distribution<double> u(0.0, 1.0);
		for (int i = 0; i < n; ++i) for (int j = i + 1; j < n; ++j) if (u(rng) < p) g.e.emplace_back(i, j);
		return g;
	}

	template<std::size_t N>
	struct StdColoring {
		std::vector<std::bitset<N>> adj, nadj;
		explicit StdColoring(const Edges& g) : adj(N), nadj(N) {
			for (auto& pr : g.e) { adj[pr.first].set(pr.second); adj[pr.second].set(pr.first); }
			for (std::size_t v = 0; v < N; ++v) nadj[v] = ~adj[v];
		}
		static std::size_t first(const std::bitset<N>& q) {
#if defined(__GLIBCXX__)
			return q._Find_first();
#else
			for (std::size_t i = 0; i < N; ++i) if (q.test(i)) return i;
			return N;
#endif
		}
		static std::size_t next(const std::bitset<N>& q, std::size_t v) {
#if defined(__GLIBCXX__)
			return q._Find_next(v);
#else
			for (std::size_t i = v + 1; i < N; ++i) if (q.test(i)) return i;
			return N;
#endif
		}
		int run_full() const {		// Q &= ~adj[v]
			std::bitset<N> U; U.set();
			int colors = 0;
			while (U.any()) {
				++colors;
				std::bitset<N> Q = U;
				for (std::size_t v = first(Q); v < N; ) {
					U.reset(v);
					Q &= ~adj[v];
					v = next(Q, v);
				}
			}
			return colors;
		}
		int run_precomputed() const {	// Q &= nadj[v]
			std::bitset<N> U; U.set();
			int colors = 0;
			while (U.any()) {
				++colors;
				std::bitset<N> Q = U;
				for (std::size_t v = first(Q); v < N; ) {
					U.reset(v);
					Q &= nadj[v];
					v = next(Q, v);
				}
			}
			return colors;
		}
	};

#if defined(BENCH_HAS_BOOST)
	/** @brief boost::dynamic_bitset: the runtime-sized competitor (std::bitset needs a compile-time size) */
	struct DynColoring {
		int n;
		std::vector<boost::dynamic_bitset<>> adj;
		explicit DynColoring(const Edges& g) : n(g.n), adj(static_cast<std::size_t>(g.n), boost::dynamic_bitset<>(static_cast<std::size_t>(g.n))) {
			for (auto& pr : g.e) { adj[pr.first].set(pr.second); adj[pr.second].set(pr.first); }
		}
		int run() const {		// Q -= adj[v]  (set difference, no temporary)
			boost::dynamic_bitset<> U(static_cast<std::size_t>(n)); U.set();
			boost::dynamic_bitset<> Q(static_cast<std::size_t>(n));
			int colors = 0;
			while (U.any()) {
				++colors;
				Q = U;
				for (std::size_t v = Q.find_first(); v != boost::dynamic_bitset<>::npos; ) {
					U.reset(v);
					Q -= adj[v];
					v = Q.find_next(v);
				}
			}
			return colors;
		}
	};
#endif

	struct BgColoring {
		int n;
		std::vector<bitarray> adj;
		explicit BgColoring(const Edges& g) : n(g.n), adj(static_cast<std::size_t>(g.n), bitarray(g.n)) {
			for (auto& pr : g.e) { adj[pr.first].set_bit(pr.second); adj[pr.second].set_bit(pr.first); }
		}
		int run_destructive() const {		// full-width andnot
			bitarray U(n); U.set_range(0, n - 1);
			bitarray Q(n);
			int colors = 0;
			while (!U.is_empty()) {
				++colors;
				Q = U;
				Q.init_scan(BBObject::DESTRUCTIVE);
				for (int v = Q.next_bit_del(); v != BBObject::noBit; v = Q.next_bit_del()) {
					U.erase_bit(v);
					Q.andnot_eq(adj[static_cast<std::size_t>(v)]);
				}
			}
			return colors;
		}
		int run_manual_blocks() const {		// the same algorithm written directly on the exposed 64-bit blocks
			bitarray U(n); U.set_range(0, n - 1);
			bitarray Q(n);
			const int nb = Q.num_blocks();
			int colors = 0;
			while (!U.is_empty()) {
				++colors;
				Q = U;
				for (int i = 0; i < nb; ++i) {
					BITBOARD w;
					while ((w = Q.block(i)) != 0) {
						const int v = i * 64 + bblock::lsb(w);
						U.erase_bit(v);
						const bitarray& A = adj[static_cast<std::size_t>(v)];
						Q.block(i) = (w & (w - 1)) & ~A.block(i);	// drop v, then its neighbours in this block
						for (int j = i + 1; j < nb; ++j) Q.block(j) &= ~A.block(j);
					}
				}
			}
			return colors;
		}
		int run_destructive_range() const {	// andnot restricted to the blocks from the cursor on
			bitarray U(n); U.set_range(0, n - 1);
			bitarray Q(n);
			int colors = 0;
			while (!U.is_empty()) {
				++colors;
				Q = U;
				Q.init_scan(BBObject::DESTRUCTIVE);
				for (int v = Q.next_bit_del(); v != BBObject::noBit; v = Q.next_bit_del()) {
					U.erase_bit(v);
					Q.erase_block(WDIV(v), BBObject::npos, adj[static_cast<std::size_t>(v)]);
				}
			}
			return colors;
		}
	};

	template<std::size_t N>
	void part_b_case(double p, int min_ms) {
		const Edges g = random_graph(static_cast<int>(N), p, 777);
		const StdColoring<N> s(g);
		const BgColoring b(g);

		const int c1 = s.run_full(), c2 = s.run_precomputed(), c3 = b.run_destructive(), c4 = b.run_destructive_range(), c5 = b.run_manual_blocks();
		if (c1 != c2 || c1 != c3 || c1 != c4 || c1 != c5) {
			std::printf("  MISMATCH in the number of colours: %d %d %d %d %d\n", c1, c2, c3, c4, c5);
			std::exit(2);
		}
		const double t1 = time_ns([&] { g_sink += s.run_full(); }, min_ms) / 1000.0;
		const double t2 = time_ns([&] { g_sink += s.run_precomputed(); }, min_ms) / 1000.0;
		const double t3 = time_ns([&] { g_sink += b.run_destructive(); }, min_ms) / 1000.0;
		const double t4 = time_ns([&] { g_sink += b.run_destructive_range(); }, min_ms) / 1000.0;
		const double t5 = time_ns([&] { g_sink += b.run_manual_blocks(); }, min_ms) / 1000.0;
		std::printf("  n=%-5zu p=%.1f  colours=%-4d  std full %8.1f | std precomp %8.1f | BG destr %8.1f | BG destr+range %8.1f | BG manual %8.1f (us)  manual vs std precomp: %.2fx\n",
			N, p, c1, t1, t2, t3, t4, t5, t2 / t5);
#if defined(BENCH_HAS_BOOST)
		{
			const DynColoring d(g);
			if (d.run() != c1) { std::printf("  MISMATCH in the number of colours (boost)\n"); std::exit(2); }
			const double t6 = time_ns([&] { g_sink += d.run(); }, min_ms) / 1000.0;
			std::printf("%34s boost::dynamic_bitset %8.1f (us)   BG destr+range vs boost: %.2fx   BG manual vs boost: %.2fx\n", "", t6, t6 / t4, t6 / t5);
		}
#endif
	}

	void part_b(int min_ms) {
		std::printf("== Part B: bit-parallel greedy sequential colouring (scan while modifying), microseconds per colouring ==\n");
		for (double p : { 0.1, 0.5, 0.9 }) part_b_case<256>(p, min_ms);
		for (double p : { 0.1, 0.5, 0.9 }) part_b_case<1024>(p, min_ms);
		for (double p : { 0.1, 0.5, 0.9 }) part_b_case<4096>(p, min_ms);
		std::printf("\n");
	}

	// ------------------------------------------------------------------ Part C

	void part_c(int min_ms) {
		std::printf("== Part C: reverse scan of a dense bitset of %d bits ==\n", NPOP);
		for (double occ : { 0.01, 0.10, 0.50 }) {
			std::mt19937_64 rng(12345);
			const std::vector<int> la = make_bits(NPOP, occ, rng);
			std::bitset<NPOP> sa;
			bitarray a(NPOP);
			for (int x : la) { sa.set(static_cast<std::size_t>(x)); a.set_bit(x); }
			const long long nbits = static_cast<long long>(a.count());

			row("std::bitset descending loop test(i)", occ, time_ns([&] {
				long long c = 0;
				for (std::size_t i = sa.size(); i-- > 0; ) if (sa.test(i)) c += static_cast<long long>(i);
				g_sink += c; }, min_ms), nbits);
			row("BITSCAN bitarray::scanR", occ, time_ns([&] {
				long long c = 0;
				bitarray::scanR s(a);
				for (int b = s.next_bit(); b != BBObject::noBit; b = s.next_bit()) c += b;
				g_sink += c; }, min_ms), nbits);
			std::printf("\n");
		}
	}
}

int main(int argc, char** argv) {
	const int min_ms = argc > 1 ? std::atoi(argv[1]) : 200;
	if (min_ms <= 0) { std::fputs("usage: bench_hotpaths [min_ms]\n", stderr); return 1; }
	std::printf("min_ms/case=%d\n\n", min_ms);
	part_a(min_ms);
	part_b(min_ms);
	part_c(min_ms);
	return 0;
}
