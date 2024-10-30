//common.h: common utilities for the framework

#ifndef __COMMON_H__
#define	__COMMON_H__

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <random>
#include "logger.h"

using namespace std;

///////////////////////
// common typedefs

using vint = vector<int>;
using vdob = vector<double>;

///////////
//switches
//#define DEBUG_STACKS					//[DEF-OFF] in relese- checks stack sizes (important to debug SAT engine)

namespace com {
 
 namespace dir {

	  inline void append_slash(string & path){
		  /////////////////
		  // appends slash at the end of a path string if required taking into account SO

		  size_t pos;

		  pos=path.find_last_of("\\/");
		  if(pos==path.length()-1){
#ifdef  __GNUC__
			  if(path[pos]=='\\')
				  path.replace(pos,path.length(),"/");
#elif	_MSC_VER
			//Windows accepts both slashes so no changes are required
			/*  if(path[pos]=='/')
				  path.replace(pos,path.length(),"\\");*/
#endif
		  }else{ //no slash at the ends
#ifdef _MSC_VER
			  //default option for windows
			  path+='\\';

#elif  __GNUC__
			  path+='/';
#endif
		  }
	  }

	  inline string remove_path(string & path) {
		  size_t pos;
		  pos = path.find_last_of("\\/");
		  return path.substr(pos + 1);	  
	  }

  }

   namespace stl {

	   template<class Collection_t>
	   inline bool all_equal(const Collection_t& col){
		   if(col.empty()) return true;
		   for(auto it=col.begin()+1; it!=col.end(); ++it){
			   if(col[0]!=*it) return false;
		   }
		return true;
	   }

	   template <class Collection_t>
	   inline
	   ostream& print_collection(const Collection_t& c,  ostream&  o= cout, bool with_endl=false){
		   copy(c.cbegin(), c.cend(), ostream_iterator<typename Collection_t::value_type>(o," " ));
		   o<<" ["<<c.size()<<"]";
		   if(with_endl) o<<endl;
		   return o;
	   }
	   	   
	/*   inline
	   ostream& print_collection(const vector<vint>& c, ostream&  o = cout) {
		   o << "printing " << c.size() << " elements" << endl;
		   for (int i = 0; i < c.size(); i++) {
			   print_collection<vint>(c[i], o, true);			  
		   }
		   return o;
	   }*/

	   template <class T>
	   inline
	   ostream& print_array(T* begin, T* end,  ostream&  o = cout,  bool with_endl = false, bool with_index = false) {
		   int nC = 0;
		   for (T* pt = begin; pt != end; pt++) {
			   if (with_index) {
				   o <<"["<<nC<<"]"<< *pt << " "; nC++;
			   }
			   else {
				   o << *pt << " "; nC++;
			   }		    
		   }
		   o << " [" << nC << "]";
		   if (with_endl) o << endl;
		   return o;
	   }

   }
   
////////////////////////
//my minimal stack
   template <class T>
   struct stack_t{
	   static const int EMPTY_VAL=-1;
	   int pt;																		//stack pointer[0, MAX_SIZE-1], always points to a free position (top of the stack) 
	   T* stack;
#ifdef DEBUG_STACKS
	   int MAX;
#endif
	  	   
	   stack_t():pt(EMPTY_VAL), stack(nullptr) {
#ifdef DEBUG_STACKS
		   int MAX=EMPTY_VAL;
#endif   
	   }
	   stack_t(int MAX_SIZE):stack(nullptr){
		   init(MAX_SIZE); 
#ifdef DEBUG_STACKS
		   MAX = MAX_SIZE;
#endif
	   }
	   ~stack_t(){clear();}
	   void init(int MAX_SIZE) {
		   clear(); stack = new T[MAX_SIZE]; pt = 0;
#ifdef DEBUG_STACKS
		   MAX = MAX_SIZE;
#endif
	   }
	   void clear() {
		   if (stack) { delete[] stack; } stack = nullptr;  pt = EMPTY_VAL;
#ifdef DEBUG_STACKS
		   MAX = EMPTY_VAL;
#endif
	   }
	   
