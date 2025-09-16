/**
* @file graph_gen.h
* @brief header for Erdos-Renyi sparse and non sparse bitstring unidrected graph generation 
* @author pss
* @details: created?, last_update 07/03/25
* 
* TODO - simplify class architecture for graph generation (07/03/25)
**/

#ifndef __GRAPH_GEN_H__
#define __GRAPH_GEN_H__

#include <iostream>
#include <sstream>
#include <random>								
#include <cassert>

#include "utils/common.h"
#include "graph/simple_ugraph.h"
#include "graph/simple_graph_w.h"					// must be after simple_ugraph include
#include "graph/simple_graph_ew.h"					// must be after simple_ugraph include
#include "graph_fast_sort.h"

//useful aliases
namespace bitgraph {
	
	using graph = bitgraph::Graph<bitarray>;						//simple graph
	using ugraph = bitgraph::Ugraph<bitarray>;						//simple undirected graph
	using sparse_graph = bitgraph::Graph<sparse_bitarray>;			//simple sparse graph
	using sparse_ugraph = bitgraph::Ugraph<sparse_bitarray>;		//simple sparse undirected graph
	using ugraph_w = bitgraph::Graph_W<ugraph, double>;
	using ugraph_wi = bitgraph::Graph_W<ugraph, int>;
	using ugraph_ew = bitgraph::Graph_EW<ugraph, double>;
	using ugraph_ewi = bitgraph::Graph_EW<ugraph, int>;
}


namespace bitgraph {

	namespace _impl {

		/////////////////
		//
		// struct random_attr_t
		// (data for random graph generation)
		//
		//////////////////

		struct random_attr_t {
			friend std::ostream& operator << (std::ostream& o, const random_attr_t& r) {
				o << "[" << r.nLB << " " << r.nUB << " "
					<< r.pLB << " " << r.pUB << " " << r.nRep << " "
					<< r.incN << " " << r.incP << "]" << endl;
				return o;
			}

			//constructor
			random_attr_t(std::size_t nLB, std::size_t  nUB, double pLB, double pUB, std::size_t  nRep, std::size_t  incN, double incP) {
				set(nLB, nUB, pLB, pUB, nRep, incN, incP);
				assert(nLB <= nUB && pLB <= pUB && incP > 0 && "bad parameters - random_attr_t");
			}

			//setter
			void set(std::size_t  nLB, std::size_t  nUB, double pLB, double pUB, std::size_t  nRep, std::size_t  incN, double incP) {
				this->nLB = nLB; this->nUB = nUB;
				this->pLB = pLB; this->pUB = pUB;
				this->nRep = nRep;
				this->incN = incN; this->incP = incP;
			}

			//data
			std::size_t  nLB;					//range of vertices [nLB, nUB]
			std::size_t  nUB;
			double pLB;							//range of densities [pLB, pUB]	
			double pUB;
			std::size_t  nRep;					//number of repetitions
			std::size_t  incN;					//increment of vertices
			double incP;						//increment of densities
		};
	}//end of namespace _impl

	using _impl::random_attr_t;					//alias for random_attr_t

}//end of namespace bitgraph	

namespace bitgraph {
	namespace _impl {

		/////////////////
		//
		// class RandomGen
		// (random generation of generic non-sparse and unweighted graphs)
		//
		//////////////////

		template<class Graph_t>
		class RandomGen {
		public:

			/*
			* @brief generates uniform simple random graph (n, p)
			*
			*			I. no self-loops
			*
			*			II. name r<n>_<p>.txt
			*
			* @param g output graph
			* @param n number of vertices
			* @param p probability of edge creation
			* @returns 0 if OK, -1 if ERROR
			*/
			static int create_graph(Graph_t& g, std::size_t n, double p);

			/*
			* @brief creates a set uniform random (n, p) directed/undirected graphs
			*
			*			I. no self-loops
			*
			*			II. names  r<n_<p>.txt
			*
			* @param path input directory where the files are created
			* @param rd input data to generate the benchmark
			*
			* @returns 0 if OK, -1 if ERROR
			*/
			static int create_graph_benchmark(const std::string& path, const random_attr_t& rd);

