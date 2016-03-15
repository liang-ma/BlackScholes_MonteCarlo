/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Here defines the typical data types utilized by the project.
* uint : unsigned integer, the type of the states for RNGs
* data_t : float/double or fixed point number for the simulation
*
*----------------------------------------------------------------------------
*/
#ifndef __DEFTYPES__
#define __DEFTYPES__

#ifdef __CLIANG__

#include <cmath>
#include <iostream>
using namespace std;

#else

#include "hls_math.h"
#include <ap_fixed.h>

#endif

typedef unsigned int uint;
typedef float data_t;
// This reduces the HW cost a lot, but slows down convergence too much.
// typedef ap_fixed<64,32,AP_RND,AP_SAT> data_t;


#endif
