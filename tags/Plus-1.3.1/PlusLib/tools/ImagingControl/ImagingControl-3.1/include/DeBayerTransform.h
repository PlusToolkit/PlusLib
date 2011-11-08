
#ifndef DEBAYERTRANSFORM_H_INC_
#define DEBAYERTRANSFORM_H_INC_

#pragma once

#include "udshl_defs.h"

namespace _DSHOWLIB_NAMESPACE
{
	class IDispEventDispatcher;
	struct DeBayerTransformImpl;
	class GrabberPImpl;

	/** The DeBayerTransform class allows you to control the automatic debayering process of IC Imaging Control. 
	 */
	class _UDSHL_EXP_API DeBayerTransform
	{
		friend GrabberPImpl;
	public:
		~DeBayerTransform();

		/** Checks whether the image data is currently debayered by IC Imaging Control. */
		bool			isActive() const;

		enum tStartPattern
		{
			eBG = 0,
			eGB,
			eGR,
			eRG,
		};

		/** Sets the pattern which determines how the algorithm interprets the pixels of the raw image data.
		 * It has to match the pattern of the first two pixels of the first line. 
		 */
		void			setStartPattern( tStartPattern pat );
		/** Returns the currently selected start pattern. */
		tStartPattern	getStartPattern() const;

		enum tDeBayerMode
		{
			eNearestColor,		///< Simplest and fastest algorithm, but lesser output image quality.
			eBilinear,			///< Algorithm with both acceptable speed and image quality.
			eEdgeSensing,		///< This algorithm creates the best image quality, but is rather slow.
		};

		/** Selects the algorithm for the debayering of the image data. */
		void			setDeBayerMode( tDeBayerMode mode );
		/** Returns the currently active debayering algorithm. */
		tDeBayerMode	getDeBayerMode() const;
	private:
		DeBayerTransform( IDispEventDispatcher& disp );

		DeBayerTransformImpl*	m_pImpl;
	};

};

#endif // DEBAYERTRANSFORM_H_INC_
