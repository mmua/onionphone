/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/***********************************************************************
Copyright (c) 2006-2010, Skype Limited. All rights reserved. 
Redistribution and use in source and binary forms, with or without 
modification, (subject to the limitations in the disclaimer below) 
are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.
- Neither the name of Skype Limited, nor the names of specific 
contributors, may be used to endorse or promote products derived from 
this software without specific prior written permission.
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED 
BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#include <ophtools.h>

#include "SKP_Silk_main_FIX.h"

/* Compute weighted quantization errors for an LPC_order element input vector, over one codebook stage */
void SKP_Silk_NLSF_VQ_sum_error_FIX(int32_t * err_Q20,	/* O    Weighted quantization errors  [N*K]         */
				    const int *in_Q15,	/* I    Input vectors to be quantized [N*LPC_order] */
				    const int *w_Q6,	/* I    Weighting vectors             [N*LPC_order] */
				    const int16_t * pCB_Q15,	/* I    Codebook vectors              [K*LPC_order] */
				    const int N,	/* I    Number of input vectors                     */
				    const int K,	/* I    Number of codebook vectors                  */
				    const int LPC_order	/* I    Number of LPCs                              */
    )
{
	int i, n, m;
	int32_t diff_Q15, sum_error, Wtmp_Q6;
	int32_t Wcpy_Q6[MAX_LPC_ORDER / 2];
	const int16_t *cb_vec_Q15;

	assert(LPC_order <= 16);
	assert((LPC_order & 1) == 0);

	memzero(Wcpy_Q6, (MAX_LPC_ORDER / 2) * sizeof(int32_t));

	/* Copy to local stack and pack two weights per int32 */
	for (m = 0; m < SKP_RSHIFT(LPC_order, 1); m++) {
		Wcpy_Q6[m] =
		    w_Q6[2 * m] | SKP_LSHIFT((int32_t) w_Q6[2 * m + 1], 16);
	}

	/* Loop over input vectors */
	for (n = 0; n < N; n++) {
		/* Loop over codebook */
		cb_vec_Q15 = pCB_Q15;
		for (i = 0; i < K; i++) {
			sum_error = 0;
			for (m = 0; m < LPC_order; m += 2) {
				/* Get two weights packed in an int32 */
				Wtmp_Q6 = Wcpy_Q6[SKP_RSHIFT(m, 1)];

				/* Compute weighted squared quantization error for index m */
				diff_Q15 = in_Q15[m] - *cb_vec_Q15++;	// range: [ -32767 : 32767 ]
				sum_error =
				    SKP_SMLAWB(sum_error,
					       SKP_SMULBB(diff_Q15, diff_Q15),
					       Wtmp_Q6);

				/* Compute weighted squared quantization error for index m + 1 */
				diff_Q15 = in_Q15[m + 1] - *cb_vec_Q15++;	// range: [ -32767 : 32767 ]
				sum_error =
				    SKP_SMLAWT(sum_error,
					       SKP_SMULBB(diff_Q15, diff_Q15),
					       Wtmp_Q6);
			}
			assert(sum_error >= 0);
			err_Q20[i] = sum_error;
		}
		err_Q20 += K;
		in_Q15 += LPC_order;
	}
}
