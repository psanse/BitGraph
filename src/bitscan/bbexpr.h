#pragma once

#include "bbtypes.h"
#include "bbconfig.h"
#include "bitblock.h"

#include <bit>
#include <type_traits>
#include <cassert>
#include <functional>

namespace bitgraph {

//////////////////////////////////////////////////////////////////////
/// Forward declarations
//////////////////////////////////////////////////////////////////////

struct BBIterBeginTag { };
struct BBIterEndTag { };

template <typename Obj>
class BBIter;

//////////////////////////////////////////////////////////////////////
/// BBExpr
//////////////////////////////////////////////////////////////////////

template <typename E>
struct BBExpr {
	E const &bbexpr_cast() const { return static_cast<E const &>(*this); }
	E       &bbexpr_cast()       { return static_cast<E       &>(*this); }

	int capacity() const { return bbexpr_cast().bbexpr_num_blocks()*64; }

	BBIter<E> begin() const {
		return BBIter<E>{*this, BBIterBeginTag{}};
	}

	BBIter<E> end() const {
		return BBIter<E>{*this, BBIterEndTag{}};
	}
};

//////////////////////////////////////////////////////////////////////
/// BBExprMutable
//////////////////////////////////////////////////////////////////////

template <typename E>
struct BBExprMutable : public BBExpr<E> {
	E const &bbexpr_cast() const { return static_cast<E const &>(*this); }
	E       &bbexpr_cast()       { return static_cast<E       &>(*this); }

	E &erase_bit() {
		E &self = bbexpr_cast();
		for (int i=0; i<self.bbexpr_num_blocks(); ++i)
			self.bbexpr_get_block(i) = 0;
		return self;
	}

	E &erase_bit(int i) {
		E &self = bbexpr_cast();
		assert(i >= 0 && i < capacity());
		self.bbexpr_get_block(WDIV(i)) &= ~(BITBOARD{1} << WMOD(i));
		return self;
	}

	E &set_bit(int firstBit, int lastBit) {
		E &self = bbexpr_cast();
		assert(firstBit >= 0 && firstBit <= lastBit);
		assert(lastBit < capacity());

		int bbl = WDIV(firstBit);
		int bbh = WDIV(lastBit);

		if (bbl == bbh)
		{
			self.bbexpr_get_block(bbh) |= bblock::MASK_1(firstBit - WMUL(bbl), lastBit - WMUL(bbh));
		}
		else
		{
			//set to one the intermediate blocks
			for (int i = bbl + 1; i < bbh; ++i) {
				self.bbexpr_get_block(i) = ONE;
			}

			//sets the first and last blocks
			self.bbexpr_get_block(bbh) |= bblock::MASK_1_LOW(lastBit - WMUL(bbh));
			self.bbexpr_get_block(bbl) |= bblock::MASK_1_HIGH(firstBit - WMUL(bbl));

		}

		return self;
	}

	/**
	* @brief Overwrites this bitstring with @bb_add (equivalent to operator=)
	*		  
	*		 Note: The bitblock size of bb_add must be at least as large as this bitstring.
	* 
	* @param bb_add: input bitstring whose bits are copied
	* returns reference to the modified bitstring
	**/
	template <typename RHS>
	E &assign_bit(BBExpr<RHS> const &rhs) {
		E &self = bbexpr_cast();
		RHS const &rhsCast = rhs.bbexpr_cast();
		assert(self.bbexpr_num_blocks() <= rhsCast.bbexpr_num_blocks());
		for (int i=0; i<self.bbexpr_num_blocks(); ++i)
			self.bbexpr_get_block(i) = rhsCast.bbexpr_get_block(i);
		return self;
	}

	template <typename RHS>
	E &operator&=(BBExpr<RHS> const &rhs) {
		return assign_bit(*this & rhs);
	}

	template <typename RHS>
	E &operator|=(BBExpr<RHS> const &rhs) {
		return assign_bit(*this | rhs);
	}

	template <typename RHS>
	E &operator^=(BBExpr<RHS> const &rhs) {
		return assign_bit(*this ^ rhs);
	}
};

//////////////////////////////////////////////////////////////////////
/// BBIter
//////////////////////////////////////////////////////////////////////

template <typename Obj>
class BBIter {
public:
	using value_type = int;
	using difference_type = ptrdiff_t;
	using pointer = void;
	using reference = const int &;
	using iterator_category = std::forward_iterator_tag;

private:
	std::conditional_t<Obj::bbexpr_use_ref, const Obj &, Obj> obj_;
	int bbi_;
	int pos_;

public:
	BBIter(BBExpr<Obj> const &bbdata, BBIterBeginTag) :
		obj_(bbdata.bbexpr_cast()),
		bbi_(0)
	{
		BITBOARD x;
		while (bbi_ < obj_.bbexpr_num_blocks() && !(x=obj_.bbexpr_get_block(bbi_)))
			++bbi_;
		if (bbi_ < obj_.bbexpr_num_blocks()) {
			pos_ = lowest_bit_assuming_nonzero(x);
		} else {
			pos_ = 0;
		}
	}

	BBIter(BBExpr<Obj> const &bbdata, BBIterEndTag) :
		obj_(bbdata.bbexpr_cast()),
		bbi_(bbdata.bbexpr_cast().bbexpr_num_blocks()),
		pos_(0)
	{ }

	bool operator==(BBIter const &o) const {
		return bbi_ == o.bbi_ && pos_ == o.pos_;
	}

	bool operator!=(BBIter const &o) const {
		return !(*this == o);
	}

