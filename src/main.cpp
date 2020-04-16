#include <iostream>
#include "ap_int.h"
#include "utils/x_hls_utils.h"
#include<stdint.h>
#include "hls_stream.h"
#include "ap_axi_sdata.h"

void randomGenerator(uint16_t *randomRet)
{
#pragma HLS INLINE off
    uint16_t start_state = 0xACE1u;  /* Any nonzero start state will work. */
    static uint16_t lfsr = start_state;
    uint16_t bit;                    /* Must be 16-bit to allow bit<<15 later in the code */
    uint16_t period = 0;

//    do
//    {   /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
        bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) /* & 1u */;
        lfsr = (lfsr >> 1) | (bit << 15);
//        ++period;
//    }
//    while (lfsr != start_state);

    *randomRet = lfsr;
}

void eventSimulator(hls::stream< ap_uint<16> > &xStreamOut,
		hls::stream< ap_uint<16> > &yStreamOut,
		hls::stream< ap_uint<64> > &tsStreamOut,
		hls::stream< ap_uint<1> > &polStreamOut,
		uint8_t *deltaTsReg)
{
#pragma HLS INTERFACE axis register both port=tsStreamOut
#pragma HLS INTERFACE axis register both port=polStreamOut
#pragma HLS INTERFACE axis register both port=yStreamOut
#pragma HLS INTERFACE axis register both port=xStreamOut

	static ap_uint<16> x, y;
	static ap_uint<1> pol;
	static ap_uint<64> ts;

	uint16_t retData;
	randomGenerator(&retData);
	uint8_t deltaTs = retData;
	ts += retData;

	if(x == 200)
	{
		x = 0;
		if(y == 200)
		{
			y = 0;
		}
		else
		{
			y++;
		}
	}
	else
	{
		x++;
	}

	xStreamOut << x;
	yStreamOut << y;
	tsStreamOut << ts;
	polStreamOut << pol;

	*deltaTsReg = deltaTs;
}
