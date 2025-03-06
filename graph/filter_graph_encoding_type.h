//filter.h : limits Graph<T> metaclass types  to bitarrays and sparse bitarrays
//i.e. a Graph cannot be constructed on watched_bitstrings

#ifndef _FILER_GRAPH_TYPE_ERROR_
#define _FILER_GRAPH_TYPE_ERROR_

//#include "bitscan/bitscan.h"
//
//#include "bitscan/bbscan.h"
//#include "bitscan/bbscan_sparse.h"
class BBScan;
class BBScanSp;

template <class T>
struct filterGraphTypeError;		//error at compile time with invalid specializations and a nice name

//valid specializations
template <>
struct filterGraphTypeError<BBScan>{};
template <>
struct filterGraphTypeError<BBScanSp>{};


#endif