/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* This class is called "blackScholes" which implements the BS model.
*
* simulation() is launched by top function.
* It defines and initializes some objects of RNG and then launch the simulation.
*
* sampleSIM performs the entire simulation and passes the results to top function.
*
*----------------------------------------------------------------------------
*/
#ifndef __BLACKSCHOLES__
#define __BLACKSCHOLES__

#include "defTypes.h"
#include "RNG.h"
#include "stockData.h"

class blackScholes
{
	stockData data;
	static const int NUM_SHARE;
public:
	blackScholes(stockData);
	void simulation(data_t *,data_t*);
	void sampleSIM(RNG *, data_t*,data_t*);
};

#endif