	   void push(T d){
		   stack[pt++]=d;   //*** no checking against N
#ifdef DEBUG_STACKS
		   if (pt > MAX) {
			   LOG_INFO("bizarre stack with size: " << pt << " and max size: " << MAX);
			   cin.get();
		   }
#endif
	   }												
	   void push_front(T d) {
		   if (pt == 0) { stack[pt++] = d; }
		   else { T t = stack[0]; stack[0] = d; stack[pt++] = t; }
#ifdef DEBUG_STACKS
		   if (pt > MAX) {    //*** no checking against N
			   LOG_INFO("bizarre stack with size: " << pt << " and max size: " << MAX);
			   cin.get();
		   }
#endif
	   }

	   T pop(){if(pt==0) return T(); else return stack[--pt];}
	   int pop_first() { if (pt <= 1) return (pt = 0); else { stack[0] = stack[--pt]; return pt; }} /* removes the first element from the stack (lost!), swaps last elememt */
	   int pop(int nb){pt-=nb; return nb;}											/* removes the last NB elements from the stack (lost!) */
	   T get_elem(int pos) const {return stack[pos];}
	   T first() const {return stack[0];}											//TODO@assert correct size
	   T last() const {return stack[pt-1];}											//TODO@assert correct size
	   void erase(int pos){if(pt>0){stack[pos]=stack[--pt]; stack[pt]=EMPTY_VAL;}}	//removes a single element at pos (holds when deleting singleton pos=0, pt=1)	
	   void erase(){pt=0;}															//clears the stack
	  // void erase_pop(int nb){pt-=nb;}											//clears nb elements from the stack-TODO, REFACTOR
	   unsigned int size(){return pt;}
	   bool empty() const{return(pt==0);}											// no elements in the stack (allocated space)
	 
	   //TODO@operator ==


	   ostream& print(ostream& o) const{
		    o<<"[";   for(int i=0; i<pt; i++) {o<<stack[i]<<" ";} o<<"]"<<"["<<pt<<"]"<<endl;
			return o;
	   }
	   friend ostream& operator <<(ostream& o, const stack_t & s){ 
		   o<<"[";   for(int i=0; i<s.pt; i++) {o<<s.stack[i]<<" ";} o<<"]"<<"["<<s.pt<<"]"<<endl;
			return o;
	   }
   };
	 
	//////////////////
	//specializations of stack_t
    template<>
	inline
    int stack_t<int>::pop(){if(pt==0) return EMPTY_VAL; else return stack[--pt];}

	///////////////////////////////////

///////////////////////
//my silly pointer to any type 
//(type requires a print method)

	template<class T>
	class pt_elem{
		T* pt;
	public:
		pt_elem(){ pt=new T;}
		pt_elem(T& data){ pt=new T(data);}
		~pt_elem(){delete pt;}

		const T* get_elem() const {return pt;}			
		T* get_elem()  {return pt;}	
		void set_elem(const T& data){ *pt = data;}							//deep copy
		void swap(pt_elem& out){T* temp=pt; pt=out.pt; out.pt=temp;}
		ostream& print_elem(ostream& o=std::cout){pt->print(o); return o;}
	};


///////////////////////
//my old-array type

	template<class T>
	class my_array {
		T* pt;
		size_t sz;
	public:
		my_array(size_t size, T val):sz(size) { pt = new T [sz];  for (int i = 0; i < sz; i++) { pt[i] = val; } }
		~my_array() { delete[] pt; }
		T& operator [](int idx) {return pt[idx];}
		const T& operator [](int idx) const { return pt[idx]; }
		T at(int idx) { return pt[idx]; }
		T* get_array() { return pt; };
		size_t size() { return sz; }
	};

/////////////////
//FUNCTORS
		
   //functors which use a global external critera which evaluates to a number
   template<class data_t, class crit_t>
   struct has_greater_val:public binary_function<data_t, crit_t, bool>{
	   has_greater_val(const vector<crit_t>& out):crit(out){}
	  