			/**
			* @brief creates an isomorphi grap g_iso by shuffling the vertices of the input graph g
			*		 randomly (uses std::shuffle)
			* @param g: input graph
			* @param g_iso: output isomorphic graph to g
			* @returns 0 if successful, -1 otherwise
			* @details: TODO - Unit test (14/04/2025)
			**/
			static int create_isomorphism(Graph_t& g_iso, Graph_t& g);
		};
	}//end of namespace _impl	

	using _impl::RandomGen;		//alias for RandomGen<Graph_t>

}//end of namespace bitgraph	

namespace bitgraph {
	namespace _impl {
		/////////////////
		//
		// class SparseRandomGen
		// (random generation of generic sparse unweighted graphs)
		// 
		// Currently only for undirected graphs
		//
		//////////////////
		template<class Graph_t, int logSizeTable = 35000 >
		class SparseRandomGen {};

		/////////////////
		//
		// Specialization of SparseRandomGen for sparse_ugraph
		// (massive sparse uniform graph generator)
		// 
		//////////////////

		template< int logSizeTable >
		class SparseRandomGen <sparse_ugraph, logSizeTable>
		{
			/*
			*  @brief decodes undimensional edge index to upper triangle of adjacency matrix
			*
			*		  (see [Fast Random graph generation, 2011], personal comm. with authors)
			*/
			static void decode(BITBOARD edge_index, unsigned int& i, unsigned int& j, unsigned int n);

		public:

			//constructor
			SparseRandomGen() { init_log_table(); }

			//driver
			int create_ugraph(sparse_ugraph& g, std::size_t n, double p);

		private:
			/*
			* @brief 16-bit table for precomputed logarithms
			*/
			void init_log_table();


			///////////////////////
			// data
		private:

			double log_[logSizeTable];
		};
	}//end of namespace _impl

	using _impl::SparseRandomGen;		//alias for SparseRandomGen<Graph_t, logSizeTable>
}//end of namespace bitgraph



namespace bitgraph {
	namespace _impl {
		/////////////////
		//
		// Class WeightGen
		// (generates weights for vertex-weighted graphs)
		// 
		// TODO specialize for sparse-graphs
		// TODO SFINAE to control template graph type deduction (07/03/2025)
		// 
		// TODO - in general revise the whole logic of the generator classes for different graph types (07/03/2025)
		//
		////////////////

		template <class Graph_t>
		class WeightGen
		{
		public:
			enum type_t { WMOD = 0, WDEG };

			/*
			* @brief Overrites new vertex weights in the graph
			*
			*		I. we(v) = 1 + (v % mod) [Pullham 2008]
			*
			*		II. Optionally the weights are written to a file
			*
			*		TODO: specialize for sparse-graphs
			*
			* @returns -1 if error, 0 if OK
			*/
			static int create_weights(Graph_t& g, type_t, int wmod = DEFAULT_WEIGHT_MODULUS,
				std::string FILE_EXTENSION = "", std::string FILE_PATH = "");
		};
	}//end of namespace _impl

	using _impl::WeightGen;		//alias for WeightGen<Graph_t>
}//end of namespace bitgraph


namespace bitgraph {
	namespace _impl {
		/////////////////
		//
		// Class EdgeWeightGen
		// (generates weights for edge-weighted graphs)
		//
		// TODO refactor to the new type (22/01/19)
		// TODO specialize for sparse-graphs (08/01/2025)
		// TODO add more weight generation modes (08/01/2025)
		// TODO SFINAE to control template graph type deduction (07/03/2025)
		//
		////////////////


		template<class Graph_t>
		class EdgeWeightGen
		{
		public:
			enum type_t { WMOD = 0 };

			/*
			* @brief Generates weights for existing edges in a graph
			*
			*		I. we(v, w) = 1 + ((v + w) % mod) [Pullham 2008]
			*
			*		II. we(v, v) and non-edge weights are set to NOWT
			*
			*		III. Optionally the weights are written to a file
			*
			*		TODO: specialize for sparse-graphs
			*
			* @returns -1 if error, 0 if OK
			*/
			static int create_weights(Graph_t& g, type_t, int wmod = DEFAULT_WEIGHT_MODULUS,
				std::string FILE_EXTENSION = "", std::string FILE_PATH = "");

		};
	}//end of namespace _impl

