#include "lutils.h"
#include "ldraw.h"
#include "graph.h"

using namespace leda;

template <class Graph_t>
void Ldraw<Graph_t>::graph_win(bool is_undirected){
//launches empty graphwin configured for DAG or undirected graphs
	GraphWin GW;
	GW.set_edge_direction(undirected_edge,is_undirected);
		
	GW.display();
	GW.edit();
}

template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(leda::ugraph & G, const list<node>& ln, bool snapshot){
/////////////////
// shows only subgraph passed

	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	G.make_directed();		//For drawing
	GW.set_graph(G);

	//show only list of nodes
	node v;
	forall_nodes (v,GW.get_graph()){
		if(ln.search(v)==0)
			GW.get_graph().hide_node(v);
	}
	
	
	
	//circular layout
	node_array<double> x(G);
	node_array<double> y(G);
	Ldraw::embed_circular(G,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	G.make_undirected();
}

template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(const Graph_t& gbb, bool snapshot){
	ugraph g;
	copy_graph_bb_to_leda(gbb,g);
	draw_circular_layout(g);
}



template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(const Graph_t& gbb, const typename Graph_t::bb_type& bb_subgraph, bool snapshot){
/////////////////////////////
// draws subgraph only
	ugraph g;
	copy_graph_bb_to_leda(gbb,g);
	list<node> LN;
	node v;
	forall_nodes(v,g){
		int id=g.index(v);		/*0 based*/
		if(bb_subgraph.is_bit(id))
			LN.push(v);
	}
	
	draw_circular_layout(g,LN);
}

template <class Graph_t>
 void Ldraw<Graph_t>::draw_circular_layout(leda::ugraph & G,int user_labels [],label_pos_t pos, bool snapshot){
	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	GW.set_node_label_type(user_label);
	switch(pos){
	case UP:
			 GW.set_node_label_pos(gw_position::north_pos);
		break;
	case DOWN:
			 GW.set_node_label_pos(gw_position::south_pos);
		break;
	case LEFT:
			 GW.set_node_label_pos(gw_position::west_pos);
		break;
	case RIGHT:
			 GW.set_node_label_pos(gw_position::east_pos);
		break;
	case UPRIGHT:
			 GW.set_node_label_pos(gw_position::northeast_pos);
		break;
	case UPLEFT:
			 GW.set_node_label_pos(gw_position::northwest_pos);
		break;
	case DOWNRIGHT:
			 GW.set_node_label_pos(gw_position::southeast_pos);
		break;
	case DOWNLEFT:
			 GW.set_node_label_pos(gw_position::southwest_pos);
		break;
	case CENTER:
		GW.set_node_label_pos(gw_position::central_pos);
		break;
	default:
		cout<<"incorrect position of labels"<<endl;
		return;
	}

	G.make_directed();		//For drawing
	GW.set_graph(G);
	
	
	node v;
	std::stringstream sstr;
	int i=0;
	forall_nodes(v,G){
		sstr.str("");
		sstr<<G.index(v)<<":"<<user_labels[i++];
		GW.set_label(v,sstr.str().c_str());
	}


	//circular layout
	node_array<double> x(G);
	node_array<double> y(G);
	Ldraw::embed_circular(G,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	
	GW.display();
	GW.edit();
	
	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	G.make_undirected();

}





//template <class Graph_t>
//Ldraw<Graph_t>::Ldraw(void){
//	clear();
//}


template <class Graph_t>
void Ldraw<Graph_t>::set_graph(const Graph_t& g){
////////////////
// Sets graphBB in leda graph
	
	clear();
	int n=g.number_of_vertices();
	node v;
	leda::map<int,node> n_id;
	for(int i=0; i<n; i++){
		v=m_g.new_node();
		n_id[i]=v;
	}

	for(int i=0; i<n-1; i++)
		for(int j=i+1; j<n; j++){
			if(g.is_adjacent(i,j))
				m_g.new_edge(n_id[i],n_id[j]);
		}
}

template <class Graph_t>
void Ldraw<Graph_t>::set_graph(const leda::ugraph& g){
	CopyGraph(m_g,g);
}

template <class Graph_t>
void Ldraw<Graph_t>::set_graph(std::string filename){
	ReadDimacsUG(filename.c_str(),m_g);			
}

template <class Graph_t>
void Ldraw<Graph_t>::set_graph(const Graph_t& g, const typename Graph_t::bb_type& bbN){
////////////////
// Sets induced subgraph of G for vertices listed in bbN
	
	
	node v;
	list<int> l_nodes;
	leda::map<int,node> n_id;
	int nv=EMPTY_ELEM;

//	clear();
	bbN.init_scan(bbo::NON_DESTRUCTIVE);		//type dependent!
	while(true){
		nv=bbN.next_bit(nv);
		if(nv==EMPTY_ELEM) break;
		v=m_g.new_node();
		n_id[nv]=v;
		l_nodes.append(nv);
	}

	//Runs through all pairs of elements in the list of nodes
	for(int i=0; i<l_nodes.size()-1; i++){
		for(int j=i+1; j<l_nodes.size(); j++){
			if(g.is_adjacent(l_nodes.inf(l_nodes[i]),l_nodes.inf(l_nodes[j])))
					m_g.new_edge(n_id[i],n_id[j]);

		}
	}
	
	
	//Implementation using the bitstring of vertices
	/*nv=PRIMER_BIT_SIGNIFICATIVO;
	while(1){
		nv=bbN.SiguienteBitN64(nv);
		if(nv==-1) 
			break;
		nv1=nv;
		while(1){
			nv1=bbN.SiguienteBitN64(nv1);
			if(nv1==-1)
				break;
			if(g.IsEdge(nv,nv1))
				m_g.new_edge(M[nv],M[nv1]);
		}
	}*/

}

template <class Graph_t>
leda::ugraph& Ldraw<Graph_t>::get_graph(){
	return  m_g;
}



template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(){
////////////////
// Draws the graph and waits for editing
	if(m_g.empty()){
		cout<<"Trying to draw an empty graph"<<endl;
		return;
	}

	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	m_g.make_directed();		//For drawing
	GW.set_graph(m_g);
	
	//circular layout
	node_array<double> x(m_g);
	node_array<double> y(m_g);
	Ldraw::embed_circular(m_g,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	GW.display();
	GW.edit();

	//Restores graph
	m_g.make_undirected();
}

template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(const list<node>& ln , bool snapshot){
/////////////////
// shows only subgraph ln  passed

	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	m_g.make_directed();		//For drawing
	GW.set_graph(m_g);

	//show only list of nodes
	node v;
	forall_nodes (v,GW.get_graph()){
		if(ln.search(v)==0)
			GW.get_graph().hide_node(v);
	}
		
	
	//circular layout
	node_array<double> x(m_g);
	node_array<double> y(m_g);
	Ldraw::embed_circular(m_g,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	m_g.make_undirected();

}

template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(const typename Graph_t::bb_type& bb_subgraph , bool snapshot){
/////////////////////////////
// draws subgraph only
	list<node> LN;
	node v;
	forall_nodes(v,m_g){
		int id=m_g.index(v);		/*0 based*/
		if(bb_subgraph.is_bit(id))
			LN.push(v);
	}
	
	draw_circular_layout(LN);

}

template <class Graph_t>
void Ldraw<Graph_t>::draw_circular_layout(const int user_labels [], label_pos_t pos, bool snapshot){
	if(m_g.empty()){
		cout<<"Trying to draw an empty graph"<<endl;
		return;
	}

	GraphWin GW;
	GW.set_node_label_type(user_label);
	GW.set_edge_direction(undirected_edge,true);
		switch(pos){
	case UP:
			 GW.set_node_label_pos(gw_position::north_pos);
		break;
	case DOWN:
			 GW.set_node_label_pos(gw_position::south_pos);
		break;
	case LEFT:
			 GW.set_node_label_pos(gw_position::west_pos);
		break;
	case RIGHT:
			 GW.set_node_label_pos(gw_position::east_pos);
		break;
	case UPRIGHT:
			 GW.set_node_label_pos(gw_position::northeast_pos);
		break;
	case UPLEFT:
			 GW.set_node_label_pos(gw_position::northwest_pos);
		break;
	case DOWNRIGHT:
			 GW.set_node_label_pos(gw_position::southeast_pos);
		break;
	case DOWNLEFT:
			 GW.set_node_label_pos(gw_position::southwest_pos);
		break;
	case CENTER:
		GW.set_node_label_pos(gw_position::central_pos);
		break;
	default:
		cout<<"incorrect position of labels"<<endl;
		return;
	}

	m_g.make_directed();		//For drawing
	GW.set_graph(m_g);
	node v;
	std::stringstream sstr;

	int id=0;
	forall_nodes(v,m_g){
		sstr.str("");
		//sstr<<m_g.index(v)<<":"<<user_labels[id++];
		sstr<<user_labels[id++];
		GW.set_label(v,sstr.str().c_str());
	}
		
	//circular layout
	node_array<double> x(m_g);
	node_array<double> y(m_g);
	Ldraw::embed_circular(m_g,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	m_g.make_undirected();
}

template <class Graph_t>
void Ldraw<Graph_t>::paint_circular_layout(const int color_label[] /* 1 based*/, bool snapshot){
//////////////
// 
// Draws layout and colors vertices accordingly

// OBSERVACIONES
// 1-The size of color_label should be the size of the graph
// 2-Unlabelled vertices should contain a value <> INFINITE_NUM

	if(m_g.empty()){
		cout<<"Trying to draw an empty graph"<<endl;
		return;
	}
	
	//Dibuja
	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	m_g.make_directed();		//For drawing
	GW.set_graph(m_g);

	//Paints nodes
	int id=0;
	node v;
	forall_nodes(v,m_g){
		if(color_label[m_g.index(v)]>=1 && color_label[m_g.index(v)]!=INFINITE_NUM )
			GW.set_color(v,color_label[m_g.index(v)]+2);		/* 0 white, 1 is black and both are avoided*/
	}
		
	//circular layout
	node_array<double> x(m_g);
	node_array<double> y(m_g);
	Ldraw::embed_circular(m_g,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	m_g.make_undirected();
}

template <class Graph_t>
void Ldraw<Graph_t>::paint_circular_layout(const typename  Graph_t::bb_type & bb_subgraph, const int color_label [] /* 1 based */,int threshold, bool snapshot){
/////////////////////
// draw subgraph and paints color labels>threshoold and not INFINITE_NUM

	
	if(m_g.empty()){
		cout<<"Trying to draw an empty graph"<<endl;
		return;
	}

	//generate list of valid nodes
	list<node> LN;	
	node v;
	forall_nodes(v,m_g){
		int id=m_g.index(v);		/*0 based*/
		if(bb_subgraph.is_bit(id))
			LN.push(v);
	}
		
	
	//Dibuja
	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	m_g.make_directed();		//For drawing
	GW.set_graph(m_g);

	//show only list of nodes
	forall_nodes (v,GW.get_graph()){
		if(LN.search(v)==0)
			GW.get_graph().hide_node(v);
	}


	//Paints nodes not hidden
	int id=0;
	forall_nodes(v,m_g){
		if(color_label[m_g.index(v)]>=threshold && color_label[m_g.index(v)]!=INFINITE_NUM)
			GW.set_color(v,color_label[m_g.index(v)]+2);		/* 0 white, 1 is black and both are avoided*/
	}
		
	//circular layout
	node_array<double> x(m_g);
	node_array<double> y(m_g);
	Ldraw::embed_circular(m_g,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	m_g.make_undirected();
		
}


template <class Graph_t>
void Ldraw<Graph_t>::paint_circular_layout(const std::vector<int>& color_label /* 1 based*/, bool snapshot){
////////////////
// 
// Draws layout and colors vertices accordingly

// OBSERVACIONES
// 1-The size of color_label must be the size of the graph
// 2-Unlabelled vertices should contain a value != INFINITE_NUM

	if(m_g.empty()){
		cout<<"Trying to draw an empty graph"<<endl;
		return;
	}
	
	//Dibuja
	GraphWin GW;
	GW.set_edge_direction(undirected_edge,true);
	m_g.make_directed();		//For drawing
	GW.set_graph(m_g);

	//Paints nodes
	int id=0;
	node v;
	forall_nodes(v,m_g){
		if(color_label[m_g.index(v)]>=1 && color_label[m_g.index(v)]!=INFINITE_NUM)
			GW.set_color(v,color_label[m_g.index(v)]+2);		/* 0 white, 1 is black and both are avoided*/
	}
		
	//circular layout
	node_array<double> x(m_g);
	node_array<double> y(m_g);
	Ldraw::embed_circular(m_g,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw::nSnap++;
	}

	//Restores graph
	m_g.make_undirected();
}

template <class Graph_t>
int Ldraw<Graph_t>::number_of_nodes(){
	return (m_g.number_of_nodes());
}

template <class Graph_t>
int Ldraw<Graph_t>::number_of_edges(){
	return (m_g.number_of_edges());
}

template <class Graph_t>
int Ldraw<Graph_t>::copy_graph_to_bb(Graph_t& g){
//////////////
// Copies leda graph to g
//
	int n=m_g.number_of_nodes();
	int m=m_g.number_of_edges();
	if(n==0){
		cout<<"Tratando de copiar un grafo leda vacio"<<endl;
		return -1;
	}
	g.init(n);

	node_array<int> N_ID(m_g);
	node v;
	int i=0;
	forall_nodes(v,m_g){
		N_ID[v]=i++;
	}

	edge e;
	forall_edges(e,m_g){
		node t=m_g.target(e);
		node s=m_g.source(e);
		g.set_undirected_edge(N_ID[s], N_ID[t]);
	}

	//Check (optional)
	if(g.number_of_vertices()!=n || g.number_of_edges_ug()!=m){
		cout<<"Error en la conversion"<<endl;
		g.init(n);
		return -1;
	}

return 0; //OK
}





