/**
 * @file simple_graph_imp.h
 * @brief Inline template implementations for the generic Graph class.
 *
 * @details
 * This implementation fragment defines the template and inline operations
 * declared by @c bitgraph::Graph. It covers graph construction and reset,
 * adjacency and edge-count maintenance, density and degree queries, neighbor
 * access, subgraph and complement creation, random-edge generation, graph
 * metadata, formatted output, and DIMACS, Matrix Market, and edge-list input.
 *
 * The graph stores adjacency rows through a compatible BITSCAN bitset type.
 * Operations that modify adjacency invalidate cached edge counts as needed,
 * while readers and density queries maintain the graph's consistency rules.
 * The definitions are kept in this header so templates can be instantiated for
 * dense and sparse adjacency representations.
 *
 * This file is included at the end of @c simple_graph.h and should not normally
 * be included directly by client code. All definitions belong to the
 * @c bitgraph namespace.
 *
 * @date created: 20/09/2026
 * @author P. San Segundo
 *
 * @see ../simple_graph.h
 * @see ../formats/detail/dimacs_format.h
 * @see ../formats/detail/matrix_market_format.h
 * @warning Graph operations require an adjacency bitset type compatible with
 *          the interface expected by @c Graph; invalid vertex indices follow
 *          the validation and error policies of the public graph class.
 */

#ifndef BITGRAPH_GRAPH_SIMPLE_GRAPH_IMP_H
#define BITGRAPH_GRAPH_SIMPLE_GRAPH_IMP_H

namespace bitgraph {
	
	template <class U>
	bool operator== (const Graph<U>& lhs, const Graph<U>& rhs) {
		return lhs.num_vertices() == rhs.num_vertices() &&
			   lhs.adj_ == rhs.adj_;
	}

	template <class U>
	bool operator!= (const Graph<U>& lhs, const Graph<U>& rhs) {
		return !(lhs.adj_ == rhs.adj_);
	}

} // namespace bitgraph


namespace bitgraph {

	template<class BitsetT>
	inline
		double Graph<BitsetT>::density(const vertex_bitset_t& vertices) const 
	{
		const std::size_t num_vertices = vertices.count();

		if (num_vertices < 2) {
			return 0.0;
		}

		const std::size_t num_edges = this->num_edges(vertices);

		const std::size_t max_edges =
			num_vertices * (num_vertices - 1);		// directed graph

		return static_cast<double>(num_edges) /
			static_cast<double>(max_edges);

	}

	template<class BitsetT>
	inline
		std::ostream& Graph<BitsetT>::print_edges(const vertex_bitset_t& bbsg, std::ostream& os) const
	{

		for (vertex_t i = 0; i < NV_ - 1; i++) {
			
			if (!bbsg.is_bit(i)) continue;

			for (vertex_t j = i + 1; j < NV_; j++) {

				if (!bbsg.is_bit(j)) continue;

				if (is_edge(i, j)) {
					os << "[" << i << "]" << "-->" << "[" << j << "]" << endl;
				}
				if (is_edge(j, i)) {
					os << "[" << j << "]" << "-->" << "[" << i << "]" << endl;
				}
			}
		}
		return os;
	}

	//////////////////////////////////////////
	//
	// class Graph<BitsetT> implementation

	template<class BitsetT>
	inline
		Graph<BitsetT>::Graph(void) noexcept :
		NV_(0), NBB_(0), NE_(0),edge_count_valid_(false),
		name_(""), path_("")
	{ }
			
