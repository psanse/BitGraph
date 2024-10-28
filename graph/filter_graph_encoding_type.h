//filter.h : limits Graph<T> metaclass types  to bitarrays and sparse bitarrays
//i.e. a Graph cannot be constructed on watched_bitstrings

#ifndef _FILER_GRAPH_ENCODING_
#define _FILER_GRAPH_ENCODING_

template <class T>
struct filterGraphEncodingType;		//error at compile time

//valid specializations
template <>
struct filterGraphEncodingType<bitarray>{};
template <>
struct filterGraphEncodingType<sparse_bitarray>{};


#endif