	BBIter &operator++() {
		BITBOARD x = obj_.bbexpr_get_block(bbi_) & ((~BITBOARD{1}) << pos_);
		if (x) {
			pos_ = lowest_bit_assuming_nonzero(x);
		} else {
			const int n = obj_.bbexpr_num_blocks();
			++bbi_;
			while (bbi_ < n && !(x=obj_.bbexpr_get_block(bbi_)))
				++bbi_;
			if (bbi_ < n) {
				pos_ = lowest_bit_assuming_nonzero(x);
			} else {
				pos_ = 0;
			}
		}
		return *this;
	}

	int operator*() const {
		return pos_ + WMUL(bbi_);
	}

	int block_index() const { return bbi_; }
	int bit_index() const { return pos_; }

private:
	static int lowest_bit_assuming_nonzero(BITBOARD x) {
		// FIXME use countr_zero if C++20
		//__attribute__((assume(x != 0)));
		//return std::countr_zero(x);
		return __builtin_ctzll(x);
	}
};

template <typename Obj>
BBIter<Obj> begin(BBExpr<Obj> const &obj) {
	return BBIter<Obj>{obj, BBIterBeginTag{}};
}

template <typename Obj>
BBIter<Obj> end(BBExpr<Obj> const &obj) {
	return BBIter<Obj>{obj, BBIterEndTag{}};
}

//////////////////////////////////////////////////////////////////////
/// Operations
//////////////////////////////////////////////////////////////////////

template <typename LHS, typename RHS, typename Op>
struct BBBinaryOp : public BBExpr<BBBinaryOp<LHS, RHS, Op>> {
	static constexpr bool bbexpr_use_ref = false;
	std::conditional_t<LHS::bbexpr_use_ref, const LHS &, LHS> lhs_;
	std::conditional_t<RHS::bbexpr_use_ref, const RHS &, RHS> rhs_;
	Op op_;

	BBBinaryOp(BBExpr<LHS> const &lhs, BBExpr<RHS> const &rhs, Op const &op=Op{}) :
		lhs_(lhs.bbexpr_cast()), rhs_(rhs.bbexpr_cast()), op_(op)
	{
		assert(lhs_.bbexpr_num_blocks() == lhs_.bbexpr_num_blocks());
	}

	int bbexpr_num_blocks() const { return lhs_.bbexpr_num_blocks(); }

	BITBOARD bbexpr_get_block(int i) const {
		return op_(lhs_.bbexpr_get_block(i), rhs_.bbexpr_get_block(i));
	}
};

template <typename Obj, typename Op>
struct BBUnaryOp : public BBExpr<BBUnaryOp<Obj, Op>> {
	static constexpr bool bbexpr_use_ref = false;
	std::conditional_t<Obj::bbexpr_use_ref, const Obj &, Obj> obj_;
	Op op_;

	BBUnaryOp(BBExpr<Obj> const &obj, Op const &op=Op{}) :
		obj_(obj.bbexpr_cast()), op_(op)
	{ }

	int bbexpr_num_blocks() const { return obj_.bbexpr_num_blocks(); }

	BITBOARD bbexpr_get_block(int i) const { return ~obj_.bbexpr_get_block(i); }
};

template <typename LHS, typename RHS>
BBBinaryOp<LHS, RHS, std::bit_and<BITBOARD>> operator&(BBExpr<LHS> const &lhs, BBExpr<RHS> const &rhs) {
	return {lhs, rhs};
}

template <typename LHS, typename RHS>
BBBinaryOp<LHS, RHS, std::bit_or<BITBOARD>> operator|(BBExpr<LHS> const &lhs, BBExpr<RHS> const &rhs) {
	return {lhs, rhs};
}

template <typename LHS, typename RHS>
BBBinaryOp<LHS, RHS, std::bit_xor<BITBOARD>> operator^(BBExpr<LHS> const &lhs, BBExpr<RHS> const &rhs) {
	return {lhs, rhs};
}

template <typename Obj>
BBUnaryOp<Obj, std::bit_not<BITBOARD>> operator~(BBExpr<Obj> const &obj) {
	return {obj};
}

//////////////////////////////////////////////////////////////////////
/// Containers
//////////////////////////////////////////////////////////////////////

// FIXME should have const and non-const, with or without constexpr length
struct BBSpan : public BBExpr<BBSpan> {
	BITBOARD const *vBB_;
	int nBB_;

	BBSpan(BITBOARD const *vBB, int nBB) :
		vBB_(vBB),
		nBB_(nBB)
	{ }

	static constexpr bool bbexpr_use_ref = false;
	int bbexpr_num_blocks() const { return nBB_; }
	BITBOARD bbexpr_get_block(int i) const { return vBB_[i]; }
};

template <int N>
struct BBStatic : public BBExprMutable<BBStatic<N>> {
	static_assert(N >= 0);
	static constexpr int nBB_ = (N+63)/64;

	//alignas(64) // For AVX512 instructions.  Doesn't seem to make it faster.
	std::array<BITBOARD, nBB_> vBB_;

	BBStatic() :
		vBB_{} /*zero-initialized*/
	{ }

	static constexpr bool bbexpr_use_ref = true;
	static constexpr int bbexpr_num_blocks() { return nBB_; }
	BITBOARD  bbexpr_get_block(int i) const { assert(i >= 0 && i < nBB_); return vBB_[i]; }
	BITBOARD &bbexpr_get_block(int i)       { assert(i >= 0 && i < nBB_); return vBB_[i]; }
};

} // namespace bitgraph