	using _impl::EdgeWeightGen;		//alias for EdgeWeightGen<Graph_t>
}//end of namespace bitgraph

	////////////////////////////////////////////////////////////////////////
	// Necessary implementations of generic code in header file

namespace bitgraph{	

	template<class Graph_t>
	int RandomGen<Graph_t>::create_graph(Graph_t& g, std::size_t n, double p) {

		//assert - TODO DEBUG MODE
		if (n <= 0) {
			LOG_ERROR("bad number of vertices - RandomGen<Graph_t>::create_graph");
			return -1;
		}

		if (g.reset(n) == -1) {
			LOG_ERROR("error during allocation - RandomGen<Graph_t>::create_graph");
			return -1;
		}

		//////////////////
		g.gen_random_edges(p);		//generate edges appropiately (undirected / directed)
		//////////////////


		//name - r<n>_<p>.txt
		std::stringstream sstr;
		sstr << "r" << n << "_" << p << ".txt";
		g.name(sstr.str());

		return 0;
	}

	template<class Graph_t>
	int RandomGen<Graph_t>::create_graph_benchmark(const std::string& path, const random_attr_t& rd) {

		std::ostringstream o;
		Graph_t g;
		std::ofstream f;

		//add slash '/' at the end to the path if required
		std::string mypath(path);
		_dir::append_slash(mypath);

		for (int i = rd.nLB; i <= rd.nUB; i += rd.incN) {
			for (double j = rd.pLB; j <= rd.pUB; j += rd.incP) {
				for (int k = 0; k < rd.nRep; k++) {

					//////////////////////////
					create_graph(g, i, j);
					//////////////////////////

					o.str("");
					o << mypath.c_str() << "r" << i << "_" << j << "_" << k << ".txt";
					f.open(o.str().c_str());
					if (!f) {
						LOGG_ERROR("error in file name: ", o.str(), "- RandomGen<Graph_t>::create_graph_benchmark");
						std::exit(-1);
					}
					g.write_dimacs(f);
					f.close();
				}
			}
		}
		return 0;		//OK
	}

	template<class Graph_t>
	inline
		int RandomGen<Graph_t>::create_isomorphism(Graph_t& g_iso, Graph_t& g_ori)
	{
		vector<int> map(g_ori.size(), 0);
		std::iota(map.begin(), map.end(), 0);
		std::shuffle(map.begin(),
			map.end(),
			std::default_random_engine(_rand::RandomUniformGen<>::FIXED_RANDOM_SEED));

		//creates the isomorphism according to the mapping
		try {
			GraphFastRootSort<ugraph> gs(g_ori);
			gs.reorder(map, g_iso);
		}
		catch (std::exception& e) {
			LOG_ERROR("error during isomorphism generation - RandomGen<Graph_t>::create_isomorphism");
			LOG_ERROR(e.what());
			return -1;
		}

		return 0;
	}

	template<int logSizeTable>
	inline
		void SparseRandomGen<sparse_ugraph, logSizeTable>::decode(BITBOARD edge_index, unsigned int& i, unsigned int& j, unsigned int n) {
		/////////////////////////////
		// decodes undimensional edge index to upper triangle of adjacency matrix 
		//
		// REMARKS: Personal communication with [Fast Random graph generation, 2011] authors

		double a = sqrt((pow((2 * n) - 1.0, 2) - 8 * (edge_index + 1)));
		BITBOARD r = ceil((((2 * n) - 1.0 - a) / 2 - 1.0));
		BITBOARD c = edge_index + 1 + r - (r * (2 * n - r - 1) / 2);			//type is important else c overflows unsigned int type for very large graphs

		//cast back to unsigned int
		i = (unsigned int)r;
		j = (unsigned int)c;

	};

	template<int logSizeTable>
	void SparseRandomGen<sparse_ugraph, logSizeTable>::init_log_table() {

		for (int i = 0; i < logSizeTable; i++) {
			SparseRandomGen::log_[i] = log(i / (double)logSizeTable);
		}
	}

