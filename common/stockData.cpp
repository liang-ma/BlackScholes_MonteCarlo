/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
*----------------------------------------------------------------------------
*/

#include "stockData.h"
#ifdef __CLIANG__
#include <iostream>
using namespace std;
#endif
stockData::stockData(data_t timeT, data_t freeRate, data_t volatility,
		data_t initPrice,data_t strikePrice)
{
	this->freeRate=freeRate;
	this->initPrice=initPrice;
	this->timeT=timeT;
	this->volatility=volatility;
	this->strikePrice=strikePrice;
}

stockData::stockData(const stockData& data)
{
	this->freeRate=data.freeRate;
	this->initPrice=data.initPrice;
	this->timeT=data.timeT;
	this->volatility=data.volatility;
	this->strikePrice=data.strikePrice;
}
#ifdef __CLIANG__
void stockData::print()const
{
	cout<<"timeT:"<<timeT<<' '
			<<"freeRate:"<<freeRate<<' '
			<<"volatility:"<<volatility<<' '
			<<"initPrice:"<<initPrice<<' '
			<<"strikePrice:"<<strikePrice<<endl;
}
#endif
