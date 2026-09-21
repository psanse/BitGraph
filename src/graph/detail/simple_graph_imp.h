/**
 * @file simple_graph_impl.h
 * @brief Provides template and inline implementations for the Graph class.
 *
 * This file contains definitions that must remain visible to translation
 * units using Graph, including template member functions and inline graph
 * operations.
 *
 * The implementation supports graph representations whose adjacency sets are
 * stored using compatible BITSCAN bitset types.
 *
 * This file is included at the end of simple_graph.h and should not normally
 * be included directly by client code.
 *
 * @note All definitions in this file belong to the bitgraph namespace.
 *
 * @date created: 20/09/2026
 * @author P. San Segundo
 */

#ifndef BITGRAPH_GRAPH_SIMPLE_GRAPH_IMP_H
#define BITGRAPH_GRAPH_SIMPLE_GRAPH_IMP_H


namespace bitgraph {
	
	template <class U>
	bool operator== (const Graph<U>& lhs, const Graph<U>& rhs) {
		return lhs.adj_ == rhs.adj_;
	}

	template <class U>
	bool operator!= (const Graph<U>& lhs, const Graph<U>& rhs) {
		return lhs.adj_ != rhs.adj_;
	}

} // namespace bitgraph


namespace bitgraph {

	template<class BitsetT>
	inline
		double Graph<BitsetT>::density(const BitsetT& vertices) const 
	{
		const BITBOARD num_vertices =
			static_cast<BITBOARD>(vertices.count());

		if (num_vertices < 2) {
			return 0.0;
		}

		const BITBOARD edges =
			static_cast<BITBOARD>(num_edges(vertices));

		const BITBOARD maxEdges =
			num_vertices * (num_vertices - 1);		// directed graph

		return static_cast<double>(edges) /
			static_cast<double>(maxEdges);

	}

	template<class BitsetT>
	template<class U>
	inline
		std::ostream& Graph<BitsetT>::print_edges(U& bbsg, std::ostream& o) const {

		for (int i = 0; i < NV_ - 1; i++) {

			if (!bbsg.is_bit(i)) continue;

			for (int j = i + 1; j < NV_; j++) {

				if (!bbsg.is_bit(j)) continue;

				if (is_edge(i, j)) {
					o << "[" << i << "]" << "-->" << "[" << j << "]" << endl;
				}
				if (is_edge(j, i)) {
					o << "[" << j << "]" << "-->" << "[" << i << "]" << endl;
				}
			}
		}
		return o;
	}

	//////////////////////////////////////////
	//
	// class Graph<BitsetT> implementation

	template<class BitsetT>
	inline
		Graph<BitsetT>::Graph(void) noexcept :
		NV_(0), NE_(0), NBB_(0), edge_count_valid_(false),
		name_(""), path_("")
	{ }

	template<class BitsetT>
	inline
		Graph<BitsetT>::Graph(std::string filename) noexcept
		: Graph() 		
	{
		reset(filename);		
	}

	template<class BitsetT>
	inline 
		Graph<BitsetT>::Graph(std::size_t NV) noexcept
		: Graph()
	{		
		reset(NV);
	}

	template <class BitsetT>
	inline
		Graph<BitsetT>::Graph(std::size_t NV, int* adj[], std::string filename) noexcept 
		: Graph()
	{
		// A null matrix is valid only when constructing an empty graph.
		assert(adj != nullptr || NV == 0);

		const vertex_t num_vertex = static_cast<vertex_t>(NV);

		reset(NV, std::move(filename));

		for (vertex_t v = 0; v < num_vertex; ++v) {
			assert(adj[v] != nullptr);

			for (vertex_t w = 0; w < num_vertex; ++w) {
				if (adj[v][w] != 0) {
					add_edge(v, w);
				}
			}
		}

		edge_count_valid_ = true;  // The edge count is valid after constructing from an adjacency matrix.
	}