	template <class BitsetT>
	inline
		Graph<BitsetT>::Graph(
			std::size_t NV, 
			const int * const adj[],
			std::string name) noexcept 
		: Graph()
	{
		// A null matrix is valid only when constructing an empty graph.
		assert(adj != nullptr || NV == 0);

		const vertex_t num_vertex = static_cast<vertex_t>(NV);

		reset(NV, std::move(name));

		for (vertex_t v = 0; v < num_vertex; ++v) {
			assert(adj[v] != nullptr);

			for (vertex_t w = 0; w < num_vertex; ++w) {
				if (adj[v][w] != 0) {
					add_edge(v, w);
				}
			}
		}		
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
				"Graph size exceeds the supported vertex-index range - "
				" Graph<BitsetT>::reset(std::size_t NV, std::string name).");
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
			NBB_ = (NV_ == 0) ? 0 : static_cast<int>(INDEX_1TO1(NV_));
			NE_ = 0;
			edge_count_valid_ = true;

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
	auto Graph<BitsetT>::create_subgraph_first_k(
			int first_k, 
			Graph<BitsetT>& graph) const -> Graph&
	{
		// Preconditions
		assert(&graph != this);
		assert(first_k >= 0 && first_k <= NV_);
		
		// Initialize output graph
		graph.reset(static_cast<std::size_t>(first_k));
		
		if (first_k == 0) {
			return graph;
		}
			
		const int last_block = WDIV(first_k - 1);
		
		// Copy the relevant blocks of the adjacency matrix
		for (vertex_t v = 0; v < graph.NV_; ++v) {
			for (int j = 0; j <= last_block; ++j) {
				graph.adj_[v].block(j) = adj_[v].block(j);
			}

			// Trim the last block
			graph.adj_[v].block(last_block) &=
				bblock::MASK_1_LOW(WMOD(first_k - 1));

			/*graph.adj_[v].block(last_block) &=
				~Tables::mask_high[WMOD(first_k - 1)];*/
		}

		// Edge count must be recomputed when needed
		graph.edge_count_valid_ = false;	

		return graph;
	}

	
	
