
#include "libbmode.h"

#include <xmmintrin.h>
#include <emmintrin.h>
#include <math.h>
#include <string.h>
#include <assert.h>

/*!\fn  int bmode_set_params_sqrt(TBModeParams* params)
 * \brief Modify the fields (data members) of params. Calculate internal params from external params.
 * \param params - A structure with input and output parameters. 
 *                 Important fields are:
 *                      .offset - Lower limit of the input values in dB (0 to 80)
 *                      .dyn_range - Dynamic range of the output in dB  (0 to 96)
 *                      .n_lines - Number of lines
 *                      .n_samples - Number of samples
 *
 * \returns 0 - Wrong function for parameter control, 1 right function for parameter control.
 * \returns params - Modifies the fields in params:
 *                   .offset is limited in 0 to 80
 *                   .dyn_range is limited so that .offset + .dyn_range < 96
 *                   .max and .min are found 
 *                              .max = sqrt(sqrt(10^(offset+dyn_range)/20))
 *                              .min = sqrt(sqrt(10^(offset)/20))
 *                   .scale is found so that env will be in the range 0..255
 *                   .len is calculated so that it is the nearest multiple of 16, 
 *                       that is less or equal to n_samples * n_lines
 */
int  bmode_set_params_sqrt(TBModeParams* params)
{

	// We will set the parameters even if the wrong function has been called.

	// Limit the offset between 0 and 80 dB
	params->offset = (params->offset < 80)?params->offset: 80;
	params->offset = (params->offset > 0)?params->offset: 0;

	// Limit dynamic range so that the sum does not exceed 96 dB
	params->dyn_range  = (params->dyn_range>0)? params->dyn_range:-params->dyn_range;
	params->dyn_range  = (params->dyn_range + params->offset < 96)? params->dyn_range : 96 - params->dyn_range;

	
	/*
		Calculate the internal values 
	 */

	params->max = powf(10, (params->offset+params->dyn_range)/10/2);
	params->min = powf(10, params->offset/10/2);

	params->max = sqrtf(sqrtf(params->max));
	params->min = sqrtf(sqrtf(params->min));
	params->scale = 255.0f/(params->max-params->min);

	params->len = (int)floor(params->n_lines*params->n_samples/16.0)*16;  // Less than or equal to the whole buffer


	if (params->alg != BMODE_DRC_SQRT){   
		return 0;	// Wrong function has been called. Report failure
	}else{
		return 1;    // Right function has been called. Report success
	}
}




/*!\fn void bmode_detect_compress_sqrt_16sc_8u(int16_t* iq, uint8_t* env, TBModeParams* par)
 * \brief Performs envelope detection and dynamic range compression
 * \param iq - Iinput samples. Each sample is a pair of 2 16-bit values. Real part followed by imaginary part.
 * \param env - Output. Detected, compressed in range, scaled and converted to 8-bit unisgned values
 * \param par - A structure with parameters for the compression. Used fields:
 *            .max - Maximum value for env
 *            .min - Minimum value for env
 *            .scale - Scaling factor to convert the values [0 ... (max-min)] to  [0 .. 255]
 *            .len - Length of iq and of env, in number of samples
 *
 * \note
 *    iq and env must be allocated on a boundary of 16-bytes (_aligned_malloc(, 16)).
 *
 *
 *   The function detects the envelope:
 *              e(n) = sqrt(i[n]^2+q[n]^2). 
 *              The result e(n) is converted to 32-bit float
 *
 *  Then it compresses the envelope
 *              o(n) = sqrt(sqrt(e(n)))
 *
 *  Then it limits o(n) in the limits .max and .min
 *            o(n) = min(max(o(n), .min), .max);
 *
 *  Then it subtracts the lower limit 
 *            o(n) = o(n) - .min
 *
 *  Then it scales the result
 *            env(n) = o(n) * .scale
 *
 *   The implementation uses SSE2 commants. Each iteration produces 16 values of env
 *   Each reading operation reads 4 input complex samples. Therefore there are 4 read operations 
 *  inside the for loop
 *
 * \note
 *  The function is documented in SDD25234
 *
 */

