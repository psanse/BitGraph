//ldraw.h a wrapper to draw simple undirected graphs for the LEDA library
//
//NOTES: LEDA uses its own vector and string type so theres is collision with STL namespace for these (and other)types
////////////////////////////////////////

#ifndef __LEDA_DRAW_H__
#define __LEDA_DRAW_H__

#include <iostream>
#include <vector>
#include <LEDA/graph/ugraph.h>
#include <LEDA/graphics/graphwin.h>

template <class Graph_t>
class Ldraw{
public:
	enum label_pos_t {UP=0, DOWN, LEFT, RIGHT, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT, CENTER};
public:
	static int nSnap;
	static void reset_snapshots(){nSnap=0;}
	static void draw_circular_layout(leda::ugraph & G, bool snapshot=false);
	static void draw_circular_layout(leda::ugraph & G, const leda::list<leda::node>&, bool snapshot=false);
	static void draw_circular_layout(const Graph_t& gbb, bool snapshot=false);
	static void draw_circular_layout(const Graph_t& gbb, const typename Graph_t::bb_type& subgraph, bool snapshot=false);
	static void draw_circular_layout_and_edit(Graph_t& gbb, bool snapshot=false);
	static void draw_circular_layout(leda::ugraph & G, int user_lables [] , label_pos_t=UP,  bool snapshot=false);
	static int copy_graph_leda_to_bb (const leda::ugraph & G, Graph_t& gbb);
	static int copy_graph_bb_to_leda (const Graph_t& gbb,  leda::ugraph & G);

	static void graph_win(bool is_undirected=true);			//launches empty graphwin

private:
	static void embed_circular(const leda::graph& G, leda::node_array<double>& x, leda::node_array<double>& y); 

public:
	Ldraw(void){clear();}
	Ldraw(const Graph_t& g){set_graph(g);}
	Ldraw(const leda::ugraph& g){set_graph(g);}
	Ldraw(std::string filename){set_graph(filename);}
	~Ldraw(void){}
	
	void set_graph(std::string filename);					//Dimacs only								
	void set_graph(const leda::ugraph& g);
	void set_graph(const Graph_t& g);
	void set_graph(const Graph_t& g, const typename Graph_t::bb_type& bbN);
	leda::ugraph& get_graph();
	int number_of_nodes();
	int number_of_edges();
	void clear();
	int copy_graph_to_bb(Graph_t& g);
	void draw_circular_layout();
	void draw_circular_layout(const leda::list<leda::node>& , bool snapshot=false);
	void draw_circular_layout(const typename Graph_t::bb_type& , bool snapshot=false);
	void draw_circular_layout(const int user_labels [], label_pos_t=UP, bool snapshot=false);
	void paint_circular_layout(const int color_labels [] /* 1 based */, bool snapshot=false);										//color labels!= INFINITE_NUM  will not be painted
	void paint_circular_layout(const typename Graph_t::bb_type&, const int color_labels [] /* 1 based */,int threshold=1, bool snapshot=false);		//color labels>threshold and !=INIFNITE_NUM will not be painted
	void paint_circular_layout(const std::vector<int>& color_labels /* 1 based */, bool snapshot=false);							//color labels!= INFINITE_NUM  will not be painted
	//*** paint induced subgraph
private:
	leda::ugraph m_g;
};

//must be in the header if it is modified elsewhere in the header file
template <class Graph_t>
int Ldraw<Graph_t>::nSnap=0;

template <class Graph_t>
inline
void Ldraw<Graph_t>::clear(){
///////////
// Empties the graph
	m_g.clear();
	//n_id.clear();
}

template <class Graph_t>
inline
void Ldraw<Graph_t>::draw_circular_layout_and_edit(Graph_t& gbb, bool snapshot){
////////////////////
// Draws and updates changes done in the GUI
	leda::ugraph g;
	copy_graph_bb_to_leda(gbb,g);
	draw_circular_layout(g);
	copy_graph_leda_to_bb(g,gbb);
}

template <class Graph_t>
inline
int Ldraw<Graph_t>::copy_graph_bb_to_leda (const Graph_t& gbb, leda::ugraph & G){
////////////////
// Sets graphBB in leda undirected graph
	
	G.clear();
	int n=gbb.number_of_vertices();
	leda::node v;
	leda::map<int,leda::node> n_id;
	for(int i=0; i<n; i++){
		v=G.new_node();
		n_id[i]=v;
	}

	//only undirected eges
	for(int i=0; i<n-1; i++)
		for(int j=i+1; j<n; j++){
			if(gbb.is_edge(i,j))
				G.new_edge(n_id[i],n_id[j]);
		}

return 0;
}

template <class Graph_t>
inline
int  Ldraw<Graph_t>::copy_graph_leda_to_bb (const leda::ugraph & G, Graph_t& gbb) {
//////////////
// Copies leda undirected graph to bitstring graph
//
	int n=G.number_of_nodes();
	int m=G.number_of_edges();
	if(n==0){
		cout<<"Tratando de copiar un grafo leda vacio"<<endl;
		return -1;
	}
	gbb.init(n);

	leda::node_array<int> N_ID(G);
	leda::node v;
	int i=0;
	forall_nodes(v,G){
		N_ID[v]=i++;
	}

	leda::edge e;
	forall_edges(e,G){
		leda::node t=G.target(e);
		leda::node s=G.source(e);
		gbb.add_edge(N_ID[s], N_ID[t]);
	}

	//Check (optional)
	if(gbb.number_of_vertices()!=n || gbb.number_of_edges()!=m){
		cout<<"Error en la conversion"<<endl;
		gbb.init(n);
		return -1;
	}

return 0; //O
 }

template <class Graph_t>
inline
void Ldraw<Graph_t>::draw_circular_layout( leda::ugraph& G, bool snapshot){
/////////////////
// Draws the current graphm which is subjeto to modifications in  the GUI
//
// OBSERVATIONS
// 1.Functionality to percieve vertex orderings

	leda::GraphWin GW;
	GW.set_edge_direction(leda::undirected_edge,true);
	G.make_directed();		//For drawing
	GW.set_graph(G);
	
	//circular layout
	leda::node_array<double> x(G);
	leda::node_array<double> y(G);
	Ldraw::embed_circular(G,x,y);
	GW.set_layout(x,y);
	GW.place_into_win();
	
	
	GW.display();
	GW.edit();

	//Place this always after edition instruction
	if(snapshot){
		std::stringstream sstr;
		sstr<<"snapshot_"<<Ldraw<Graph_t>::nSnap;
		GW.get_window().screenshot(sstr.str().c_str());
		Ldraw<Graph_t>::nSnap++;
	}

	//Restores graph
	G.make_undirected();
}


template <class Graph_t>
inline
void Ldraw<Graph_t>::embed_circular(const leda::graph& G, leda::node_array<double>& x, leda::node_array<double>& y){ 
/////////////////////////
// LEDA (AGD on request) for random layouts (5/4/12)
//
// OBSERVACIONES: Creates a random layout with radios 0.5 and center (0.5, 0.5)

  int n = G.number_of_nodes();
  if (n == 0){
	  cout<<"Empty graph-no layout"<<endl;
	  return;
  }
  leda::point  m = leda::point(0.5,0.5);
  double alpha = 0;
  double step  = 6.2832/n;
  leda::node v;
  forall_nodes(v,G){
	leda::point p = m.translate_by_angle(alpha,0.5);
    x[v] = p.xcoord();
    y[v] = p.ycoord();
    alpha+=step;
  }
}


#endif