	template<class BitsetT>
	inline
	void Graph<BitsetT>::set_name(std::string name_or_path) {

		std::string new_name;
		std::string new_path;

		const std::string::size_type separator =
			name_or_path.find_last_of("/\\");

		if (separator != std::string::npos) {
			new_name = name_or_path.substr(separator + 1);
			new_path = name_or_path.substr(0, separator + 1);
		}
		else {
			new_name = std::move(name_or_path);
		}

		/*
		 * Commit only after both strings have been constructed successfully.
		 * swap() does not allocate when standard string allocators are used.
		 */
		name_.swap(new_name);
		path_.swap(new_path);	
	}

	template<class BitsetT>
	inline
	void Graph<BitsetT>::reset() noexcept {
		adj_.clear(), name_.clear(), path_.clear();
		NV_ = 0, NBB_ = 0, NE_ = 0;
		edge_count_valid_ = true;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::reset(std::size_t NV, std::string name) noexcept {
		
		//check size - must fit in int type
		if (NV > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
			graph_initialization_error(
				"Graph size exceeds the supported vertex-index range - reset(std::size_t NV, std::string name).");
		}
		
		try {
			/*
			 * Build the new adjacency matrix independently. This avoids leaving
			 * the graph partially reinitialized while allocation is in progress.
			 *
			 * reserve() also prevents vector reallocations while rows are added.
			 */
			std::vector<BitsetT> new_adjacency;
			new_adjacency.reserve(NV);


			for (std::size_t i = 0; i < NV; ++i) {
				new_adjacency.emplace_back(BitsetT{ NV });
			}

			// Commit the newly constructed representation.
			adj_.swap(new_adjacency);

			NV_ = static_cast<int>(NV);
			NBB_ = static_cast<int>(INDEX_1TO1(NV_));
			NE_ = 0;
			edge_count_valid_ = false;

			set_name(std::move(name));
		}
		catch (const std::bad_alloc& error) {
			LOGG_ERROR(
				"Unable to allocate memory for a graph with ",
				NV,
				" vertices - reset(std::size_t NV, std::string name): ",
				error.what());

			std::terminate();
		}
		catch (const std::exception& error) {
			LOGG_ERROR(
				"Graph initialization failed - reset(std::size_t NV, std::string name): ",
				error.what());

			std::terminate();
		}
		catch (...) {
			LOG_ERROR("Graph initialization failed - reset(std::size_t NV, std::string name): with an unknown error.");
			std::terminate();
		}		
	}

	template<class BitsetT>
	inline
		Graph<BitsetT>& Graph<BitsetT>::create_subgraph(int first_k, Graph<BitsetT>& newg) const {

		//assert is size required is greater or equal current size
		if (first_k >= NV_ || first_k <= 0) {
			LOG_ERROR("Bad new size - graph remains unchangedGraph<BitsetT>&- Graph<BitsetT>::create_subgraph");
			return newg;
		}

		//////////////////////
		newg.reset(static_cast<std::size_t>(first_k));
		///////////////////////
				
		auto bbh = WDIV(first_k - 1);

		//copy the relevant vertices of the adjacency matrix
		for (int i = 0; i < newg.NV_; ++i) {
			for (int j = 0; j <= bbh; ++j) {
				newg.adj_[i].block(j) = adj_[i].block(j);
			}
			//trims last bitblock
			newg.adj_[i].block(bbh) &= ~Tables::mask_high[WMOD(first_k - 1)];
		}

		return newg;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::shrink_to_fit() {

		for (vertex_t v = 0; v < NV_; ++v) {
			adj_[v].shrink_to_fit();
		}


		edge_count_valid_ = false;
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::shrink_to_fit(std::size_t N) {

		/*LOG_ERROR("not yet implemented for non-sparse graphs - Graph<BitsetT>::shrink_to_fit");
		LOG_ERROR("graph remains unchanged");*/

		//assertions
		if (NV_ <= N) {
			LOGG_WARNING("Wrong shrinking size ", N, " the graph remains unchanged - Graph<BitsetT>::shrink_to_fit");
			return -1;
		}

		//sets to 0 bitblocks outside the range but
		//does not remove the empty bitbloks
		for (vertex_t v = 0; v < NV_; ++v) {
			adj_[v].erase_bit(NV_, -1);
		}

		//resizes adjacency matrix
		adj_.resize(N);
		NV_ = static_cast<int>(N);
		NE_ = 0;												// clears cached value
		edge_count_valid_ = false;								// so that when num edges are required, its value will be recomputed
		NBB_ = INDEX_1TO1(NV_);									// maximum number of bitblocks per row (for sparse graphs)		

		return 0;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::reset(std::string filename) noexcept
	{
		const bool loaded =
			read_dimacs(filename) != -1 ||
			read_mtx(filename) != -1 ||
			read_EDGES(filename) != -1 ||
			read_01(filename) != -1;

		if (!loaded) {
			const std::string message =
				"Unable to read graph from file '" + filename + "'.\n"
				"Formats considered: DIMACS / MTX / EDGES / 01.";

			graph_initialization_error(message.c_str());
		}
		
		edge_count_valid_ = false;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::add_edge(vertex_t v, vertex_t w) 
	{
		assert(v >= 0 && v < NV_);
		assert(w >= 0 && w < NV_);

		if (v != w) {
			adj_[v].set_bit(w);
			NE_++;
		}
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_edge(vertex_t v, vertex_t w)
	{
		assert(v >= 0 && v < NV_);
		assert(w >= 0 && w < NV_);
		adj_[v].erase_bit(w);
		NE_--;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_edges(vertex_t v)
	{
		assert(v >= 0 && v < NV_);

		//erases all outgoing edges from v
		adj_[v].erase_bit();

		//erases all ingoing edges
		for (int w = 0; w < NV_; w++) {
			if (w == v) continue;
			adj_[w].erase_bit(v);
		}

		edge_count_valid_ = false;  // The edge count is no longer valid after removing edges.

		//updates edges
		//NE_ = 0;					//resets edges to avoid lazy evaluation later

	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_edges() {
		for (int v = 0; v < NV_; ++v) {
			adj_[v].erase_bit();
		}

		edge_count_valid_ = false;  // The edge count is no longer valid after removing edges.
		//NE_ = 0;
	}

	template <class BitsetT>
	inline
		ostream& Graph<BitsetT>::print_adj(std::ostream& o, bool eofl) const {

		for (int i = 0; i < NV_; ++i) {
			for (int j = 0; j < NV_; ++j) {
				if (is_edge(i, j)) {
					o << "1" << " ";
				}
				else {
					o << "0" << " ";
				}
			}
			o << '\n';
		}

		if (eofl) o << '\n';
		return o;
	}

	template<class BitsetT>
	inline
		std::ostream& Graph<BitsetT>::timestamp_dimacs(std::ostream& o) const {
		o << "c File written by GRAPH:" << PrecisionTimer::local_timestamp();
		return o;
	}

	template<class BitsetT>
	inline
		std::ostream& Graph<BitsetT>::name_dimacs(std::ostream& o) const {
		if (!name_.empty())
			o << "\nc " << name_.c_str() << endl;
		return o;
	}

	template<class BitsetT>
	inline
		std::ostream& Graph<BitsetT>::header_dimacs(std::ostream& o, bool lazy) {
		o << "p edge " << NV_ << " " << num_edges(lazy) << endl << endl;
		return o;
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::read_dimacs(const std::string& filename) noexcept{

		int n = 0, m = 0, v1 = 0, v2 = 0;			

		fstream f(filename.c_str());
		if (!f) {
			LOG_ERROR("Graph<BitsetT>::read_dimacs-File could not be opened reading DIMACS format");
			reset();
			return -1;
		}

		if (gio::dimacs::read_dimacs_header(f, n, m) == -1) {
			reset();
			f.close();
			return -1;
		}

		reset(n);
		gio::skip_empty_lines(f);   
		
		////////////////////////
		//parse edges directly from the stream
		std::string line; char c;
		for (int e = 0; e < m; e++) {			//iterates on the number of edges, TODO - iterate on lines and check edges
			f >> c;
			if (c != 'e') {
				LOGG_ERROR(filename, ":wrong header for edges reading DIMACS format");
				reset();
				f.close();
				return -1;
			}

			//read and add edge
			f >> v1 >> v2;
#ifdef DIMACS_INDEX_0_FORMAT 
			add_edge(v1, v2);
#else
			add_edge(v1 - 1, v2 - 1);
#endif
					
			//removes remaining part of the line
			std::getline(f, line);		
		}

		f.close();
				
		set_name(filename);			//removes full path
		return 0;
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::read_01(const std::string& filename) noexcept {


		int size;
		char ch, line[250];

		fstream f(filename.c_str());
		if (!f) {
			LOG_ERROR("Graph<BitsetT>::read_01-File could not be opened reading DIMACS format");
			reset();
			f.close();
			return -1;
		}

		//read size
		f >> size;

		///////////////
		reset(size);
		///////////////

		f.getline(line, 250);  //remove final part of the line

		//read rows		
		for (int r = 0; r < size; r++) {
			for (int c = 0; c < size; c++) {
				f >> ch;
				if ((r < c) && ch == '1') {
					add_edge(r, c);
				}
			}
			f.getline(line, 250);  //remove final part of the line
		}
		f.close();

		//name (removes path)
		set_name(filename);
		return 0;
	}

	template<class BitsetT>
	inline
		int  Graph<BitsetT>::read_mtx(const std::string& filename) noexcept {

		detail::MMI<Graph<BitsetT> > myreader(*this);
		return (myreader.read(filename));
	}

	template<class BitsetT>
	inline
		int  Graph<BitsetT>::read_EDGES(const std::string& filename) noexcept {

		EDGES<Graph<BitsetT> > myreader(filename, *this);
		return (myreader.read());
	}

	template<class BitsetT>
	inline
		ostream& Graph<BitsetT>::print_data(bool lazy, std::ostream& o, bool eofl) {

		if (!name_.empty()) { o << name_.c_str() << '\t'; }

		////////////////////////////////////////////////////////////////
		o << "n:= " << num_vertices() << "\t"
			<< std::fixed << "m:= " << num_edges(lazy) << "\t"
			<< std::setprecision(3) << "p:= " << density(true);
		////////////////////////////////////////////////////////////////

		/*Note: lazy comp. of density since the number of edges have already been computed previously */

		if (eofl) { o << '\n'; }
		return o;
	}

	template<class BitsetT>
	inline
		ostream& Graph<BitsetT>::print_edges(std::ostream& o, bool eofl) {

		for (int i = 0; i < NV_ - 1; ++i) {
			for (int j = i + 1; j < NV_; ++j) {

				if (is_edge(i, j)) {
					o << "[" << i << "]" << "-->" << "[" << j << "]" << endl;
				}
				if (is_edge(j, i)) {
					o << "[" << j << "]" << "-->" << "[" << i << "]" << endl;
				}

			}
		}

		if (eofl) { o << '\n'; }
		return o;
	}



	template<class BitsetT>
	inline
		std::size_t Graph<BitsetT>::num_edges(const BitsetT& bbn) const
	{

		std::size_t NE = 0;

		for (int i = 0; i < NV_; ++i) {
			if (bbn.is_bit(i)) {
				for (int j = 0; j < NV_; ++j) {
					if (bbn.is_bit(j)) {						//includes possible self loops
						if (adj_[i].is_bit(j)) { NE++; }
					}
				}
			}
		}

		return NE;
	}

	template<class BitsetT>
	inline
	auto Graph<BitsetT>::neighbors(vertex_t v) const -> const vertex_bitset_t&
	{
		assert(v >= 0 && v < NV_);

		return adj_[v];
	}

	template<class BitsetT>
	inline
	auto Graph<BitsetT>::neighbors(vertex_t v) -> vertex_bitset_t&
	{
		assert(v >= 0 && v < NV_);

		// The caller may modify the returned adjacency row.
		edge_count_valid_ = false;

		return adj_[v];
	}



	template<class BitsetT>
	inline
		std::size_t Graph<BitsetT>::num_edges(bool lazy) {

		/*
		 * NE_ == 0 also acts as the "not yet computed" sentinel.
		 * Consequently, an edgeless graph is rescanned on every lazy call.
		 */
		if (!lazy || !edge_count_valid_ ) {					
			NE_ = 0;

			for (const auto& neighbors : adj_) {
				NE_ += static_cast<std::size_t>(neighbors.count());
			}

			edge_count_valid_ = true;  // The edge count is now valid after computation.
		}

		return NE_;
	}

	/**
	 * @brief Computes the edge density of the directed graph.
	 *
	 * The density is defined as
	 * \f[
	 *     \frac{|E|}{|V|(|V|-1)}
	 * \f]
	 * for a directed graph without self-loops.
	 *
	 * @param lazy If `true`, num_edges() may use its cached edge count.
	 * @return Graph density in the interval `[0,1]`. Returns `0.0` when the graph
	 *         contains fewer than two vertices.
	 */
	template<class BitsetT>
	inline
		double Graph<BitsetT>::density(bool lazy)
	{
		if (NV_ < 2) {	return 0.0; }

		/*
		 * Convert before multiplication so that the product is computed using a
		 * 64-bit unsigned type. The value may exceed a 32-bit integer.
		 */
		const BITBOARD num_vertices = static_cast<BITBOARD>(NV_);
		const BITBOARD max_num_edges = num_vertices * (num_vertices - 1);

		return static_cast<double>(num_edges(lazy)) /
			static_cast<double>(max_num_edges);
	}

	template<class BitsetT>
	inline
		double Graph<BitsetT>::block_density()	const
	{

		if (NV_ == 0 || NBB_ == 0) {
			return 0.0;
		}

		std::size_t non_empty_blocks = 0;

		for (vertex_t vertex = 0; vertex < NV_; ++vertex) {
			for (block_index_t block = 0; block < NBB_; ++block) {
				if (adj_[vertex].block(block) != 0) {
					++non_empty_blocks;
				}
			}
		}

		const std::size_t total_blocks =
			static_cast<std::size_t>(NV_) *
			static_cast<std::size_t>(NBB_);

		return static_cast<double>(nonemptyBlocks) /
			static_cast<double>(totalBlocks);

		//size_t nBB = 0;
		//for (auto v = 0u; v < NV_; ++v) {
		//	for (auto bb = 0u; bb < NBB_; bb++) {
		//		if (adj_[v].block(bb))		//non-empty bitblock
		//			nBB++;
		//	}
		//}

		//return (nBB / static_cast<double>(NBB_ * NV_));
	}


	template<class BitsetT>
	inline
		bool Graph<BitsetT>::is_self_loop() const {
		for (auto i = 0u; i < NV_; i++)
			if (adj_[i].is_bit(i)) {
				return true;
			}
		return false;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_vertices(const Bitset& bbn, Graph& g) {

		//determine the size of the graph g
		auto pc = bbn.count();
		auto new_size = NV_ - pc;

		if (new_size <= 0) {
			LOG_ERROR("empty graph after deletion - Graph<BitsetT>::remove_vertices");
			g.reset();
			return;
		}

		//initialize new graph
		if (g.reset(new_size) == -1) {
			LOG_ERROR("memory for graph not allocated - Graph<BitsetT>::remove_vertices");
			return;
		}

		//computes the induced graph in g
		g.name(this->name_);
		int l = 0;

		//adds the edges with endpoints not in the bbn set
		for (int i = 0; i < NV_ - 1; i++) {
			if (bbn.is_bit(i)) continue;				//jumps over vertices marked for deletion
			int m = l + 1;
			for (int j = i + 1; j < NV_; j++) {
				if (bbn.is_bit(j)) continue;				//jumps over vertices marked for deletion
				if (adj_[i].is_bit(j)) {
					g.add_edge(l, m);
				}
				m++;
			}
			l++;
		}

	}

	//template<class BitsetT>
	//void Graph<BitsetT>::remove_vertices (const Bitset& bbn){
	/////////////////
	//// Experimental: deletes input list of nodes by creating a temporal graph
	////
	//// OBSERVATIONS:
	//// 1.Inefficient implementation with double allocation of memory
	//
	//	Graph<BitsetT> g;
	//	this->remove_vertices(bbn,g);			//allocation 1
	//	(*this)=g;								//allocation 2	
	//}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::degree_in(vertex_t v) const 
	{
		assert(v >= 0 && v < NV_);

		int res = 0;
		for (auto i = 0u; i < NV_; i++) {
			if (i == v) continue;
			if (adj_[i].is_bit(v)) { res++; }

		}
		return res;
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::degree_out(vertex_t v) const
	{
		assert(v >= 0 && v < NV_);

		return adj_[v].count();
	}


	template<class BitsetT>
	inline
		int Graph<BitsetT>::create_complement(Graph& g) const
	{
		//resets g with new allocation
		if (g.reset(NV_) == -1) return -1;

		for (int i = 0; i < NV_ - 1; ++i) {
			for (int j = i + 1; j < NV_; ++j) {

				if (!adj_[i].is_bit(j)) {
					g.add_edge(i, j);
				}

				if (!adj_[j].is_bit(i)) {
					g.add_edge(j, i);
				}
			}
		}

		return 0;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::make_bidirected() {

		for (int i = 0; i < NV_; ++i) {
			for (int j = 0; j < NV_; ++j) {
				if (is_edge(i, j)) add_edge(j, i);
				if (is_edge(j, i)) add_edge(i, j);
			}
		}
		
		
		edge_count_valid_ = false;
		//NE_ = 0;	//resets edges to avoid lazy evaluation later
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::gen_random_edges(double p) {

		//removes all edges
		remove_edges();

		//sets directed edges with probability p
		for (int i = 0; i < NV_; ++i) {
			for (int j = 0; j < NV_; ++j) {
				if (_rand::uniform_dist(p)) {
					add_edge(i, j);
				}
			}
		}
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::gen_random_edge(vertex_t v, vertex_t w, double p) {

		//assert - TODO condition to DEBUG mode
		if (v == w || v >= NV_ || w >= NV_ || p < 0 || p > 1) {
			LOG_ERROR("wrong input params - Graph<BitsetT>::gen_random_edge");
			return -1;
		}

		//generates edge
		if (_rand::uniform_dist(p)) {
			add_edge(v, w);
		}

		return 0;
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::write_dimacs(ostream& o) {

		//timestamp comment
		timestamp_dimacs(o);

		//name comment
		name_dimacs(o);

		//dimacs header - recompute edges
		header_dimacs(o, false);

		//write edges 1-based vertex notation 
		for (int v = 0; v < NV_; ++v) {
			for (int w = 0; w < NV_; ++w) {
				if (v == w) continue;
				if (is_edge(v, w)) {								//O(log) for sparse graphs: specialize
					o << "e " << v + 1 << " " << w + 1 << endl;
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void  Graph<BitsetT>::write_EDGES(ostream& o) {

		//timestamp comment
		o << "% File written by GRAPH:" << PrecisionTimer::local_timestamp();

		//name comment
		if (!name_.empty())
			o << "\n%  " << name_.c_str() << endl;

		//write edges 1-based vertex notation
		for (int v = 0; v < NV_; ++v) {
			for (int w = 0; w < NV_; ++w) {
				if (v == w) continue;
				if (is_edge(v, w)) {							//O(log) for sparse graphs: specialize
					o << v + 1 << " " << w + 1 << endl;
				}

			}
		}
	}

	template<class BitsetT>
	inline
	bool Graph<BitsetT>::is_edge(vertex_t v, vertex_t w) const {
		assert(v >= 0 && v < NV_);

		// unnecessary check with perfect contract, but it is a safe guard for the case of sparse graphs
		// and shrhink_to_fit() functions 
		if (static_cast<unsigned>(w) >= static_cast<unsigned>(NV_)) {
			return false;
		}
		return(adj_[v].is_bit(w));
	}

} //end namespace bitgraph




#endif // BITGRAPH_GRAPH_SIMPLE_GRAPH_IMP_H	
