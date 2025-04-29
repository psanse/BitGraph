BITSCAN
===================
BITSCAN is a C++ library designed to efficiently manipulate bit arrays—also called bitstrings or bitsets—with a particular emphasis on high-performance bitscanning operations. The library is the outcome of over **two decades of research in combinatorial optimization** and is very efficient when working with sets. In this context, a bitset encodes a set of elements, and each element is represented by a single bit set to 1.

Main Features
-------------------------------
BITSCAN features several low-level optimizations for bitscanning—the enumeration of set bits—that have proven effective in implementing high-performance combinatorial optimization algorithms. Among other features, it supports destructive and non-destructive scans, forward and reverse scans, random starting points and caching of the last scanned bit. It also includes many useful operations for 64-element bitsets (encoded by a single number).

Getting started
-------------------------------
The library currently offers a variety of types for handling bitsets, the most relevant ones being:

- `bitarray`: main type for standard bitset manipulation. 
- `sparse_bitarray`: main type for large and massive sets with few elements.

BITSCAN also offers a `bblock` namespace with many useful operations for 64-bit numbers.

To include the BITSCAN library in your project, add the header `#include "BitGraph/bitscan/bitscan.h"`. In the following example, a bitset of 100 elements is declared, containing only the 11th element (index numbering starts at 0). 

``` plaintext
#include <iostream>
#include "BitGraph/bitscan/bitscan.h"

int main(){
   bitarray bb(100);
   bb.set_bit(10);
   bb.set_bit(20);	

   std::cout << bb;	//screen output [10, 20 (2)]
 }
```   
Many more examples are provided in the *[tests](https://github.com/psanse/BitGraph/tree/master/src/bitscan/tests)* folder, and in the [install](https://github.com/psanse/BitGraph/tree/master/install/src) folder. 

Efficient bitscanning
-------------------------------

The BITSCAN library features specific optimisation techniques for *destructive* and *non-destructive* scans. In a destructive bitscan, each 1-bit is removed from the bitset as it is read, resulting in an empty bitset at the end of the process. In contrast, during a (conventional) non-destructive bitscan, the bitset remains unchanged. Other features include reverse bitscanning, initial, configurable starting points, specific scanning objects with simplified syntax and others.  

Here is an example of the bitscan loop syntax (non-destructive type):

``` plaintext
bitarray bb(100);
bb.set_bit(10);
bb.set_bit(20);

//...

bb.init_scan(bbo::NON_DESTRUCTIVE);
int nBit = bbo::noBit;
while( (nBit = bb.next_bit()) != bbo::noBit ){
     //Do something with nBit...
}
```
And here is the equivalent, less verbose syntax, using one of the four specialised scanning objects:

``` plaintext
//...
bbo::Scan<bitarray> sc(bb);
int nBit = bbo::noBit;
while ( (nBit = sc.next_bit()) != bbo::noBit) {
  //Do something with nBit...
}
```
Specialised types for sparse bitsets are also available.

``` plaintext
sparse_bitarray bbsp(1000000);
bbsp.set_bit(0);
bbsp.set_bit(999999);

bbo::Scan<sparse_bitarray> scs(bbsp);
int nBit = bbo::noBit;
while ((nBit = scs.next_bit()) != bbo::noBit) {
	cout << "sparse bit scanned:" << nBit << endl;
}

Screen output: [0, 999999 (2)]
```  
See the unit tests related to sparse bitsets for additional examples [here](https://github.com/psanse/BitGraph/blob/master/src/bitscan/tests/test_bitset_sparse.cpp).
