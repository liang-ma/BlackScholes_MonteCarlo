/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* 	MAX_NUM_RNG is the number of RNG objects implemented
*   MAX_SAMPLE 	is the number of repeating of a group of simulation, has to be an even number
*   TIME_PAR	is the number of time steps/partitions, 1 in this sample.
*
* init_array(), a static member function of RNG is defined here due to the dimension of the array.
*
* The algorithm in sampleSIM() is optimized in mathematical point of view.
*
*----------------------------------------------------------------------------
*/
#include "blackScholes.h"
#define 	MAX_NUM_RNG 	8
#define 	MAX_SAMPLE 		4
#define 	TIME_PAR		1



const int blackScholes::NUM_SHARE=512;
blackScholes::blackScholes(stockData data):data(data)
{
}

void blackScholes::sampleSIM(RNG* mt_rng, data_t* pCall, data_t *pPut)
{
	data_t
		ratio1=data.strikePrice*expf(-data.freeRate*data.timeT),
		ratio3=data.volatility*data.volatility*data.timeT,
		ratio2=(data.freeRate-0.5f*data.volatility*data.volatility)*data.timeT,
		ratio4=ratio2-logf(data.strikePrice/data.initPrice);

	data_t sumCall=0,sumPut=0;
	data_t pCall1[MAX_NUM_RNG][NUM_SHARE],pCall2[MAX_NUM_RNG][NUM_SHARE];
#pragma HLS ARRAY_PARTITION variable=pCall1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=pCall2 complete dim=1
	data_t pPut1[MAX_NUM_RNG][NUM_SHARE],pPut2[MAX_NUM_RNG][NUM_SHARE];
#pragma HLS ARRAY_PARTITION variable=pPut1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=pPut2 complete dim=1

	data_t num1[MAX_NUM_RNG][NUM_SHARE],num2[MAX_NUM_RNG][NUM_SHARE];
#pragma HLS ARRAY_PARTITION variable=num2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=num1 complete dim=1


	loop_init:for(int k=0;k<NUM_SHARE;k++)
	{
#pragma HLS PIPELINE
		loop_r:for(int i =0;i<MAX_NUM_RNG;i++)
		{
#pragma HLS UNROLL
			pCall1[i][k]=0;
			pCall2[i][k]=0;
			pPut1[i][k]=0;
			pPut2[i][k]=0;
		}
	}
	loop_main:for(int j=0;j<MAX_SAMPLE;j+=2)
	{
		loop_share:for(uint k=0;k<NUM_SHARE;k++)
		{
#pragma HLS PIPELINE
			loop_parallel:for(int i=0;i<MAX_NUM_RNG;i++)
			{
#pragma HLS UNROLL
				mt_rng[i].BOX_MULLER(&num1[i][k],&num2[i][k],ratio4,ratio3);
				if(num1[i][k]>0)
					pCall1[i][k]+=expf(num1[i][k])-1;
				else
					pPut1[i][k]+=1-expf(num1[i][k]);
				if(num2[i][k]>0)
					pCall2[i][k]+=expf(num2[i][k])-1;
				else
					pPut2[i][k]+=1-expf(num2[i][k]);
			}
		}
	}

	loop_sumCall:for(int k=0;k<NUM_SHARE;k++)
	{
		loop_sr:for(int i =0;i<MAX_NUM_RNG;i++)
		{
#pragma HLS UNROLL
			sumCall+=pCall1[i][k]+pCall2[i][k];
			sumPut+=pPut1[i][k]+pPut2[i][k];
		}
	}
	*pCall=ratio1*sumCall/MAX_NUM_RNG/MAX_SAMPLE/NUM_SHARE;
	*pPut =ratio1*sumPut/MAX_NUM_RNG/MAX_SAMPLE/NUM_SHARE;
}

void blackScholes::simulation(data_t *pCall, data_t *pPut)
{

	RNG mt_rng[MAX_NUM_RNG];
#pragma HLS ARRAY_PARTITION variable=mt_rng complete dim=1

	uint seeds[MAX_NUM_RNG];

	loop_seed:for(int i=0;i<MAX_NUM_RNG;i++)
	{
#pragma HLS UNROLL
		seeds[i]=i;
	}
	RNG::init_array(mt_rng,seeds);
	sampleSIM(mt_rng,pCall,pPut);

}

void RNG::init_array(RNG* rng, uint* seed)
{
	uint tmp[MAX_NUM_RNG];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

	loop_set_seed:for(int i=0;i<MAX_NUM_RNG;i++)
	{
#pragma HLS UNROLL
		rng[i].index = 0;
		rng[i].seed=seed[i];
		tmp[i]=seed[i];
	}


	loop_seed_init:for (int i = 0; i < RNG_H; i++)
	{
		loop_group_init:for(int k=0;k<MAX_NUM_RNG;k++)
		{
#pragma HLS UNROLL
			rng[k].mt_e[i]=tmp[k];
			tmp[k]= RNG_F*(tmp[k]^ (tmp[k] >> (RNG_W - 2))) + i*2+1;
			rng[k].mt_o[i]=tmp[k];
			tmp[k]= RNG_F*(tmp[k]^ (tmp[k] >> (RNG_W - 2))) + i*2+2;
		}
	}
}
