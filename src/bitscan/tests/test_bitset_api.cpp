/**
 * @file test_bitset_api.cpp
 * @brief Typed tests of the API shared by Bitset (dense) and BitsetSp (sparse).
 *
 * The same test bodies run for both classes, so a drift in either API fails the tests.
 *
 * Group 1 (compile-time): every shared member function exists in both classes with the
 *   same return type (C++14 detection with decltype / static_assert).
 * Group 2 (run-time): construction, resetting, range operations, conversions and I/O,
 *   which behave the same in both classes.
 *
 * Not repeated here (already covered elsewhere):
 *  - queries, masking and extraction against a reference: test_bitset_sparse.cpp,
 *    Sparse.differential_against_bitset;
 *  - behavior specific to one class (zero blocks, block storage, scan cursors): the
 *    tests of each class.
 *
 * Deliberately different (not tested as shared): size() and num_blocks() (stored blocks vs
 * blocks of the universe), block() / bitset() (storage), the sparse-only block functions,
 * flip() and the (nPop, true) constructor.
 *
 * @date Created: 14/10/2026
 * @author P. San Segundo and Claude
 */

#include "bitscan/bitscan.h"
#include "gtest/gtest.h"
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace bitgraph;

template<class T>
class BitsetApiTest : public ::testing::Test {};

using BitsetTypes = ::testing::Types<Bitset, BitsetSp>;
TYPED_TEST_SUITE(BitsetApiTest, BitsetTypes);

namespace {

	template<class T> using Decl = decltype(std::declval<T&>());
	template<class T> using CDecl = decltype(std::declval<const T&>());

	template<class B>
	std::vector<int> bits_of(const B& b) { return static_cast<std::vector<int>>(b); }

	template<class B>
	std::vector<int> scan(const B& b) {
		std::vector<int> v;
		for (int x = b.next_bit(BBObject::noBit); x != BBObject::noBit; x = b.next_bit(x)) v.push_back(x);
		return v;
	}
}

//////////////////////////////////////////////
// Group 1: same API at compile time
//////////////////////////////////////////////