	template<class BitsetT>
	inline
		void Graph<BitsetT>::shrink_to_fit()
	{
		for (auto& neighbors : adj_) {
			neighbors.shrink_to_fit();
		}		
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::shrink_to_fit(std::size_t new_size)
	{

		// Compare before converting to int.
		if (new_size >= static_cast<std::size_t>(NV_)) {
			LOGG_WARNING(
				"Invalid shrinking size ", new_size,
				": expected a value smaller than ", NV_,
				". The graph remains unchanged - Graph<BitsetT>::shrink_to_fit(std::size_t)");
			return -1;
		}

		const vertex_t new_vertex_count = static_cast<vertex_t>(new_size);

		// Remove edges to vertices outside the new range.
		for (vertex_t v = 0; v < new_vertex_count; ++v) {
			adj_[v].erase_bit(new_vertex_count, -1);
		}

		// Remove rows belonging to discarded vertices.
		adj_.resize(new_size);
		NV_ = new_vertex_count;

		NE_ = 0;
		edge_count_valid_ = false;
		NBB_ = INDEX_1TO1(NV_);			
				
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

		// edge count is valid after reading from a file
		
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::add_edge(vertex_t v, vertex_t w) 
	{
		assert(v >= 0 && v < NV_);
		assert(w >= 0 && w < NV_);
		
		if (v != w && !adj_[v].is_bit(w)) {
			adj_[v].set_bit(w);
			++NE_;
		}		
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_edge(vertex_t v, vertex_t w)
	{
		assert(v >= 0 && v < NV_);
		assert(w >= 0 && w < NV_);

		if (v != w && adj_[v].is_bit(w)) {
			adj_[v].erase_bit(w);

			// necessary to avoid NE_ underflow if edge_count_valid_ is false
			if (edge_count_valid_) {	
				--NE_;
			}
			
		}
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_edges(vertex_t v)
	{
		assert(v >= 0 && v < NV_);

		//erases all outgoing edges from v
		adj_[v].erase_bit();

		//erases all ingoing edges
		for (vertex_t w = 0; w < NV_; w++) {
			if (w != v) {
				adj_[w].erase_bit(v);
			}
		}

		// edge count is no longer valid after removing edges.
		edge_count_valid_ = false;		

	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::remove_edges() {

		for (auto& neighbors : adj_) {
			neighbors.erase_bit();
		}
				
		NE_ = 0;
		edge_count_valid_ = true;
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
		std::ostream& Graph<BitsetT>::timestamp_dimacs(
			std::ostream& o) const {
		o << "c File written by BITGRAPH:" 
			<< PrecisionTimer::local_timestamp();
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
		std::ostream& Graph<BitsetT>::header_dimacs(std::ostream& o, bool lazy) const
	{
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

		if (io::detail::dimacs::read_dimacs_header(f, n, m) == -1) {
			reset();
			f.close();
			return -1;
		}

		reset(n);
		io::detail::skip_empty_lines(f);   
		
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

		io::detail::MMI<Graph<BitsetT> > myreader(*this);
		return (myreader.read(filename));
	}

	template<class BitsetT>
	inline
		int Graph<BitsetT>::read_EDGES(const std::string& filename) noexcept {

		io::detail::EDGES<Graph<BitsetT> > myreader(filename, *this);
		return (myreader.read());
	}

	template<class BitsetT>
	inline
		ostream& Graph<BitsetT>::print_data(
			bool lazy,
			std::ostream& os,
			bool trailing_newline) const
	{

		if (!name_.empty()) {
			os << name_.c_str() << '\t'; 
		}

		os << "n:= " << num_vertices() << '\t'
			<< "m:= " << num_edges(lazy) << '\t';

		// Save stream formatting
		const auto flags = os.flags();
		const auto precision = os.precision();

		os << std::fixed << std::setprecision(3)
			<< "p:= " << density(true);

		// Restore stream formatting
		os.flags(flags);
		os.precision(precision);

		if (trailing_newline) {
			os << '\n'; 
		}
		return os;
	}

	template<class BitsetT>
	inline
		ostream& Graph<BitsetT>::print_edges(
			std::ostream& os, bool trailing_newline)
	{

		for (vertex_t v = 0; v < NV_ - 1; ++v) {
			for (vertex_t w = v + 1; w < NV_; ++w) {

				if (is_edge(v, w)) {
					os << "[" << v << "]" << "-->" << "[" << w << "]" << endl;
				}
				if (is_edge(w, v)) {
					os << "[" << w << "]" << "-->" << "[" << v << "]" << endl;
				}

			}
		}

		if (trailing_newline) {
			os << '\n'; 
		}
		return os;
	}

	template<class BitsetT>
	inline
		std::size_t Graph<BitsetT>::num_edges(
			const vertex_bitset_t& vertices) const
	{

		std::size_t NE = 0;

		for (vertex_t v = 0; v < this->NV_; ++v) {

			if (!vertices.is_bit(v)) {
				continue;
			}

			for (vertex_t w = 0; w < this->NV_; ++w) {
				if (vertices.is_bit(w) &&
					this->adj_[static_cast<std::size_t>(v)].is_bit(w)) {
					++NE;
				}
			}
		}

		return NE;;
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
		std::size_t Graph<BitsetT>::num_edges(bool lazy) const {
				
		if (!lazy || !edge_count_valid_ ) {					
			NE_ = 0;

			for (const auto& neighbors : adj_) {
				NE_ += static_cast<std::size_t>(neighbors.count());
			}

			edge_count_valid_ = true;  // The edge count is now valid after computation.
		}

		return NE_;
	}


	template<class BitsetT>
	inline
		double Graph<BitsetT>::density(bool lazy) const
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
			for (block_index_t block_id = 0; block_id < NBB_; ++block_id) {
				if (adj_[vertex].block(block_id) != 0) {
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
		bool Graph<BitsetT>::is_self_loop() const 
	{
		for (vertex_t v = 0; v < NV_; ++v) {
			if (adj_[v].is_bit(v)) {
				return true;
			}
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

	template<class BitsetT>
	inline
		degree_t Graph<BitsetT>::degree_in(vertex_t v) const 
	{
		assert(v >= 0 && v < NV_);
		
		degree_t degree = 0;

		for (vertex_t source = 0; source < NV_; source++) {
			if (adj_[source].is_bit(v)) {
				++degree; 
			}

		}
		return degree;
	}

	template<class BitsetT>
	inline
		degree_t Graph<BitsetT>::degree_out(vertex_t v) const
	{
		assert(v >= 0 && v < NV_);

		return adj_[v].count();
	}


	template<class BitsetT>
	inline
		void Graph<BitsetT>::create_complement(Graph& gcomp) const
	{
		if (this == &gcomp) {
			BaseT::graph_initialization_error(
				"Input and output graphs must be different in "
				"Graph::create_complement(Graph& gcomp).");
		}

		gcomp.reset(static_cast<std::size_t>(NV_));	
			
		for (vertex_t i = 0; i + 1< NV_; ++i) {
			for (vertex_t j = i + 1; j < NV_; ++j) {

				if (!adj_[i].is_bit(j)) {
					gcomp.add_edge(i, j);
				}

				if (!adj_[j].is_bit(i)) {
					gcomp.add_edge(j, i);
				}
			}
		}		

		//note: gcomp.edge_count_valid_ = true;  after reset
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::make_bidirected() {

		for (vertex_t i = 0; i + 1 < NV_; ++i) {
			for (vertex_t j = i + 1; j < NV_; ++j) {
				if (is_edge(i, j) ) {
					add_edge(j, i);
				}
				else if (is_edge(j, i) ) {
					add_edge(i, j);
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::gen_random_edges(double p) {

		//removes all edges
		remove_edges();

		//sets directed edges with probability p
		for (vertex_t v = 0; v < NV_; ++v) {
			for (vertex_t w = 0; w < NV_; ++w) {
				if (utils::uniform_dist(p)) {
					add_edge(v, w);
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
		void Graph<BitsetT>::create_subgraph(
			Graph& graph, 
			const vertex_set_t& vertices) const
	{
		assert(&graph != this);
		
		const vertex_t NV = static_cast<int>(vertices.size());

		// Preconditions
		for (vertex_t v : vertices) {
			assert(v >= 0 && v < NV_);
		}

		
		// Initialize output graph
		graph.reset(NV);


		// Add directed edges
		for (vertex_t v = 0; v < nV; ++v) {
			for (vertex_t w = 0; w < nV; ++w) {
				if (this->is_edge(vertices[v], vertices[w])) {
					graph.add_edge(v, w);
				}
			}
		}
	}

	template<class BitsetT>
	inline 
		void Graph<BitsetT>::create_subgraph(
			Graph& graph,
			vertex_t v) const
	{
		assert(v >= 0 && v < NV_);
		assert(&graph != this);

		vertex_set_t vnn;
		this->neighbors(v).extract(vnn);

		create_subgraph(graph, vnn);
	
	}

	template<class BitsetT>
	inline
		void Graph<BitsetT>::write_dimacs(ostream& os) {

		// Timestamp comment
		timestamp_dimacs(os);

		// Graph name
		name_dimacs(os);

		// DIMACS header
		header_dimacs(os, false);

		//write edges 1-based vertex notation 
		//O(log) for sparse graphs: specialize
		for (vertex_t v = 0; v < NV_; ++v) {
			for (vertex_t w = 0; w < NV_; ++w) {
				if (v != w && is_edge(v, w)) {
					os << "e " << v + 1 << " " << w + 1 << '\n';
				}
			}
		}
	}

	template<class BitsetT>
	inline
		void  Graph<BitsetT>::write_EDGES(ostream& os) {

		// Timestamp comment
		timestamp_dimacs(os);
	
		// Graph name
		if (!name_.empty())
			os << "\n%  " << name_.c_str() << endl;

		// Write directed edges (1-based vertex indices)
		// O(log) for sparse graphs: specialize
		for (vertex_t v = 0; v < NV_; ++v) {
			for (vertex_t w = 0; w < NV_; ++w) {
				if (v != w && is_edge(v, w)) {
					os << v + 1 << " " << w + 1 << '\n';
				}
			}
		}
	}

	template<class BitsetT>
	inline
	bool Graph<BitsetT>::is_edge(vertex_t v, vertex_t w) const {
		
		assert(v >= 0 && v < NV_);

		// unnecessary check with perfect contract, but it is a safe guard for the case of sparse graphs
		// and shrink_to_fit() functions 
		if (static_cast<unsigned>(w) >= static_cast<unsigned>(NV_)) {
			return false;
		}

		return(adj_[v].is_bit(w));
	}

} //end namespace bitgraph

#endif // BITGRAPH_GRAPH_SIMPLE_GRAPH_IMP_H	
