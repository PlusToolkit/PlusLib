#ifndef __LIBBMODE_H
#define __LIBBMODE_H

#if _MSC_VER < 1600
#include "stdint.h"   // Needed for int16_t, int32_t etc
#else
#include <stdint.h>    // Defined in Visual Studio 2010
#endif


/* 
          TYPES
 */


/*! Define the dynamic range algorithms. Depending on the type different values for min and max will be calculated */
typedef enum _BmodeDRCAlgEnum {BMODE_DRC_SQRT = 0}  TBmodeDRCAlg;


/*! \struct TBModeParams
 * \brief All parameters for dynamic range compression collected in a data structure
 */
typedef struct _BModeParams{
	//! Externally supplied
	TBmodeDRCAlg alg;       //!< Choose the type of DRC compression
	float dyn_range;        //!< Dynamic range  given in  [dB]
	float offset;           //!< Offset for dynamic range given in [dB]

	int n_samples;          //!< Number of samples in the input signal
	int n_lines;            //!< Number of lines in the input signal

	//! Internally calculated
	int len;                //!< Length in the input signal. Equal or less to n_samples*n_lines

	float max;             //!< Maximum value after compression, before scaling
	float min;             //!< Minimum value after compression, before removing the offset
	float scale;           //!< Scaling coefficient to fit the range compressed signal in the range [0 .. 255]

} TBModeParams;


/*
      FUNCTIONS
 */

#ifdef __cplusplus
extern "C"{
#endif

void bmode_detect_compress_sqrt_16sc_8u(const int16_t* iq, uint8_t* env, TBModeParams* par);
int  bmode_set_params_sqrt(TBModeParams* params);

#ifdef __cplusplus
};
#endif

#endif
