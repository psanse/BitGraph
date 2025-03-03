/*
* @file graph_inout.h
* @brief stateless functions for I/O operations on graphs
* @created 10/07/2019
* @last_update 27/01/25
* @author pss
* 
* @comment generic code for graph types in the GRAPH lib
* 
* TODO@: this code contains only yED format - possibly change name of file (30/12/2022)
*/

#ifndef	__GRAPH_INPUT_OUTPUT_H__
#define __GRAPH_INPUT_OUTPUT_H__

#include <fstream>
#include <sstream>
#include <iostream>
#include <array>

#include "utils/file.h"
#include "utils/logger.h"
#include "utils/common.h"
//#include graph/graph.h"


#define  _USE_MATH_DEFINES				//for PI
#include <math.h>

using vint = vector<int>;

namespace gio {
	
	namespace yed {

		/**********************************************
		*
		*	 Utilities to write in gml format (yED) 
		*    (experimental)
		*
		************************************************/

		constexpr int MAX_COL_RGB_GML = 23;
		//constexpr std::array<const char*, 23> table_RGB = { "#990000", "#ff0000", "#ff6666",  "#999900", "#ffff00", "#ffff66",  "#009900", "#00ff00", "#66ff66", "#004C99", "#0080ff", "#66b2ff",
		//												    "#990099", "#ff00ff", "#ff66ff",  "#404040", "#808080", "#c0c0c0" , "#009999", "#00ffff", "#66ffff"  "#ffcc00", "#000000",  "#ffffff" };
		//enum col_t {
		//	DARK_RED, RED, LIGHT_RED, DARK_YEL, YEL, LIGHT_YEL, DARK_GREEN, GREEN, LIGHT_GREEN, DARK_BLUE, BLUE, LIGHT_BLUE, DARK_PURPLE, PURPLE, LIGHT_PURPLE,
		//	DARK_GREY, GREY, LIGHT_GREY, DARK_CYAN, CYAN, LIGHT_CYAN, DEFAULT /* yellow of yEd */, BLACK, WHITE																					};

		constexpr std::array<const char*, MAX_COL_RGB_GML> table_RGB = { "#FF0000", "#FFFF00", "#008000",  "#00FFFF", "#0000FF", "#FF00FF", "#800080", "#800000", "#FF7F50",  "#FA8072", "#FFA500", "#FFD700",   "#FFFF00",  "#008B8B",  "#40E0D0",
			"#00008B",  "#8A2BE2",  "#FFC0CB", "#F5DEB3", "	#A0522D",	"#ffcc00", "#000000",  "#ffffff" };
		enum col_t {
			 RED=0, YELLOW, GREEN, CYAN, BLUE, MAGENTA, PURPLE, BROWN, CORAL, SALMON, ORANGE, GOLD, LIME, DARK_CYAN,  TURQUOISE, DARK_BLUE, BLUE_VIOLET,
			  PINK, WHEAT, SIENNA,  DEFAULT /* yellow of yEd */, BLACK, WHITE												};
		enum edge_t {
			DASHED, NONE
		};

		inline
		std::ostream& add_vertex(std::ostream& gml, int i, double x = 0, double y = 0, double scale = 7.5, col_t COL = DEFAULT, double w = 11.0, double h = 11.0, string type = "roundrectangle", int font_size = 7) {
			///////////////////////////
			// param@i: id, label:= i+1

			gml << "node  [ "
				<< "id " << i
				<< " graphics  [ x " << x * scale
				<< " y " << y * scale
				<< " w " << w
				<< " h " << h
				<< " type \"roundrectangle\" fill \"" << table_RGB[COL]
				<< "\"]"
				<< " LabelGraphics  [ "
				<< " text " << i + 1
				<< " fontSize " << font_size
				<< " ] "
				<< " ]  " << endl;
			return gml;
		}

		inline
		std::ostream& add_edge(std::ostream& gml, int i, int j, col_t COL = BLACK, edge_t E_STYLE= NONE) {
			gml << " edge   [ "
				<< " source  " << i
				<< " target " << j
				<< " graphics  [ "
				<< "  fill \"" << table_RGB[COL] << "\"";
			
			//edge style
			switch (E_STYLE) {
			case NONE:
				break;
			case DASHED:
				gml << " style \"dashed\"";
				break;
			default:
				LOG_ERROR("error edge style - gio::yed::add_edge");
			}
			
			gml	<< " ]"
				<< " ]" << endl;
			return gml;
		}

		inline
		std::ostream& HEADER(std::ostream& gml) {
			gml << "graph  [ hierarchic  1  directed  1 \n\n" << endl;
			return gml;
		}

		inline
		std::ostream& CLOSE_HEADER(std::ostream& gml) {
			gml << "\n] \n\n" << endl;
			return gml;
		}

