//A templatized batch class to run tests of type Alg_t with Params Param_t
//Alg_t must have a constructor for Param_t
//AlgVariant_t should derive from Alg_t (base of the hierarchy of algorithms)

#ifndef __BATCH_H__
#define __BATCH_H__

#include <vector>

using namespace std;

template <class Alg_t, class Param_t>
class Batch{
public:
	template<class AlgVariant_t>
	void add_test(Param_t p){
		tests.push_back(new AlgVariant_t(p));	
	}

	void clear(){
		for(typename vector<Alg_t*>::iterator it=tests.begin(); it!=tests.end(); it++){
			delete *it;
		}
		tests.clear();
	}

	void start(){
		for(typename vector<Alg_t*>::iterator it=tests.begin(); it!=tests.end(); it++){
			(*it)->setup();
			(*it)->run();
			(*it)->tear_down();
		}
	}
	
	~Batch(){ clear();}

	int number_of_tests() {return tests.size();}
	Alg_t* get_test(int id){ return tests[id];}

/////////
// vector
protected:
	vector<Alg_t*> tests; 
};

#endif





