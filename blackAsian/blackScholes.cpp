/*----------------------------------------------------------------------------
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* 	MAX_NUM_RNG is the number of RNG objects implemented
*   MAX_SAMPLE 	is the number of repeating of a group of simulation, has to be an even number
*   TIME_PAR	is the number of time steps/partitions
*
* init_array(), a static member function of RNG is defined here due to the dimension of the array.
*
* The algorithm in sampleSIM() is optimized in mathematical point of view.
*
*----------------------------------------------------------------------------
*/
#include "blackScholes.h"
#define 	MAX_NUM_RNG 	2
#define 	MAX_SAMPLE 		2
#define 	TIME_PAR		128


const int blackScholes::NUM_SHARE=512;
void blackScholes::groupSIM(RNG* mt_rng, data_t *pCall, data_t *pPut)
{

	const data_t Dt=data.timeT/data_t(TIME_PAR);

	const data_t
			ratio1=expf(-data.freeRate*data.timeT),
			ratio2=(data.freeRate-data.volatility*data.volatility*0.5)*Dt,
			ratio3=Dt*data.volatility*data.volatility,
			initValue=logf(data.initPrice/TIME_PAR/1.0f)	;

	data_t call=0,put=0;

	data_t sum_call[MAX_NUM_RNG], sum_put[MAX_NUM_RNG];
#pragma HLS ARRAY_PARTITION variable=sum_call complete dim=1
#pragma HLS ARRAY_PARTITION variable=sum_put complete dim=1

	data_t sum_call1[MAX_NUM_RNG], sum_put1[MAX_NUM_RNG];
#pragma HLS ARRAY_PARTITION variable=sum_call1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=sum_put1 complete dim=1

	data_t acc1[MAX_NUM_RNG][NUM_SHARE],acc[MAX_NUM_RNG][NUM_SHARE];
#pragma HLS ARRAY_PARTITION variable=acc1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=acc complete dim=1

	data_t ave1[MAX_NUM_RNG][NUM_SHARE],ave[MAX_NUM_RNG][NUM_SHARE];
#pragma HLS ARRAY_PARTITION variable=ave1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ave complete dim=1
	data_t num1[MAX_NUM_RNG][NUM_SHARE],num2[MAX_NUM_RNG][NUM_SHARE];
#pragma HLS ARRAY_PARTITION variable=num2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=num1 complete dim=1

	loop_set:for(int k=0;k<NUM_SHARE;k++)
	{
#pragma HLS PIPELINE
		loop_set1:for(int i=0;i<MAX_NUM_RNG;i++)
		{
	#pragma HLS UNROLL
			if(k==0)
			{
				sum_call[i]=0;
				sum_put[i]=0;
				sum_call1[i]=0;
				sum_put1[i]=0;
			}
			acc1[i][k]=initValue;
			ave1[i][k]=-data.strikePrice;
			acc[i][k]=initValue;
			ave[i][k]=-data.strikePrice;
		}
	}
	loop_main:for(int s=0;s<MAX_SAMPLE;s+=2)
	{
		loop_sim:for(int p=0;p<TIME_PAR;p++)
		{
			loop_share:for(int k=0;k<NUM_SHARE;k++)
			{
	#pragma HLS PIPELINE
				loop_parallel:for(int i=0;i<MAX_NUM_RNG;i++)
				{
					mt_rng[i].BOX_MULLER(&num1[i][k],&num2[i][k],ratio3);

					acc1[i][k]+=ratio2+num1[i][k];
					ave1[i][k]+=expf(acc1[i][k]);

					acc[i][k]+=ratio2+num2[i][k];
					ave[i][k]+=expf(acc[i][k]);
				}
			}
		}
		loop_sumShare:for(int k=0;k<NUM_SHARE;k++)
		{
			loop_sum:for(int i=0;i<MAX_NUM_RNG;i++)
			{
#pragma HLS UNROLL
			//	std::cout<<"i="<<i<<" k="<<k<<" ave="<<ave[i][k]<<" ave1="<<ave1[i][k]<<std::endl;
				if(ave[i][k]>0)
					sum_call[i]+=ave[i][k];
				else
					sum_put[i]+=ave[i][k];
				if(ave1[i][k]>0)
					sum_call1[i]+=ave1[i][k];
				else
					sum_put1[i]+=ave1[i][k];
				acc1[i][k]=initValue;
				ave1[i][k]=-data.strikePrice;
				acc[i][k]=initValue;
				ave[i][k]=-data.strikePrice;
			}
		}
	}
	loop_sum_all:for(uint i=0;i<MAX_NUM_RNG;i++)
	{
			call+=sum_call[i]+sum_call1[i];
			put+=sum_put[i]+sum_put1[i];
	}
	*pCall=ratio1*call/(NUM_SHARE*MAX_NUM_RNG*MAX_SAMPLE*1.0f);
	*pPut =-ratio1*put/(NUM_SHARE*MAX_NUM_RNG*MAX_SAMPLE*1.0f);
}

void blackScholes::simulation(data_t *call, data_t *put)
{

	RNG mt_rng[MAX_NUM_RNG];
#pragma HLS ARRAY_PARTITION variable=mt_rng complete dim=1

	uint seeds[MAX_NUM_RNG];
#pragma HLS ARRAY_PARTITION variable=seeds complete dim=1

	loop_seed:for(int i=0;i<MAX_NUM_RNG;i++)
	{
#pragma HLS UNROLL
		seeds[i]=i;
	}
	RNG::init_array(mt_rng,seeds);
	groupSIM(mt_rng,call,put);

}

blackScholes::blackScholes(stockData data):data(data)
{
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