	template<int logSizeTable>
	int SparseRandomGen<sparse_ugraph, logSizeTable>::create_ugraph(sparse_ugraph& g, std::size_t n, double p) {

		BITBOARD m = ((BITBOARD)n * (n - 1) / 2);			//max num edges
		BITBOARD e = 0;								//**check
		double logp = log(1 - p);
		BITBOARD skip;
		unsigned int i = 0, j = 0;
		int num = 0;									//optional index which counts edges added to the graph

		default_random_engine generator;
		uniform_int_distribution<int> distribution(0, logSizeTable - 1);
		generator.seed((unsigned)time(nullptr));

		g.reset(n);
		do {
			int l = ceil(log_[distribution(generator)] / logp) - 1;
			(l > 0) ? skip = l : skip = 0;
			if ((e += ++skip) >= m)
				break;

			decode(e, i, j, n);					//perfectly valid option and similar in O
			/*while(skip>0){
			if( (m_n-1-j)>=skip ){
			j+=skip;
			break;
			}else{
			i++;
			skip-=(m_n-1-j);
			j=i;
			}
			}*/

			num++;
			g.add_edge(i, j);					//O(log) because it is a sparse graph

		} while (e < m);

		//opional
		/*stringstream sstr("");
		sstr<<m<<":"<<"["<<num<<"]"<<std::endl;
		cout<<sstr.str();*/

		return 0;
	}

	template<class Graph_t>
	inline
		int WeightGen<Graph_t>::create_weights(Graph_t& g, type_t type, int wmod, std::string FILE_EXTENSION, std::string FILE_PATH) {

		const int NV = g.graph().number_of_vertices();

		//no need to clear the current weights since they are overwritten

		switch (type) {
		case WDEG:
			for (std::size_t v = 0; v < NV; ++v) {
				g.set_weight(v, g.graph().degree(v));
			}
			break;
		case WMOD:
			for (std::size_t v = 0; v < NV; ++v) {
				g.set_weight(v, (1 + ((v + 1) % wmod)));
			}
			break;
		default:
			LOG_INFO("incorrect weight generation mode - WeightGen<Graph_t>::create_weights");
			return -1;
		}

		/////////////////////////////
		//streams weights to file
		if (!FILE_EXTENSION.empty() && !FILE_PATH.empty()) {

			//determines the filename
			std::string path = FILE_PATH;
			_dir::append_slash(path);
			std::string filename = path + g.graph().name() + FILE_EXTENSION;

			//streams weights to file
			std::ofstream f(filename);
			if (!f) {
				LOGG_INFO("WeightGen<Graph_t>::create_weights () - cannot write weights to file ", filename);
				return -1;
			}

			///////////////////////////
			g.print_weights(f, false);			//no vertex index is streamed with the weight value
			///////////////////////////

			f.close();
		}

		return 0;
	}

	template<class Graph_t>
	inline
		int EdgeWeightGen<Graph_t>::create_weights(Graph_t& g, type_t type, int wmod, std::string FILE_EXTENSION, string FILE_PATH) {

		switch (type) {
		case WMOD:
			g.set_modulus_edge_weight(wmod);
			break;
		default:
			LOG_INFO("bad weight generation mode - EdgeWeightGen<Graph_t>::create_weights");
			LOG_INFO("original weights unchanged");
			return -1;
		}

		/////////////////////////////
		//streams weights to file
		if (!FILE_EXTENSION.empty() && !FILE_PATH.empty()) {

			//determines the filename
			std::string path = FILE_PATH;
			_dir::append_slash(path);
			std::string filename = path + g.graph().name() + FILE_EXTENSION;

			//streams weights to file
			std::ofstream f(filename);
			if (!f) {
				LOGG_INFO("cannot write edge-weights to file ", filename, " - EdgeWeightGen<Graph_t>::create_weights");
				return -1;
			}

			///////////////////////////
			g.print_weights(f, false);			//NOWT weight values are not streamed - vertex index is not streamed with the weight value
			///////////////////////////

			f.close();
		}

		return 0;
	}

}//end namespace bitgraph




#endif