#define B_ std::declval<B&>()
#define CB_ std::declval<const B&>()
#define LV_ std::declval<std::vector<int>&>()
#define API_RETURNS(expr, type) \
	static_assert(std::is_same<decltype(expr), type>::value, "unexpected return type: " #expr)

TYPED_TEST(BitsetApiTest, member_functions_returning_a_reference_to_the_bitset) {
	using B = TypeParam;

	API_RETURNS(B_.set_bit(0), B&);
	API_RETURNS(B_.set_bit(0, 1), B&);
	API_RETURNS(B_.set_bit(CB_), B&);
	API_RETURNS(B_.set_bit(LV_), B&);
	API_RETURNS(B_.set(0), B&);
	API_RETURNS(B_.erase_bit(0), B&);
	API_RETURNS(B_.erase_bit(0, 1), B&);
	API_RETURNS(B_.erase_bit(), B&);
	API_RETURNS(B_.erase_bit(CB_), B&);
	API_RETURNS(B_.clear(0), B&);
	API_RETURNS(B_.clear_all(), B&);
	API_RETURNS(B_.erase_all_bits(), B&);
	API_RETURNS(B_.andnot_eq(CB_), B&);
	API_RETURNS(B_.and_eq(CB_), B&);
	API_RETURNS(B_.or_eq(CB_), B&);
	API_RETURNS(B_.assign_bit(CB_), B&);
	API_RETURNS(B_.set_block(0, 1, CB_), B&);
	API_RETURNS(B_.erase_block(0, 1, CB_), B&);
	API_RETURNS(B_ &= CB_, B&);
	API_RETURNS(B_ |= CB_, B&);
	API_RETURNS(B_ ^= CB_, B&);
	SUCCEED();
}

TYPED_TEST(BitsetApiTest, member_functions_returning_values) {
	using B = TypeParam;

	API_RETURNS(CB_.count(), int);
	API_RETURNS(CB_.msb(), int);
	API_RETURNS(CB_.lsb(), int);
	API_RETURNS(CB_.next_bit(0), int);
	API_RETURNS(CB_.prev_bit(0), int);
	API_RETURNS(CB_.is_singleton(), int);
	API_RETURNS(CB_.find_first_common(CB_), int);
	API_RETURNS(CB_.is_bit(0), bool);
	API_RETURNS(CB_.test(0), bool);
	API_RETURNS(CB_.is_empty(), bool);
	API_RETURNS(CB_.is_disjoint(CB_), bool);
	API_RETURNS(CB_ == CB_, bool);
	API_RETURNS(CB_ != CB_, bool);
	API_RETURNS(CB_.size(), std::size_t);
	API_RETURNS(CB_.bit_capacity(), std::size_t);
	API_RETURNS(CB_.extract(LV_), void);
	API_RETURNS(CB_.extract_set(std::declval<bit_index_set&>()), void);
	API_RETURNS(CB_.extract_stack(std::declval<utils::FixedStack<int>&>()), void);
	SUCCEED();
}

TYPED_TEST(BitsetApiTest, noexcept_queries) {
	using B = TypeParam;

	static_assert(noexcept(CB_.is_bit(0)), "is_bit");
	static_assert(noexcept(CB_.test(0)), "test");
	static_assert(noexcept(CB_.next_bit(0)), "next_bit");
	static_assert(noexcept(CB_.prev_bit(0)), "prev_bit");
	static_assert(noexcept(CB_.is_empty()), "is_empty");
	static_assert(noexcept(CB_.is_disjoint(CB_)), "is_disjoint");
	static_assert(noexcept(B_.erase_bit()), "erase_bit()");
	SUCCEED();
}

TYPED_TEST(BitsetApiTest, construction_and_conversion_traits) {
	using B = TypeParam;

	static_assert(std::is_default_constructible<B>::value, "default");
	static_assert(std::is_constructible<B, std::size_t>::value, "(nPop)");
	static_assert(std::is_constructible<B, std::size_t, std::initializer_list<int>>::value, "(nPop, {..})");
	static_assert(std::is_constructible<B, std::size_t, const std::vector<int>&>::value, "(nPop, bits)");
	static_assert(std::is_constructible<B, const std::vector<int>&>::value, "(bits)");
	static_assert(std::is_copy_constructible<B>::value && std::is_nothrow_move_constructible<B>::value, "copy/move");
	static_assert(std::is_copy_assignable<B>::value && std::is_nothrow_move_assignable<B>::value, "assignment");

	//explicit construction only: no implicit conversion from a number or a vector
	static_assert(!std::is_convertible<std::size_t, B>::value, "explicit (nPop)");
	static_assert(!std::is_convertible<std::vector<int>, B>::value, "explicit (bits)");

	//explicit conversion to a vector of bits, never implicit
	static_assert(std::is_constructible<std::vector<int>, const B&>::value, "explicit cast to vector");
	static_assert(!std::is_convertible<B, std::vector<int>>::value, "no implicit cast to vector");
	SUCCEED();
}

TYPED_TEST(BitsetApiTest, free_functions_by_argument_dependent_lookup) {
	using B = TypeParam;

	API_RETURNS(AND(CB_, CB_, B_), B&);
	API_RETURNS(AND(CB_, CB_), B);
	API_RETURNS(OR(CB_, CB_, B_), B&);
	API_RETURNS(erase_bit(CB_, CB_, B_), B&);
	SUCCEED();
}

//////////////////////////////////////////////
// Group 2: same behavior at run time

TYPED_TEST(BitsetApiTest, construction_forms) {
	using B = TypeParam;

	B e;
	EXPECT_TRUE(e.is_empty());
	EXPECT_EQ(0, e.count());

	B a(200);
	EXPECT_TRUE(a.is_empty());
	EXPECT_GE(a.bit_capacity(), 200u);

	B b(200, { 1, 70, 199 });
	EXPECT_EQ((std::vector<int>{ 1, 70, 199 }), bits_of(b));

	const std::vector<int> lv = { 199, 1, 70 };				//unsorted
	B c(200, lv);
	EXPECT_EQ(bits_of(b), bits_of(c));

	B d(lv);												//population from the maximum bit
	EXPECT_EQ(bits_of(b), bits_of(d));
	EXPECT_GE(d.bit_capacity(), 200u);

	B z((std::vector<int>()));								//empty list
	EXPECT_TRUE(z.is_empty());
}

TYPED_TEST(BitsetApiTest, copy_move_and_comparison) {
	using B = TypeParam;
	B a(200, { 1, 70, 199 });

	B copy(a);
	EXPECT_TRUE(copy == a);
	EXPECT_FALSE(copy != a);
	copy.set(2);
	EXPECT_TRUE(copy != a);
	EXPECT_EQ(3, a.count());								//the original is unchanged

	B moved(std::move(copy));
	EXPECT_EQ((std::vector<int>{ 1, 2, 70, 199 }), bits_of(moved));

	B assigned(200);
	assigned = a;
	EXPECT_TRUE(assigned == a);
	B move_assigned(200);
	move_assigned = std::move(assigned);
	EXPECT_TRUE(move_assigned == a);
}

TYPED_TEST(BitsetApiTest, reset_init_and_shrink) {
	using B = TypeParam;
	B a(200, { 1, 70 });

	a.reset(300);
	EXPECT_TRUE(a.is_empty());
	EXPECT_GE(a.bit_capacity(), 300u);

	a.reset(300, std::vector<int>{ 5, 299 });
	EXPECT_EQ((std::vector<int>{ 5, 299 }), bits_of(a));

	a.init(100);
	EXPECT_TRUE(a.is_empty());
	a.init(100, std::vector<int>{ 7, 99 });
	EXPECT_EQ((std::vector<int>{ 7, 99 }), bits_of(a));

	a.shrink_to_fit();
	EXPECT_EQ((std::vector<int>{ 7, 99 }), bits_of(a));
}

TYPED_TEST(BitsetApiTest, bit_ranges) {
	using B = TypeParam;
	B a(300);

	a.set_bit(60, 70);										//closed range, crosses a block boundary
	EXPECT_EQ(11, a.count());
	EXPECT_EQ(60, a.lsb());
	EXPECT_EQ(70, a.msb());

	a.set_bit(130, 130);									//single-bit range
	EXPECT_TRUE(a.is_bit(130));

	a.erase_bit(63, 65);
	EXPECT_FALSE(a.is_bit(63));
	EXPECT_FALSE(a.is_bit(64));
	EXPECT_FALSE(a.is_bit(65));
	EXPECT_TRUE(a.is_bit(62));
	EXPECT_TRUE(a.is_bit(66));
	EXPECT_EQ(9, a.count());								//62, 61, 60, 66..70 and 130

	a.set_bit(0, 299);										//whole population
	EXPECT_EQ(300, a.count());
	a.erase_bit(0, 299);
	EXPECT_TRUE(a.is_empty());
}

TYPED_TEST(BitsetApiTest, set_bit_from_list_and_bitset) {
	using B = TypeParam;
	B a(200);
	a.set_bit(std::vector<int>{ 150, 3, 70 }).set(3);		//duplicates and chaining are fine
	EXPECT_EQ((std::vector<int>{ 3, 70, 150 }), bits_of(a));

	B b(200, { 3, 100 });
	a.set_bit(b);											//union
	EXPECT_EQ((std::vector<int>{ 3, 70, 100, 150 }), bits_of(a));

	a.erase_bit(b);											//difference
	EXPECT_EQ((std::vector<int>{ 70, 150 }), bits_of(a));
}

TYPED_TEST(BitsetApiTest, block_ranges) {
	using B = TypeParam;
	B src(300, { 1, 70, 130, 200, 299 });					//blocks 0, 1, 2, 3, 4

	B a(300, { 5 });
	a.set_block(1, 3, src);									//ORs the blocks 1..3 of src
	EXPECT_EQ((std::vector<int>{ 5, 70, 130, 200 }), bits_of(a));

	a.erase_block(2, 3, src);								//removes the bits of src in the blocks 2..3
	EXPECT_EQ((std::vector<int>{ 5, 70 }), bits_of(a));
}

TYPED_TEST(BitsetApiTest, extraction_and_conversions) {
	using B = TypeParam;
	B a(300, { 0, 63, 64, 299 });

	std::vector<int> lv = { 9, 9 };							//reused output
	a.extract(lv);
	EXPECT_EQ((std::vector<int>{ 0, 63, 64, 299 }), lv);
	EXPECT_EQ(lv, static_cast<std::vector<int>>(a));

	utils::FixedStack<int> st(10);
	a.extract_stack(st);
	EXPECT_EQ(4, static_cast<int>(st.size()));

	B e(300);
	e.extract(lv);
	EXPECT_TRUE(lv.empty());
}

TYPED_TEST(BitsetApiTest, io_format) {
	using B = TypeParam;
	B a(100, { 3, 5 });
	B e(100);

	std::ostringstream os;
	a.print(os, true, false);
	EXPECT_EQ("[3 5 (2)]", os.str());

	std::ostringstream os2;
	a.print(os2, false, false);
	EXPECT_EQ("[3 5 ]", os2.str());

	std::ostringstream os3;
	e.print(os3, true, false);
	EXPECT_EQ("[]", os3.str());

	EXPECT_EQ("[3 5 (2)]", a.to_string());
}

