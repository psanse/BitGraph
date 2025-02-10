/**
 * @file bbscanning.h
 * @brief Attempt to refactor bitscanning operations using generic programming
 * @details created 29/01/2025
 * author pss
 **/

#ifndef __BBSCANNING_H__
#define __BBSCANNING_H__

//#include "bitset.h"
#include "bbintrinsic.h"
#include "bitscan/bbscanningFeature.h"

template< class ScanPolicy>
class BBScanning {

	friend ScanPolicy;

public:

	BBScanning(typename ScanPolicy::basic_type& bb):
		scp_(bb)
	{
		scp_.init();
	}

	int next_bit() {
		return scp_();	
	}

private:			
	ScanPolicy scp_;					//scanning policy	
};	


/////////////////////////////////

template<class BitSet_t>
struct NonDectructiveScan {
	using basic_type = BitSet_t;	

	int operator()() {
		return bb_.next_bit();
	}

	int init() {
		return bb_.init_scan(BBObject::NON_DESTRUCTIVE);
	}
	NonDectructiveScan(BitSet_t& bb) : bb_(bb) {}

	//state
	BitSet_t& bb_;
};

/////////////////////////////////


template<class BitSet_t>
struct NonDectructiveReverseScan {
	using basic_type = BitSet_t;

	int operator()() {
		return bb_.prev_bit();
	}

	int init() {
		return bb_.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	}
	NonDectructiveReverseScan(BitSet_t& bb) : bb_(bb) {}

	//state
	BitSet_t& bb_;
};

/////////////////////////////////

template<class BitSet_t>
struct DectructiveScan {
	using basic_type = BitSet_t;

	int operator()() {
		return bb_.next_bit_del();
	}

	int init() {
		return bb_.init_scan(BBObject::DESTRUCTIVE);
	}
	DectructiveScan(BitSet_t& bb) : bb_(bb) {}

	//state
	BitSet_t& bb_;
};

/////////////////////////////////

template<class BitSet_t>
struct DectructiveReverseScan {
	using basic_type = BitSet_t;

	int operator()() {
		return bb_.next_bit_del();
	}

	int init() {
		return bb_.init_scan(BBObject::DESTRUCTIVE_REVERSE);
	}
	DectructiveReverseScan(BitSet_t& bb) : bb_(bb) {}

	//state
	BitSet_t& bb_;
};

//adapters
template<class BitSet_t = BBIntrin>
using SCAN = BBScanning<DectructiveScan<BBIntrin>>;
template<class BitSet_t = BBIntrin>
using SCAN_REV = BBScanning<DectructiveReverseScan<BitSet_t>>;
template<class BitSet_t = BBIntrin>
using SCAN_DEST = BBScanning<DectructiveScan<BitSet_t>>;
template<class BitSet_t = BBIntrin>
using SCAN_DEST_REV = BBScanning<DectructiveScan<BitSet_t>>;

#endif