	   bool operator()(const data_t &a, const data_t &b) const{
		   return (crit[a]>crit[b]);
	   }
	   const vector<crit_t>& crit;
   };
      
   template<class data_t, class crit_t>
   struct has_greater_val<data_t*, crit_t>:public binary_function<data_t*, crit_t, bool>{
	   has_greater_val(const vector<crit_t>& out):crit(out){}
	  
	   bool operator()(const data_t *a, const data_t *b) const{
		   return (crit[*a]>crit[*b]);
	   }
	   const vector<crit_t>& crit;
   };

     template<class data_t, class crit_t>
   struct has_greater_val_prod:public binary_function<data_t, crit_t, bool>{
	   has_greater_val_prod(const vector<crit_t>& out):crit(out){}
	  
	   bool operator()(const data_t &a, const data_t &b) const{
		   return (crit[a]*a>crit[b]*b);
	   }
	   const vector<crit_t>& crit;
   };

    template<class data_t, class crit_t>
   struct has_greater_val_diff:public binary_function<data_t, crit_t, bool>{
	   has_greater_val_diff(const vector<crit_t>& out):crit(out){}
	   bool operator()(const data_t &a, const data_t &b) const{
		   return ((crit[a]-a) > (crit[b]-b));
	   }
	   const vector<crit_t>& crit;
   };

    template<class data_t, class crit_t>
   struct has_smaller_val:public binary_function<data_t, crit_t, bool>{
	   has_smaller_val(const vector<crit_t>& out):crit(out){}
	   bool operator()(const data_t &a, const data_t &b) const{
		   return (crit[a]<crit[b]);
	   }
	   const vector<crit_t>& crit;
   }; 

    template<class data_t, class crit_t>
   struct has_smaller_val<data_t*, crit_t>:public binary_function<data_t*, crit_t, bool>{
	   has_smaller_val(const vector<crit_t>& out):crit(out){}
	   bool operator()(const data_t* &a, const data_t* &b) const{
		   return (crit[*a]<crit[*b]);
	   }
	   const vector<crit_t>& crit;
   }; 

    template<class data_t, class crit_t>
   struct has_smaller_val_prod:public binary_function<data_t, crit_t, bool>{
	   has_smaller_val_prod(const vector<crit_t>& out):crit(out){}
	   bool operator()(const data_t &a, const data_t &b) const{
		   return (crit[a]*a < crit[b]*b);
	   }
	   const vector<crit_t>& crit;
   };

    template<class data_t, class crit_t>
   struct has_smaller_val_diff:public binary_function<data_t, crit_t, bool>{
	   has_smaller_val_diff(const vector<crit_t>& out):crit(out){}
	   bool operator()(const data_t &a, const data_t &b) const{
		   return ((crit[a]-a) < (crit[b]-b));
	   }
	   const vector<crit_t>& crit;
   };


   //functors which use two (1.main, 2.tiebreak) global external critera which evaluates to a number
   template<class data_t, class crit_t>
   struct has_smaller_val_with_tb {
	   has_smaller_val_with_tb(const vector<crit_t>& ref, const vector<crit_t>& tb) :crit1(ref), crit2(tb) {}

	   bool operator()(const data_t &a, const data_t &b) const {

		   if (crit1[a] < crit1[b]) return true;
		   if (crit1[a] > crit1[b]) return false;

		   return (crit2[a] < crit2[b]);
		   			 		 
	   }
	   const vector<crit_t>& crit1;
	   const vector<crit_t>& crit2;		/* tiebreak */
   };


   template<class data_t, class crit_t>
   struct has_greater_val_with_tb {
	   has_greater_val_with_tb(const vector<crit_t>& ref, const vector<crit_t>& tb) :crit1(ref), crit2(tb) {}

	   bool operator()(const data_t &a, const data_t &b) const {

		   if (crit1[a] > crit1[b]) return true;
		   if (crit1[a] < crit1[b]) return false;

		   return (crit2[a] > crit2[b]);		  	     
	   }
	   const vector<crit_t>& crit1;
	   const vector<crit_t>& crit2;		/* tiebreak */
   };