		/*******************
		*
		* graph_to_gml
		*
		*********************/
		template<class Graph_t>
		inline
			int graph_to_gml(Graph_t& g, std::string filename, double scale = 7.5, int flag_edges=1) {
			const unsigned int N = g.number_of_vertices();
			if (N == 0) { LOG_INFO("cannot paint the empty graph, will produce no output -gio::yed::graph_to_gml"); return 0; }
			std::string filenameExt = filename + ".gml";
			std::ofstream grafo(filenameExt);
			if (!grafo) { LOGG_ERROR("file: " , filenameExt , "could no be opened" , " -gio::yed::graph_to_gml(...)"); return -1; }

			//////////////////////////////////
			HEADER(grafo);
			//////////////////////////////////

			///////////////
			//vertices
			for (int i = 0; i < N; ++i) {
				add_vertex(grafo, i, 0, 0, scale, gio::yed::DEFAULT);
			}

			//////////////////
			//edges
			if (flag_edges == 1) {
				for (int i = 0; i < N - 1; ++i) {
					for (int j = i + 1; j < N; ++j) {
						if (g.is_edge(i, j)) {
							add_edge(grafo, i, j, gio::yed::BLACK);
						}
					}
				}
			}

			//////////////////////////////////
			CLOSE_HEADER(grafo);
			//////////////////////////////////		

			grafo.close();
			return 0;
		}

		/*******************
		*
		* graph_to_gml
		* (including coloring a subset of vertices with different color)
		*
		*********************/
		template<class Graph_t>
		inline
			int graph_to_gml(Graph_t& g, vint& vset, std::string filename, col_t col = gio::yed::GREEN, double scale = 7.5, int flag_edges = 1, std::string path="") {
			const int N = g.number_of_vertices();
			if (N == 0) { LOG_INFO("cannot paint the empty graph, will produce no output- gio::yed::graph_to_gml"); return 0; }
			std::string filenameExt =path + filename + ".gml";
			std::ofstream grafo(filenameExt);
			if (!grafo) { LOGG_ERROR("file: " , filenameExt , "could no be opened" , " -graph_to_gml(...)"); return -1; }
			typename Graph_t::_bbt bbclq(N, vset);
			//bbclq.print(cout, true);


			//////////////////////////////////
			HEADER(grafo);
			//////////////////////////////////

			///////////////
			//vertices
			for (int i = 0; i < N; ++i) {
				if (bbclq.is_bit(i)) {
					add_vertex(grafo, i, 0, 0, scale, col);
				}
				else {
					add_vertex(grafo, i, 0, 0, scale, gio::yed::DEFAULT);
				}
			}

			//////////////////
			//edges
			if (flag_edges == 1) {
				for (int i = 0; i < N - 1; ++i) {
					for (int j = i + 1; j < N; ++j) {
						if (g.is_edge(i, j)) {
							add_edge(grafo, i, j, gio::yed::BLACK);
						}
					}
				}
			}

			//////////////////////////////////
			CLOSE_HEADER(grafo);
			//////////////////////////////////		

			grafo.close();
			return 0;
		}


		/*******************
		*
		* graph_to_gml_clique
		*
		*********************/
		template<class Graph_t>
		inline
			int graph_to_gml_clique(Graph_t& g, vint& clq, std::string filename, col_t col_clq = gio::yed::GREEN, col_t col = gio::yed::DEFAULT, double scale = 7.5, int flag_edges = 1, std::string path = "") {
			const int N = g.number_of_vertices();
			if (N == 0) { LOG_INFO("cannot paint the empty graph, will produce no output -gio::yed::graph_to_gml_clique"); return 0; }
			std::string filenameExt = path + filename + ".gml";
			std::ofstream grafo(filenameExt);
			if (!grafo) { LOGG_ERROR("file: " , filenameExt , " could no be opened" , " -gio::yed::graph_to_gml_clique(...)"); return -1; }
			typename Graph_t::_bbt bbclq(N, clq);
			//bbclq.print(cout, true);


			//////////////////////////////////
			HEADER(grafo);
			//////////////////////////////////

			///////////////
			//vertices
			for (int i = 0; i < N; ++i) {
				if (bbclq.is_bit(i)) {
					add_vertex(grafo, i, 0, 0, scale, col_clq);
				}
				else {
					add_vertex(grafo, i, 0, 0, scale, col);
				}
			}

			//////////////////
			//edges
			if (flag_edges == 1) {
				for (int i = 0; i < N - 1; ++i) {
					for (int j = i + 1; j < N; ++j) {
						if (g.is_edge(i, j)) {
							if (bbclq.is_bit(i) && bbclq.is_bit(j)) {
								add_edge(grafo, i, j, gio::yed::GREEN, gio::yed::DASHED);
							}
							else {
								add_edge(grafo, i, j, gio::yed::BLACK);
							}
						}
					}
				}
			}

			//////////////////////////////////
			CLOSE_HEADER(grafo);
			//////////////////////////////////		

			grafo.close();
			return 0;
		}

		
		/*******************
		*
		* graph_to_gml_color
		*
		*********************/
		template<class Graph_t>
		inline int graph_to_gml_color(Graph_t& g, vector<vint>& isets, std::string filename, double scale = 7.5, int flag_edges = 1) {
			const int N = g.number_of_vertices();
			if (N == 0) { LOG_INFO("cannot paint the empty graph, will produce no output -gio::yed::graph_to_gml_color"); return 0; }
			std::string filenameExt = filename + ".gml";
			std::ofstream grafo(filenameExt);
			if (!grafo) { 
				LOGG_ERROR("file: " , filenameExt , "could no be opened" , " -gio::yed::graph_to_gml_color(...)");
				return -1;
			}
			

			////////////////////////
			vint isets_aux(N, -1);
			for (int c = 0; c < isets.size(); c++) {
				for (int i = 0; i < isets[c].size(); i++) {
					isets_aux[isets[c][i]] = c;
				}
			}
			LOG_INFO("******************");
			com::stl::print_collection(isets_aux, cout, true);
			LOG_INFO("******************");
			///////////////////////


			//////////////////////////////////
			HEADER(grafo);
			//////////////////////////////////

			///////////////
			//vertices
			for (int i = 0; i < N; ++i) {
				add_vertex(grafo, i, 0, 0, scale, (gio::yed::col_t)isets_aux[i]);
			}

			//////////////////
			//edges
			if (flag_edges == 1) {
				for (int i = 0; i < N - 1; ++i) {
					for (int j = i + 1; j < N; ++j) {
						if (g.is_edge(i, j)) {
							add_edge(grafo, i, j, gio::yed::BLACK);			
						}
					}
				}
			}

			//////////////////////////////////
			CLOSE_HEADER(grafo);
			//////////////////////////////////		

			grafo.close();
			return 0;
		}


