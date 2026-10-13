/**
 * @file graph_gen_imp.h
 * @brief Implementation of the RandomGen class for random graph generation.
 *
 * @details Provides implementations for generating random graphs,
 * benchmark collections, and isomorphic graphs through vertex permutations.
 * Supports directed and undirected graph types.
 *
 * @author pss
 * @date Last updated: 13/10/2026
 *
 * @copyright This file is part of the BITGRAPH C++ library.
 */

#ifndef BITGRAPH_GRAPH_GEN_IMP_H
#define BITGRAPH_GRAPH_GEN_IMP_H

namespace bitgraph{	

	template<class GraphT>
	void  RandomGen<GraphT>::create_graph(
		graph_t& graph,
		std::size_t NV,
		double p)
	{

		if (NV == 0 || !(p >= 0.0 && p <= 1.0)) {
			LOG_ERROR("Invalid parameters - RandomGen<GraphT>::create_graph");
			std::terminate();
		}

		
		graph.reset(NV);
		graph.gen_random_edges(p);


		// Name: r<NV>_<p>.txt
		std::stringstream sstr;
		sstr << "r" << NV << "_" << p << ".txt";
		graph.set_name(sstr.str());
	}

	template<class GraphT>
	void RandomGen<GraphT>::create_graph_benchmark(
		const std::string& path,
		const random_attr_t& rd)
	{
		// add slash '/' at the end to the path if required
		std::string mypath(path);
		utils::append_slash(mypath);


		graph_t g;


		for (std::size_t n = rd.nLB; ; ) {

			for (std::size_t pi = 0; ; ++pi) {

				const double p = rd.pLB + pi * rd.incP;

				if (p > rd.pUB)
					break;

				for (std::size_t k = 0; k < rd.nRep; ++k) {

					create_graph(g, n, p);

					std::ostringstream os;
					os << mypath << "r" << n << "_"
						<< p << "_" << k << ".txt";

					std::ofstream f(os.str());

					if (!f) {
						LOG_ERROR("Cannot open file: {}", os.str());
						std::terminate();
					}

					g.write_dimacs(f);

					if (!f) {
						LOG_ERROR("Cannot write graph to file: {}", os.str());
						std::terminate();
					}
				}

				if (p >= rd.pUB)
					break;
			}

			if (n >= rd.nUB || rd.nUB - n < rd.incN)
				break;

			n += rd.incN;
		}
	}


	template<class GraphT>
	inline
		void RandomGen<GraphT>::create_isomorphism(
			graph_t& g_iso, 
			graph_t& g_source)
	{
		// Generate a random permutation
		vertex_ordering order(g_source.size(), 0);
		std::iota(order.begin(), order.end(), 0);
		
		std::default_random_engine rng(
			_rand::RandomUniformGen<>::FIXED_RANDOM_SEED);
		
		std::shuffle(order.begin(), order.end(), rng);

		// Construct the isomorphic graph
		GraphFastRootSort<graph_t> gs(g_source);
		gs.reorder(order, g_iso);

	}

	template<int logSizeTable>
	inline
		void experimental::SparseRandomGen<sparse_ugraph, logSizeTable>::decode(
			BITBOARD edge_index,
			unsigned int& i,
			unsigned int& j, 
			unsigned int n) 
	{
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
	void experimental::SparseRandomGen<sparse_ugraph, logSizeTable>::init_log_table()
	{
		for (int i = 0; i < logSizeTable; i++) {
			log_[i] = log(i / (double)logSizeTable);
		}
	}

	template<int logSizeTable>
	int experimental::SparseRandomGen<sparse_ugraph, logSizeTable>::create_ugraph(
		sparse_ugraph& g,
		std::size_t n,
		double p) 
	{

		BITBOARD m = ((BITBOARD)n * (n - 1) / 2);			//max num edges
		BITBOARD e = 0;								//**check
		double logp = log(1 - p);
		BITBOARD skip;
		unsigned int i = 0, j = 0;
		int num = 0;									//optional index which counts edges added to the graph

		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0, logSizeTable - 1);
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

	template<class GraphT>
	inline
		int experimental::WeightGen<GraphT>::create_weights(
			GraphT& g, 
			type_t type, 
			int wmod, 
			std::string FILE_EXTENSION,
			std::string FILE_PATH) 
	{

		const int NV = g.num_vertices();

		//no need to clear the current weights since they are overwritten

		switch (type) {
		case WDEG:
			for (int v = 0; v < NV; ++v) {
				g.set_weight(v, g.graph().degree(v));
			}
			break;
		case WMOD:
			for (int v = 0; v < NV; ++v) {
				g.set_weight(v, (1 + ((v + 1) % wmod)));
			}
			break;
		default:
			LOG_INFO("incorrect weight generation mode - WeightGen<GraphT>::create_weights");
			return -1;
		}

		/////////////////////////////
		//streams weights to file
		if (!FILE_EXTENSION.empty() && !FILE_PATH.empty()) {

			//determines the filename
			std::string path = FILE_PATH;
			utils::append_slash(path);
			std::string filename = path + g.graph().name() + FILE_EXTENSION;

			//streams weights to file
			std::ofstream f(filename);
			if (!f) {
				LOGG_INFO("WeightGen<GraphT>::create_weights () - cannot write weights to file ", filename);
				return -1;
			}

			///////////////////////////
			g.print_weights(f, false);			//no vertex index is streamed with the weight value
			///////////////////////////

			f.close();
		}

		return 0;
	}

	template<class GraphT>
	inline
		int	experimental::EdgeWeightGen<GraphT>::create_weights(
			GraphT& g, 
			type_t type,
			int wmod, 
			std::string FILE_EXTENSION, std::string FILE_PATH)
	{
		switch (type) {
		case WMOD:
			g.set_modulus_weights(wmod);
			break;
		default:
			LOG_INFO("bad weight generation mode - EdgeWeightGen<GraphT>::create_weights");
			LOG_INFO("original weights unchanged");
			return -1;
		}

		/////////////////////////////
		//streams weights to file
		if (!FILE_EXTENSION.empty() && !FILE_PATH.empty()) {

			//determines the filename
			std::string path = FILE_PATH;
			utils::append_slash(path);
			std::string filename = path + g.graph().name() + FILE_EXTENSION;

			//streams weights to file
			std::ofstream f(filename);
			if (!f) {
				LOGG_INFO("cannot write edge-weights to file ", filename, " - EdgeWeightGen<GraphT>::create_weights");
				return -1;
			}

			///////////////////////////
			g.print_edges(f, GraphT::edge_print_format::edge_matrix, false);
			///////////////////////////

			f.close();
		}

		return 0;
	}

}//end namespace bitgraph



#endif //BITGRAPH_GRAPH_GEN_IMP_H__
