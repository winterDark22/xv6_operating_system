#ifndef __DEFS__
#define __DEFS__

#define SAME_GROUP 0
#define DIFFERENT_GROUP 1
#define PRINTING 0
#define WANT_TO_PRINT 1
#define DONE_PRINTING 2


#include <cmath>
int getRand(long double mean) {
    long double L = exp(-mean);
    long double p = 1.0;
    int k = 0;
    do {
        k++;
        p *= (long double) rand() / RAND_MAX;
    } while(p > L);
    return k - 1;
}

#endif