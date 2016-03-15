/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* This class is called stockData which stores the basic data for a given stock and option.
*
*----------------------------------------------------------------------------
*/
#ifndef __STOCKDATA_H__
#define __STOCKDATA_H__

#include "defTypes.h"

class stockData
{
public:
	data_t timeT;
	data_t freeRate;
	data_t volatility;
	data_t initPrice;
	data_t strikePrice;
	
	stockData(data_t, data_t,data_t,data_t,data_t);
	stockData(const stockData&);
	void print()const;
};

#endif
