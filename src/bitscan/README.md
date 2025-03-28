BITSCAN  - DOCUMENTATION UNDER DEVELOPMENT
===================
BITSCAN is a C++ library that manipulates bit arrays (alias bit strings, bit sets, etc.) and is specifically oriented toward fast bitscanning operations. BITSCAN results from **more than 20 years of research** in combinatorial optimization problems and has been proven very useful when modelling sets. In this application, a bitset represents the set population, with each member encoded as a single 1-bit.

BITSCAN has been tested in LINUX and Windows OS.  

The library currently offers a variety of bitset types, the main ones being:

- `bitblock`: Wrapper for bit manipulation of 64-bit numbers (*lsb*, *msb*, *popcount* etc.).
- `bitarray`: Main type for standard bit manipulation. 
- `sparse_bitarray`: Main type for large and massive sets with few elements.
  
Other types such as `simple_bitarray`, `simple_sparse_bitarray` and `watched_bitarray` are included for specific operations. For example, the `watched_bitarray` type is intended for set populations with low density, but not sparse. Empty bit blocks are still stored in full, but two pointers (aka sentinels) which point (alias *watch*) the highest and lowest empty blocks respectively, determine the range of useful bitmasks (see [here](http://download.springer.com/static/pdf/797/chp%253A10.1007%252F978-3-319-09584-4_12.pdf?auth66=1411550130_ba322f209d8b171722fa67741d3f77e9&ext=.pdf "watched bit arrays") for an interesting application).

The library features several optimizations for typical enumeration of set members (in this context, *bitscanning*) which have been found useful for fast implementation of algorithms during research, such as  destructive vs non-destructive scans, forward and reverse_forward scans, random starting points, caching indexes etc. 

GETTING STARTED
-------------------------------
To include the BITSCAN library in your project, add the dependency to the *bitscan.h file*. In the following example, a bitset of 100 elements is declared and the 11th bit is set to 1 (index numbering starts at 0). 

    #include "bitscan/bitscan.h"
    	//...
	int main(){
   	  bitarray bbi(100);
  	  bbi.set_bit(10);
   	  std::cout << bbi;
  	}    
You will find many more examples in the *tests* folder.  

EXAMPLES OF BITSCANNING OPERATIONS
-------------------------------

The BITSCAN library features specific optimisation techniques for *destructive* and *non-destructive* scans. In a destructive bit scan (DBS), whenever a 1-bit is read *it is removed from the bitset* (at the end of the process the bit string is empty). In the (conventional) non-destructive case (NDBS), the bitset remains unaltered. 

Here is an example of fast bit scan loop syntax in BITSCAN:

    bitarray bba(100);
    	//...
	bba.init_scan(bbo::NON_DESTRUCTIVE);
   	int nBit = BBObject::noBit;;
  	 while( (nBit=bba.next_bit()) != BBObject::noBit ){
   	     //Do something with nBit...
  	}   
The *init_scan* member function configures cache information appropriately. Currently four types of scans are supported: DESTRUCTIVE, NON\_DESTRUCTIVE and the reverse counterparts (DESTRUCTIVE\_REVERSE and NON\_DESTRUCTIVE\_REVERSE). For each type there is an appropriate family of functions. Here is another example, this time for a sparse bitset of 100000 bits:


    sparse_bitarray bba(100000);
    	//...

    if(bba.init_scan(bbo::DESTRUCTIVE)!= -1){
	   int nBit = BBObject::nobit;
   	   while( (nBit=bbi.next_bit_del()) != BBObject::noBit ){
		//Do something with nBit...
	   }
	}
In this case the *if* clause is necessary because sparse bitsets can have empty semantics (no memory allocation), which is checked in the init_scan function. Moreover, the scanning now differs from the previous case because it deletes each population member from the set once it is found (DESTRUCTIVE type). Finally, it is worth noting that scanning operations from different starting points in the bitset are also supported.


CONFIGURATION PARAMETERS
-------------------------

The file *bbconfig.h* contains customization parameters. An important parameter is:

1. POPCOUNT_64: Disable in processor architectures which do not support hardware population count assembly instructions. 