		/*******************
		*
		* graph_to_gml_layered
		* (each layer is painted with a different color)
		*
		* param@layers: partition of independent sets, separated by -1 markers
		********************/
		template<class Graph_t>
		inline
			int graph_to_gml_layered(Graph_t& g, vector<int>& layers, string filename, double scale = 20, int flag_edges=1) {
			auto N = g.number_of_vertices();
			string filenameExt = filename + ".gml";
			ofstream grafo(filenameExt);
			if (!grafo) { LOGG_ERROR("file: " , filenameExt , "could no be opened" , "--gio::yed::graph_to_gml_layered(...)"); return -1; }

													   
			///////////////////////////////
			gio::yed::HEADER(grafo);
			//////////////////////////////
			
			auto i = 0, col = 0 /* RED */;
			auto vx = 0.0, vy = 0.0;
			for (auto v : layers) {
				if (v == EMPTY_ELEM) {
					vx = 0.0;
					vy+=2;
					col ++;
					continue;
				}
				///////////////////////////////////////////////////////////
				gio::yed::add_vertex(grafo, v, vx++, vy, scale, (gio::yed::col_t)col);
				///////////////////////////////////////////////////////////
				i++;
			}
			
			if (i != N) {
				LOGG_ERROR("not all vertices are in the layers, FAILURE" , "--gio::yed::graph_layered_to_gml");
				gio::yed::CLOSE_HEADER(grafo);
				return -1;
			}

			if (col > gio::yed::MAX_COL_RGB_GML) {
				LOGG_ERROR("too many colors:" , col , "layered graph not created--gio::yed::graph_to_gml_layered");
				return -1;
			}

			//add edges
			if (flag_edges) {
				for (int i = 0; i < N - 1; i++) {
					for (int j = i + 1; j < N; j++) {
						if (g.is_edge(i, j)) {
							///////////////////////////////////////////////
							gio::yed::add_edge(grafo, i, j);
							///////////////////////////////////////////////
						}
					}
				}				
			}
				
			/////////////////////////////////////
			gio::yed::CLOSE_HEADER(grafo);
			/////////////////////////////////////
			return 0;
		}

		/*******************
		*
		* graph_to_gml_circular
		*
		********************/
		template<class Graph_t>
		inline
		int graph_to_gml_circular(Graph_t& g, std::string filename, double radius = 2, double scale = 20, int flag_edges = 1) {
			const unsigned int N = g.number_of_vertices();
			std::string filenameExt = filename + ".gml";
			std::ofstream grafo(filenameExt);
			if (!grafo) {
				LOGG_ERROR("file: ", filenameExt, "could no be opened", "- graph_to_gml(...)");
				return -1; 
			}

			///////////////////////////////
			gio::yed::HEADER(grafo);
			//////////////////////////////

			///////////////
			//vertices
			double angle = 0, inc = (2 * M_PI) / N;		//rads
			double x, y;
			for (unsigned int i = 0; i < N; ++i) {
				x = radius * cos(angle);
				y = radius * sin(angle);
				add_vertex(grafo, i, x, y, scale, gio::yed::DEFAULT);
				angle -= inc;											//anticlockwise
			}

			//add edges
			if (flag_edges) {
				for (unsigned int i = 0; i < N - 1; i++) {
					for (unsigned int j = i + 1; j < N; j++) {
						if (g.is_edge(i, j)) {
							///////////////////////////////////////////////
							gio::yed::add_edge(grafo, i, j);
							///////////////////////////////////////////////
						}
					}
				}
			}

			/////////////////////////////////////
			gio::yed::CLOSE_HEADER(grafo);
			/////////////////////////////////////
			return 0;
		}
	}
}



#endif