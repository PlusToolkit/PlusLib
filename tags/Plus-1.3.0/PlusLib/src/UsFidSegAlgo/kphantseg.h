/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _KPHANTSEG_H
#define _KPHANTSEG_H

#include <vector>

#include "SegImpl.h"


class KPhantomSeg
{

public:

	

	/* Constructor: initialize the segmentation structure with image
	 * dimenstions.  It can then be used to segment a sequence of images. See
	 * below for descriptions of parameters.
	 * DebugOutput: if it is set to true, then intermediate results 
	   are written into files.
	 */
	KPhantomSeg(int sizeX, int sizeY, 
			int searchOriginX, int searchOriginY, 
			int searchSizeX, int searchSizeY, bool debugOutput=false, std::string possibleFiducialsImageFilename="");
	
	~KPhantomSeg();
	
	/* Run the segmentation, store the results in the 
	 * output parameters described below. 
	 *
	 * data: One byte per pixel, on range 0-255. The origin of the image
	 * (0,0) is the top left corner. Increasing X moves to the right and
	 * increasng Y moves down. The returned dot locations are in this
	 * coordinate system. The data is in row major order. The offset of a
	 * pixel is computed using the formula ( posY * sizeX + posX ). */
	void segment( unsigned char *data,  const SegmentationParameters &segParams );

	/* Debugging routines: printing and drawing the results. */
	void printResults();
	void drawResults( unsigned char *data );
	void GetSegmentationResults(SegmentationResults &segResults); 
	
	static std::vector<std::vector<double>> sortInAscendingOrder(std::vector<std::vector<double>> fiducials);  
	
	


	/*
	 * Input
	 */

	/* Image size. 
	 *
	 * NOTE: hardcoding these values in morph.cpp (known there as cols and
	 * rows) will yeild a 15-20% speed increase.
	 * */
	int m_SizeX;
	int m_SizeY;

	/* Search origin and size parameters define the region within the image
	 * where we should search for the dots. Outside of this region the image
	 * data must be null. Around all sides of this region there must be at
	 * least 8 pixels of this null space (searchOrigin >= 8 and searchSize <=
	 * imageSize-16). */
	int m_SearchOriginX;
	int m_SearchOriginY;

	int m_SearchSizeX;
	int m_SearchSizeY;

	std::string m_PossibleFiducialsImageFilename; 



	/*
	 * Output
	 */

	

	//(3, vector<int>(2,0)); 
	// vector<double> m_FoundDotsYValue; 

	
	/*
	delete later
	float x1, y1;
	float x2, y2;
	float x3, y3;
	float x4, y4;
	float x5, y5;
	float x6, y6;
	// delete later ...///
	*/ 

	

	

	/*
	 * Private Data
	 */  
	
	SegmentationParameters m_SegParams; 

	SegImpl *m_SegImpl;

	SegmentationResults m_SegResult; 
};

#endif
