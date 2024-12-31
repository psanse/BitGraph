/*
* common_paths.h useful paths for different tests and targest and by different machines
* @creation_date  20/06/19
* @last_update    31/12/24
*/

#ifndef __COMMON_PATHS_H__
#define __COMMON_PATHS_H__

///////////////////
//
// TESTS GRAPH (in source code)
//
// uses CMake definition: _PATH_FOR_TESTS_DATA_IN_SRC_CODE
//////////////////
 
//cmake GUI
#define TESTS_GRAPH_DATA_CMAKE				PATH_GRAPH_TESTS_CMAKE_SRC_CODE
#define TESTS_COPT_DATA_CMAKE				PATH_COPT_TESTS_CMAKE_SRC_CODE


//ETSIDI
#define TEST_GRAPH_PATH_DIMACS_ETSIDI_I7	"C:/Users/i7/Desktop/dimacs/"
#define TEST_GRAPH_PATH_DIMACS_ETSIDI_I9	"C:/Users/pablo/Desktop/dimacs/"
#define TEST_GRAPH_PATH_TEST_ETSIDI_I7		"C:/Users/i7/Desktop/test/"
#define TEST_GRAPH_PATH_TEST_ETSIDI_I9		"C:/Users/pablo/Desktop/test/"
#define TEST_GRAPH_PATH_LOG_ETSIDI_I7		"C:/Users/i7/Desktop/log/"
#define TEST_GRAPH_PATH_LOG_ETSIDI_I9		"C:/Users/pablo/Desktop/log/"
#define TEST_GRAPH_DESKTOP_ETSIDI_I7		"C:/Users/i7/Desktop/"
#define TEST_GRAPH_DESKTOP_ETSIDI_I9		"C:/Users/pablo/Desktop/"
#define TEST_GRAPH_PATH_DATA_ETSIDI_I9		"C:/Users/pablo/Desktop/copt/graph/data/"

//LAPTOP
#define TEST_GRAPH_PATH_DIMACS_LAPTOP_19	"C:/Users/pablo/Desktop/dimacs/"
//#define TEST_GRAPH_PATH_DIMACS_LAPTOP_17	"C:/Users/pablo/Desktop/dimacs/"
#define TEST_GRAPH_PATH_LOG_LAPTOP_19		"C:/Users/pablo/Desktop/log/"
#define TEST_GRAPH_PATH_DATA_LAPTOP_19		"C:/Users/pablo/Desktop/copt/graph/data/";

#define TEST_GRAPH_PATH_DIMACS_LAPTOP_17	"C:/Users/pablo/Desktop/dimacs/"
//#define TEST_GRAPH_PATH_DIMACS_LAPTOP_17	"C:/Users/pablo/Desktop/dimacs/"
#define TEST_GRAPH_PATH_LOG_LAPTOP_17		"C:/Users/pablo/Desktop/log/"
#define TEST_GRAPH_PATH_DATA_LAPTOP_17		"C:/Users/pablo/Desktop/copt_bash/copt/graph/data/";

////////////////////
//TESTS CLIQUE 

//ETSIDIcopt_bash
#define TEST_CLIQUE_PATH_DATA_ETSIDI_I9		"C:/Users/pablo/Desktop/copt/copt/data/"
#define TEST_CLIQUE_PATH_DIMACS_ETSIDI_I9	"C:/Users/pablo/Desktop/dimacs/"

//LAPTOP
#define TEST_CLIQUE_PATH_DIMACS_LAPTOP_19	"C:/Users/pablo/Desktop/dimacs/"
#define TEST_CLIQUE_PATH_DIMACS_LAPTOP_17	"C:/Users/pablo/Desktop/dimacs/"
#define TEST_CLIQUE_PATH_DATA_LAPTOP_19		"C:/Users/pablo/Desktop/copt/copt/data/"
#define TEST_CLIQUE_PATH_DATA_LAPTOP_17		"C:/Users/pablo/Desktop/copt_bash/copt/copt/data/"

#define TEST_CLIQUE_PATH_CSP_LAPTOP_19		"C:/Users/pablo/Desktop/csp/"

//general parameters
#define TEST_GRAPH_PATH_DATA	TEST_GRAPH_PATH_DATA_LAPTOP_19
#define TEST_CLIQUE_PATH_DATA	TEST_CLIQUE_PATH_DATA_LAPTOP_19
#define TEST_GRAPH_PATH_LOG		TEST_GRAPH_PATH_LOG_LAPTOP_19

#endif