void bmode_detect_compress_sqrt_16sc_8u(const int16_t* iq, uint8_t* env, TBModeParams* par)
{
	int n;
	__m128i* psrc;
	__m128i* pdest;

	__m128 mmax;          // 4 component vector with the value max
	__m128 mmin;          // 4 component vector with the value min

	__m128 mscale;        // 4 component vector with the value scale
	__m128i  m0, m1, m2, m3, m4;         // Temporary register to hold IQ samples
	__m128   a;                          // Temporary register to hold floating point result

	mmax = _mm_set_ps1(par->max);
    mmin = _mm_set_ps1(par->min);
	mscale = _mm_set_ps1(par->scale);
	
    for (n = 0,  psrc = (__m128i*)iq,  pdest = (__m128i*)env; n < par->len; n+= 16, pdest++){
		/*
		    Process samples 0 to 3
		 */
		m0 = _mm_load_si128(psrc);             /* Load 4 samples I0, Q0, I1, Q1, I2, Q2, I3, Q3 */
		psrc++;                                /* Point to next location                     */
		

		m1 = m0;  
		m1 = _mm_madd_epi16(m0, m1);            /* R0 = I0*I0 + Q0*Q0, R1 = I1*I1 + Q1*Q1 ... */
		a = _mm_cvtepi32_ps(m1);                /* Convert to floating point                  */
        a = _mm_sqrt_ps(a);                     /* Now we have the envelope                   */

		a = _mm_sqrt_ps(a);                    /* Compression by taking 2 times the envelope  */
		a = _mm_sqrt_ps(a);

        a = _mm_min_ps(a, mmax);               /* Limit the result                            */
        a = _mm_max_ps(a, mmin);                 
		a = _mm_sub_ps(a, mmin);

		a = _mm_mul_ps(a, mscale);            /* a contains samples 0 to 3                    */
		                                      /* The values of a are scaled 0 to 255          */

		m2 =  _mm_cvttps_epi32(a);            /*Convert to integer. This like m2 = int(floor(a)) */

		/*
		   Process samples 4 to 7
		 */


		m0 = _mm_load_si128(psrc);             /* Load 4 samples and point to next location  */
		psrc++;                                /* Point to next location                     */


		m1 = m0;  
		m1 = _mm_madd_epi16(m0, m1);            /* R0 = I0*I0 + Q0*Q0, R1 = I1*I1 + Q1*Q1 ... */
		a = _mm_cvtepi32_ps(m1);                /* Convert to floating point                  */
        a = _mm_sqrt_ps(a);                     /* Now we have the envelope                   */

		a = _mm_sqrt_ps(a);                    /* Compression by taking 2 times the envelope  */
		a = _mm_sqrt_ps(a);

        a = _mm_min_ps(a, mmax);               /* Limit the result                            */
        a = _mm_max_ps(a, mmin);                 
		a = _mm_sub_ps(a, mmin);

		a = _mm_mul_ps(a, mscale);            /* a contains samples 0 to 3                    */
		                                      /* The values of a are scaled 0 to 255          */
		m3 = _mm_cvttps_epi32(a); 

		m4 = _mm_packs_epi32(m2, m3);         /* Pack to 16-bit values */
		
		/* m2 and m3 are free at this step. m4 contains samples 0 to 7.  */


		/*
		   Process samples 8 to 11
		 */
		m0 = _mm_load_si128(psrc);             /* Load 4 samples I0, Q0, I1, Q1, I2, Q2, I3, Q3 */
		psrc++;                                /* Point to next location                     */
		

		m1 = m0;  
		m1 = _mm_madd_epi16(m0, m1);            /* R0 = I0*I0 + Q0*Q0, R1 = I1*I1 + Q1*Q1 ... */
		a = _mm_cvtepi32_ps(m1);                /* Convert to floating point                  */
        a = _mm_sqrt_ps(a);                     /* Now we have the envelope                   */

		a = _mm_sqrt_ps(a);                    /* Compression by taking 2 times the envelope  */
		a = _mm_sqrt_ps(a);

        a = _mm_min_ps(a, mmax);               /* Limit the result                            */
        a = _mm_max_ps(a, mmin);                 
		a = _mm_sub_ps(a, mmin);

		a = _mm_mul_ps(a, mscale);            /* a contains samples 0 to 3                    */
		                                      /* The values of a are scaled 0 to 255          */

		m2 =  _mm_cvttps_epi32(a);            /*Convert to integer. This like m2 = int(floor(a)) */

		/* 
		   Process samples 12 to 15

		 */
		m0 = _mm_load_si128(psrc);             /* Load 4 samples and point to next location  */
		psrc++;                                /* Point to next location                     */


		m1 = m0;  
		m1 = _mm_madd_epi16(m0, m1);            /* R0 = I0*I0 + Q0*Q0, R1 = I1*I1 + Q1*Q1 ... */
		a = _mm_cvtepi32_ps(m1);                /* Convert to floating point                  */
        a = _mm_sqrt_ps(a);                     /* Now we have the envelope                   */

		a = _mm_sqrt_ps(a);                    /* Compression by taking 2 times the envelope  */
		a = _mm_sqrt_ps(a);

        a = _mm_min_ps(a, mmax);               /* Limit the result                            */
        a = _mm_max_ps(a, mmin);                 
		a = _mm_sub_ps(a, mmin);

		a = _mm_mul_ps(a, mscale);            /* a contains samples 0 to 3                    */
		                                      /* The values of a are scaled 0 to 255          */
		m3 = _mm_cvttps_epi32(a); 

		m0 = _mm_packs_epi32(m2, m3);         /* Pack to 16-bit values */
		/* m0 contains samples 8 to 15 */

		m1 = _mm_packus_epi16 (m4, m0);

		_mm_store_si128 (pdest, m1); 
						
	}
}

