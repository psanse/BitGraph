//lutils.h: my global functions, possibly remove
//added to this package: 17/3/16

#ifndef __LEDA_UTILS_H__
#define __LEDA_UTILS_H__

#include <iostream>
#include <fstream>

#ifdef LEDA_ON
#include <LEDA/graph/graph_gen.h>
#include <LEDA/graph/ugraph.h>

using namespace std;

namespace myleda{
void CreateRandomSimpleUG(leda::ugraph &G, int size, double p){
	
  int nV=size;
  int nE=p*(nV*(nV-1)/2);
  random_simple_undirected_graph(G,nV,nE);

return;
}

double DensityUG (const leda::ugraph &G){
	int nV=G.number_of_nodes();
	if(nV!=nil)
		return ((double)G.number_of_edges()/(nV*(nV-1)/2));
	else{
		cout<<"Empty graph"<<endl;
		return -1;
	}
}

int WriteDimacsUG(std::string filename, const leda::ugraph & G)
{
////////////////
// Preserves current vertex order. If grah is sorted will create an isomorphism

	if(G.empty()){
		cout<<"Writing an empty file in Dimacas format"<<endl;
		return -1;
	}
	std::ofstream f(filename.c_str());
	if(!f){
		cout<<"Error in file when writing Dimacas format"<<endl;
		return -1;
	}

	//Assigns identifiers 1-N to nodes 
	int id = 1;
	leda::node v;
	leda::node_array<int> n_id(G);
	forall_nodes(v,G){
		n_id[v] =id++;		
	}

	f<< "c LEDA undirected dimacs graph" << endl;
	f << "c" << endl;
	f << "p edge " << G.number_of_nodes() << " " <<  G.number_of_edges()<< endl;

	//runs through all edges (I guess only for undirected graphs)
	leda::edge e;
	forall_edges(e,G){
		int i = n_id[G.source(e)];
		int j = n_id[G.target(e)];
		f << "e " << i << " " << j << endl;
	}

return 0;
}

int WriteDimacsUG(std::string filename,const leda::node_array<int> n_id, const leda::ugraph & G)
{
////////////////
// Preserves current vertex order. If graph is sorted will create an isomorphism

	if(G.empty()){
		cout<<"Writing an empty file in Dimcas format"<<endl;
		return -1;
	}
	std::ofstream f(filename.c_str());
	if(!f){
		cout<<"Error in file when writing Dimcas format"<<endl;
		return -1;
	}

	f<< "c LEDA undirected dimacs graph" << endl;
	f << "c" << endl;
	f << "p edge " << G.number_of_nodes() << " " <<  G.number_of_edges()<< endl;

	//runs through all edges (I gues only for undirected graphs)
	leda::edge e;
	forall_edges(e,G){
		int i = n_id[G.source(e)];
		int j = n_id[G.target(e)];
		f << "e " << i << " " << j << endl;
	}

return 0;
}

int ReadDimacsUG(const char* filename, leda::ugraph& G){
	
	int n, m, v1, v2, edges=0;
	char token[10], line [250], c;
	leda::map<int, leda::node> M;
	std::ifstream f(filename);
	if(!f){
		cout<<"Error in file when reading Dimaca format for LEDA"<<endl;
		return -1;
	}
	G.clear();
	while(f.good()){
		f>>c;
		switch(c){
			case 'c':
				f.getline(line, 250); //remove line
				break;
			case 'p':
				//Leo el tamaño del grafo y reservo espacio en memoria
				f>>token>>n>>m;
				if(!f.good() || !strstr(token, "edge")){
					cout<<"Error al leer size y numero de aristas"<<endl;
					return -1;
				}

				//Creates n nodes and gives 1-N ids
				leda::node v;
				for(int i=1; i<=n; i++){
					v=G.new_node();
					M[i]=v;
				}
				
				break;
			case 'e':	
				f>>v1>>v2;
				if(f.good()){
					G.new_edge(M[v1],M[v2]);
					edges++;
				}
				f.getline(line, 250); //remove line
				break;
			default:
				//Capture a possible END at the end
				f.unget();
				f>>token;
				if(!strstr(token,"END")){		//Token no es "END"
					cout<<"Linea inesperada, termina lectura"<<endl;
					return -1;
				}
		}
	}

	//Check edges
	if(G.number_of_edges()!=m || G.number_of_nodes()!=n ){
		cout<<"Error. Size or number of edges read does not coincide with specification in file"<<endl;
		G.clear();
		return -1;
	}
return 0;
}
};


#endif //LEDA_ON

#endif //__LEDA_UTILS_H




