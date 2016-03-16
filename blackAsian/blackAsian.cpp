/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Top function is defined here with interface specified as axi4.
* It creates an object of blackScholes and launches the simulation.
*
*----------------------------------------------------------------------------
*/
#include "../common/defTypes.h"
#include "../common/stockData.h"
#include "../common/blackScholes.h"



void blackAsian(data_t *call, data_t *put,   // call price and put price
		data_t timeT,				// time period of options
		data_t freeRate,			// interest rate of the riskless asset
		data_t volatility,			// volatility of the risky asset
		data_t initPrice,			// stock price at time 0
		data_t strikePrice)			// strike price
{
#pragma HLS INTERFACE m_axi port=call bundle=gmem
#pragma HLS INTERFACE s_axilite port=call bundle=control
#pragma HLS INTERFACE m_axi port=put bundle=gmem
#pragma HLS INTERFACE s_axilite port=put bundle=control
#pragma HLS INTERFACE s_axilite port=timeT bundle=gmem
#pragma HLS INTERFACE s_axilite port=timeT bundle=control
#pragma HLS INTERFACE s_axilite port=freeRate bundle=gmem
#pragma HLS INTERFACE s_axilite port=freeRate bundle=control
#pragma HLS INTERFACE s_axilite port=volatility bundle=gmem
#pragma HLS INTERFACE s_axilite port=volatility bundle=control
#pragma HLS INTERFACE s_axilite port=initPrice bundle=gmem
#pragma HLS INTERFACE s_axilite port=initPrice bundle=control
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=gmem
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	stockData sd(timeT,freeRate,volatility,initPrice,strikePrice);
	blackScholes bs(sd);
	data_t callPrice,putPrice;
	bs.simulation(&callPrice,&putPrice);
	*call=callPrice;
	*put=putPrice;
	return;

}