    //functors for collections of elements 
   template<class T>
   struct has_bigger_size{
		bool operator()(const vector<T>& lhs,  const vector<T>& rhs) const  { return lhs.size() > rhs.size(); } 
	};

    template<class T>
   struct has_smaller_size{
		bool operator()(const vector<T>& lhs,  const vector<T>& rhs) const  { return lhs.size() < rhs.size(); } 
	};
   

   namespace mat{

	 ///////////////////
	//
	// class RandomUniformGen
	//
	//////////////////


	   template<	typename D = std::uniform_int_distribution<int>,
		   typename RE = std::mt19937						>		//std::default_random_engine in this machine
		   class RandomUniformGen {
		   public:
			   using result_type = typename D::result_type;
			   using dist_type = D;
			   using rgen_type = RE;

			   constexpr static unsigned int FIXED_RANDOM_SEED = 10000;		//default seed for randomness

			   result_type a() { return dist_.param().a(); }
			   result_type b() { return dist_.param().b(); }

			   static void set_range(result_type min, result_type max) {
				   dist_.param(D::param_type{ min, max });
			   }

			   static void seed(std::size_t seed) {							//external seed
				   seed_ = seed;
				   re_.seed(seed_);
			   }
			   static void seed() {											//internal seed- 
				   std::random_device rd;

				   ///////////////////////////////////////
				   if (rd.entropy())											//checks if the source of randomness in HW is valid
					   seed_ = rd();
				   else
					   seed_ = static_cast<decltype(seed_)>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
				   /////////////////////////////////////////

				   re_.seed(seed_);
			   }

			   result_type operator()() {
				   return dist_(re_);
			   }

			   result_type operator()(result_type min, result_type max) {
				   return dist_(re_, D::param_type{ min, max });
			   }

			   //////////////////////////////////////////////////////////////////
			   RandomUniformGen() = default;
			   RandomUniformGen(const RandomUniformGen&) = delete;
			   RandomUniformGen& operator = (const RandomUniformGen&) = delete;
			   //////////////////////////////////////////////////////////////////
		   private:
			   static RE re_;										//random gen
			   static D dist_;										//uniform distribution
			   static std::size_t seed_;							//default seed
	   };

	   ////////////////////////////////////////////////////////////////
	   template<typename D = std::uniform_int_distribution<int>,
		   typename RE = std::mt19937							>
		   using ugen = RandomUniformGen< D, RE >;

	   using iugen = RandomUniformGen< std::uniform_int_distribution<int>, std::mt19937 >;
	   using rugen = RandomUniformGen< std::uniform_real_distribution<double>, std::mt19937 >;
	   ////////////////////////////////////////////////////////////////

	   template<typename D, typename RE> RE RandomUniformGen<D, RE>::re_;
	   template<typename D, typename RE> D RandomUniformGen<D, RE>::dist_;
	   template<typename D, typename RE>
	   std::size_t RandomUniformGen<D, RE>::seed_ = RandomUniformGen<D, RE>::FIXED_RANDOM_SEED;


	   inline
	   bool uniform_dist(double p){
		   //returns true with prob p, 0 with 1-p
		  /* double n_01=rand()/(double)RAND_MAX;
		   return (n_01<=p);*/

		   /////////////////////////////
		   rugen r;
		   return r(0.0, 1.0) <= p;
		    ////////////////////////////
	   }
	   template<typename T>
	   inline 
	   T min3(T x, T y, T z){
		   return std::min<T>(std::min<T>(x, y),z);
	   }
//////////////////////

