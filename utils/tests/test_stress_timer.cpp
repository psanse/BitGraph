//stress test for precision timer: uses omp (do not forget to set flag)

#include "gtest/gtest.h"
#include "../prec_timer.h"
#include <math.h>

using namespace std;

TEST(PrecTimer, wall_cpu_times){
	PrecisionTimer pt;
	//timestamp
	cout<<PrecisionTimer::local_timestamp();

	pt.wall_tic();
	pt.cpu_tic();
	
    //some parallel computation 
    double sum = 0;
#pragma omp parallel for reduction(+ : sum)
    for (long long i = 1; i < 1000000000LL; i++){
        sum += log((double)i);
    }

    //meauser time
	cout << "Wall Time = " << pt.wall_toc()<< endl;
	cout << "CPU Time  = " << pt.cpu_toc()  << endl;

    //  Prevent Code Elimination
    cout << endl;
    cout << "Sum = " << sum << endl;

	//timestamp
	cout<<PrecisionTimer::local_timestamp();

}