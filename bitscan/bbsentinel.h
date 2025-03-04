 /**
  * @file bbsentinel.h file
  * @brief header file of the BBSentinel class.
  *		   Manages efficient bitset operations by circumscribing them to the range [low_sentinel, high_sentinel] 
  * @details The lower sentinel is the first non-zero bitblock in the bitstring
  * @detials The higher sentinel is the last non-zero bitblock in the bitstring
  * @details created?,  last_updated 13/02/2025
  * @details used to enhance a clique algorithm (results published in [XXX))
  * 
  * @author pss
  *
  * TODO- PENDING FULL REFACTORING (13/02/2025)
  **/

#ifndef __BB_SENTINEL_H__
#define __BB_SENTINEL_H__

#include "bitscan/bbscan.h"
#include "bitscan/bbalgorithm.h"			//MIN, MAX

 using namespace std;

class BBSentinel : public BBScan{
	friend BBSentinel&  AND	(const BitSet& lhs, const BBSentinel& rhs,  BBSentinel& res);		//updates sentinels

	public:
	BBSentinel():m_BBH(EMPTY_ELEM), m_BBL(EMPTY_ELEM){init_sentinels(false);}
explicit BBSentinel(int popsize): BBScan(popsize){ init_sentinels(false);}
	BBSentinel(const BBSentinel& bbN) : BBScan(bbN){ m_BBH=bbN.m_BBH; m_BBL=bbN.m_BBL;}
	~BBSentinel(){};

////////////
// setters / getters
	void set_sentinel_H(unsigned int i){ m_BBH=i;}
	void set_sentinel_L(unsigned int i){ m_BBL=i;}
	void set_sentinels(int low, int high);
	void init_sentinels(bool update=false);								//sets sentinels to maximum scope of current bit string
	void clear_sentinels();												//sentinels to EMPTY
	int get_sentinel_L(){ return m_BBL;}
	int get_sentinel_H(){ return m_BBH;}
/////////////
// basic sentinel

	int  update_sentinels			();
	int  update_sentinels			(int bbl, int bbh);
	int  update_sentinels_high		();
	int  update_sentinels_low		();

	void update_sentinels_to_v		(int v);	

//////////////
// basic overwritten operations (could be extended)
	
	//erase: will not update sentinels	
virtual	void  erase_bit				();											//in sentinel range
virtual	void  erase_bit				(int nBit) {BitSet::erase_bit(nBit);}	//required because of the cast-to-int construction of sentinels (1)
	void  erase_bit_and_update		(int nBit);									//erases and updates sentinels			
	BBSentinel& erase_bit			(const BitSet&);							//(1): required for SEQ coloring
	
virtual	bool is_empty				()const;
virtual	bool is_empty				(int nBBL, int nBBH) const;					//is empty in range

#ifdef POPCOUNT_INTRINSIC_64
	int popcn64						() const;
#endif

////////////////
// operators
	BBSentinel& operator=		(const BBSentinel&);
	BBSentinel& operator&=		(const BitSet&);

//////////////
// I/O
	
	std::ostream& print(ostream& o=cout, bool show_pc = true, bool endl = true) const override;

/////////////////
// bit scanning operations 

	int init_scan(scan_types sct) override;

 inline int prev_bit_del() override;							//**TODO- empty bitstring
 inline	int next_bit_del () override;
 inline	int next_bit_del (BBSentinel& bbN_del) ;				//Does not override! CHECK (12/02/2025)
	
 inline	int next_bit() override;

 //inline	int next_bit(int& nBB) override;

 //////////////////////////////////
 //data members - sentinel information
protected:	
	 int m_BBH;										//explicit storage for sentinel high index
	 int m_BBL;										//explicit storage for sentinel low index
};



#ifdef POPCOUNT_INTRINSIC_64
inline int BBSentinel::popcn64() const{
	BITBOARD pc=0;
	for(int i=m_BBL; i<=m_BBH; i++){
		pc+=__popcnt64(vBB_[i]);
	}
return pc;
}

#endif

//specializes the only bitscan function used

int BBSentinel::prev_bit_del(){
//////////////
// BitScan reverse order and distructive
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_BBH; i>=m_BBL; i--){
		if(_BitScanReverse64(&posInBB,vBB_[i])){
			m_BBH=i;
			vBB_[i]&=~Tables::mask[posInBB];			//erase before the return
			return (posInBB+WMUL(i));
		}
	}
return EMPTY_ELEM;  
}


int BBSentinel::next_bit_del (){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_BBL; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB,vBB_[i]) ){
			m_BBL=i;
			vBB_[i]&=~Tables::mask[posInBB];					//erase before the return
			return (posInBB+ WMUL(i));
		}
	}
return EMPTY_ELEM;  
}


int BBSentinel::next_bit_del (BBSentinel& bbN_del){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop (experimental, does not use sentinesl of bbN_del)

	unsigned long posInBB;

	for(int i=m_BBL; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB, vBB_[i]) ){
			m_BBL=i;
			vBB_[i]&=~Tables::mask[posInBB];					//erase before the return
			bbN_del.vBB_[i]&=~Tables::mask[posInBB];
			return (posInBB+ WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}



int BBSentinel::next_bit(){
////////////////////////////
// last update:31/12/2013
// BitScan non destructive
//
// COMMENTS
// 1- update sentinels, set scan_.bbi_ to m_BBL and set scan_.pos_ to MASK_LIM at the start of loop

	unsigned long posInBB;
				
	if(_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])){
		scan_.pos_ =posInBB;
		return (posInBB + WMUL(scan_.bbi_));
	}else{													
		for(int i=scan_.bbi_+1; i<=m_BBH; i++){
			if(_BitScanForward64(&posInBB,vBB_[i])){
				scan_.bbi_=i;
				scan_.pos_=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}

//inline
//int BBSentinel::next_bit(int& nBB){
//////////////////////////////
//// last update:31/12/2013
//// BitScan non destructive
////
//// COMMENTS
//// 1- update sentinels, set scan_.bbi_ to m_BBL and set scan_.pos_ to MASK_LIM at the start of loop
//
//	unsigned long posInBB;
//			
//	//look uo in the last table
//	if(_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])){
//		scan_.pos_ =posInBB;
//		nBB=scan_.bbi_;
//		return (posInBB + WMUL(scan_.bbi_));
//	}else{											//not found in the last table. look up in the rest
//		for(int i=(scan_.bbi_+1); i<=m_BBH; i++){
//			if(_BitScanForward64(&posInBB,vBB_[i])){
//				scan_.bbi_=i;
//				scan_.pos_=posInBB;
//				nBB=i;
//				return (posInBB+ WMUL(i));
//			}
//		}
//	}
//return EMPTY_ELEM;
//}

#endif 