	   /* Sorting functions: Fabio Furini (20/07/17)

	   Sorting of n items (integer) according to non-increasing values of their scores (double)
	   */
	   /********************************************************************************************************************************/
	   inline
	   void SORT_NON_INCR(int *item,double *score,int n)
		   /********************************************************************************************************************************/
	   {
		   int salto,i,j,tempItem;
		   double tempScore;

		   for (salto = n/2; salto > 0; salto /=2)
			   for (i = salto; i < n; i++)
				   for (j = i-salto; j >= 0; j-=salto) {
					   if (score[j] >= score[j+salto]) break;
					   tempScore = score[j]; score[j]=score[j+salto]; score[j+salto]=tempScore;
					   tempItem = item[j]; item[j]=item[j+salto]; item[j+salto]=tempItem;
				   }
	   }

	   /* Sorting function:

	   Sorting of n items (integer) according to non-increasing values of their scores (double)
	   */
	   /********************************************************************************************************************************/
	   inline
	   void SORT_NON_DECR(int *item, double *score,int n)
		   /********************************************************************************************************************************/
	   {
		   int salto,i,j,tempItem;
		   double tempScore;

		   for (salto = n/2; salto > 0; salto /=2)
			   for (i = salto; i < n; i++)
				   for (j = i-salto; j >= 0; j-=salto) {
					   if (score[j] <= score[j+salto]) break;
					   tempScore = score[j]; score[j]=score[j+salto]; score[j+salto]=tempScore;
					   tempItem = item[j]; item[j]=item[j+salto]; item[j+salto]=tempItem;
				   }
	   }
	   
	    /*
	   Insert an element into n-SORTED items (type T) according to non-decreasing values of their scores (type T)
	   After insertion, there are n sorted items
	   (returns the position of insertion)
	   */
	   /********************************************************************************************************************************/
	   template<class T>
	   inline
	   int INSERT_ORDERED_SORT_NON_INCR(T* item, T* score, int n, T target, T target_val){		
		 /********************************************************************************************************************************/ 
		   
		   int i=n-1;			
		   for(;i>=1; i--){
			  // if( target_val>score[item[i-1]] ){
			   if( target_val>score[i-1] ){
				   item[i] = item[i-1];
				   //score[item[i]]=score[item[i-1]];	
				   score[i]=score[i-1];		
			   }else break;
		   }
		   item[i] = target;				/* note target can be written in item[n-1] */
		   //score[item[i]]=target_val;
		   score[i]=target_val;
		   return i;		
	   }
   }


   namespace fileproc{
		  
	   inline
	   int READ_EMPTY_LINES(fstream& f){
		/* experimental */
			   char line[250], c;
			   do{
				   c=f.peek();
				   if(c=='\n'){
					   f.getline(line, 250);
					   continue;		
				   }
				   break;
			   }while(true);

			   return 0;
	   }

	   inline
	   int READ_SET_OF_INTERDICTED_NODES(const char* filename, vector<int>& interdicted_nodes) {
		   /*reads a line of 0s and 1s and provides the position of the 0s  */

		   std::ifstream f(filename, ios::binary | ios::in);
		   if (!f) {
			   LOG_ERROR("Could not open file: " << filename);
			   return -1;
		   }
		   interdicted_nodes.clear();
		   int index = 0;
		   char c;
		   while (f.get(c)) {
			   if (c == '0') { interdicted_nodes.push_back(index); }
			   index++;
		   }

		   f.close();
		   return 0;
	   }

	   template<class T>
	   inline
		   int WRITE_SET_OF_NODES(const char* filename, vector<T>& nodes, bool plus_one=true) {
		   //writes a set of elements to file in the format:
		   //size <SIZE>
		   //elem1
		  //elem2 ...
		   std::ofstream f(filename, std::ofstream::out);
		   if (!f) {
			   LOG_ERROR("Could not open file: " << filename);
			   return -1;
		   }
		   int SIZE = nodes.size();
		   f << "size " << SIZE << endl;
		   for (int i = 0; i < SIZE; i++) {
			   f << ((plus_one)? nodes[i]+1 : nodes[i]) << endl;
		   }
		   f.close();
		   return 0;
	   }	   
   }

    namespace counting{
		inline
		int count_words(string str){
			int word_count( 0 );
			stringstream ss(str);
			string word;
			while( ss >> word ) ++word_count;
			return word_count;

		}
	}
}
	


#endif