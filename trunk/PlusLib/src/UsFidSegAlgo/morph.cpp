#include <string.h>
#include <iostream>
#include "segimpl.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h" 
#include "itkMeanImageFilter.h" 
#include "itkSubtractImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkPNGImageIO.h"

using std::cout;
using std::endl;

bool shape_contains( std::vector<Item> shape, Item newItem )
{
	for ( unsigned int si = 0; si < shape.size(); si++ ) 
	{
		if ( shape[si] == newItem )
		{
			return true;
		}
	}
	return false;
}

inline PixelType min( PixelType v1, PixelType v2 )
{
	return v1 < v2 ? v1 : v2;
}

inline PixelType max( PixelType v1, PixelType v2 )
{
	return v1 > v2 ? v1 : v2;
}

inline PixelType SegImpl::erode_point_0( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = UCHAR_MAX;
	unsigned int p = ir*cols + ic - barSize; // current pixel - bar size (position of the start of the bar)
	unsigned int p_max = ir*cols + ic + barSize;// current pixel +  bar size (position of the end  of the bar)

	//find lowest intensity in bar shaped area in image
	for ( ; p <= p_max; p++ ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

int SegImpl::GetMorphologicalOpeningBarSizePx()
{
	int barsize = floor(m_SegParams.mMorphologicalOpeningBarSizeMm / m_SegParams.mScalingEstimation + 0.5 );
	return barsize; 
}

void SegImpl::erode_0( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		unsigned int ic = horzLow;
		unsigned int p_base = ir*cols;

		PixelType dval = erode_point_0( image, ir, ic ); // find lowest pixel intensity in surroudning region ( postions +/- 8 of current pixel position) 
		dest[p_base+ic] = dval; // p_base+ic = current pixel

		for ( ic++; ic < horzHigh; ic++ ) {
			PixelType new_val = image[p_base + ic + barSize];
			PixelType del_val = image[p_base + ic - 1 - barSize];

			dval = new_val <= dval ? new_val  : // dval = new val if new val is less than or equal to dval
					del_val > dval ? min(dval, new_val) : // if del val is greater than dval, dval= min of dval and new val
					erode_point_0( image, ir, ic ); //else dval = result of erode function

			dest[ir*cols+ic] = dval; // update new "eroded" picture
		}
	}
}

inline PixelType SegImpl::erode_point_45( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	PixelType dval = UCHAR_MAX;
	unsigned int p = (ir+barSize)*cols + ic-barSize;
	unsigned int p_max = (ir-barSize)*cols + ic+barSize;

	for ( ; p >= p_max; p = p - cols + 1 ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

void SegImpl::erode_45( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Down the left side. */
	for ( unsigned int sr = vertLow; sr < vertHigh; sr++ ) {
		unsigned int ir = sr;
		unsigned int ic = horzLow;

		PixelType dval = erode_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_45( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}

	/* Accross the bottom */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertHigh-1;

		PixelType dval = erode_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_45( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}
}

inline PixelType SegImpl::erode_point_90( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = UCHAR_MAX;
	unsigned int p = (ir-barSize)*cols + ic;
	unsigned int p_max = (ir+barSize)*cols + ic;

	for ( ; p <= p_max; p += cols ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}

void SegImpl::erode_90( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ic = horzLow; ic < horzHigh; ic++ ) {
		unsigned int ir = vertLow;

		PixelType dval = erode_point_90( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir++; ir < vertHigh; ir++ ) {
			PixelType new_val = image[(ir + barSize)*cols+ic];
			PixelType del_val = image[(ir - 1 - barSize)*cols+ic];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_90( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}
}

inline PixelType SegImpl::erode_point_135( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = UCHAR_MAX;
	unsigned int p = (ir-barSize)*cols + ic-barSize;
	unsigned int p_max = (ir+barSize)*cols + ic+barSize;

	for ( ; p <= p_max; p = p + cols + 1 ) {
		if ( image[p] < dval )
			dval = image[p];
		if ( image[p] == 0 )
			break;
	}
	return dval;
}


void SegImpl::erode_135( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Up the left side. */
	for ( unsigned int sr = vertHigh-1; sr >= vertLow; sr-- ) {
		unsigned int ir = sr;

		unsigned int ic = horzLow;
		PixelType dval = erode_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_135( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}

	/* Across the top. */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertLow;

		PixelType dval = erode_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval;

		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val <= dval ? new_val : 
					del_val > dval ? min(dval, new_val) :
					erode_point_135( image, ir, ic );

			dest[ir*cols+ic] = dval;
		}
	}
}

void SegImpl::erode_circle( PixelType *dest, PixelType *image )
{
	unsigned int slen = m_SegParams.mMorphologicalCircle.size();

	memset( dest, 0, rows*cols*sizeof(PixelType) );

	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		for ( unsigned int ic = horzLow; ic < horzHigh; ic++ ) {
			PixelType dval = UCHAR_MAX;
			for ( unsigned int sp = 0; sp < slen; sp++ ) {
				unsigned int sr = ir + m_SegParams.mMorphologicalCircle[sp].roff;
				unsigned int sc = ic + m_SegParams.mMorphologicalCircle[sp].coff;

				if ( image[sr*cols+sc] < dval )
					dval = image[sr*cols+sc];

				if ( image[sr*cols+sc] == 0 )
					break;
			}
			dest[ir*cols+ic] = dval;
		}
	}
}

inline PixelType SegImpl::dilate_point_0( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = ir*cols + ic - barSize;
	unsigned int p_max = ir*cols + ic + barSize;

	for ( ; p <= p_max; p++ ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

void SegImpl::dilate_0( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );

	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		unsigned int ic = horzLow;
		unsigned int p_base = ir*cols;

		PixelType dval = dilate_point_0( image, ir, ic );
		dest[ir*cols+ic] = dval;
		for ( ic++; ic < horzHigh; ic++ ) {
			PixelType new_val = image[p_base + ic + barSize];
			PixelType del_val = image[p_base + ic - 1 - barSize];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_0( image, ir, ic ));
			dest[ir*cols+ic] = dval;
		}
	}
}

inline PixelType SegImpl::dilate_point_45( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = (ir+barSize)*cols + ic-barSize;
	unsigned int p_max = (ir-barSize)*cols + ic+barSize;

	for ( ; p >= p_max; p = p - cols + 1 ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

void SegImpl::dilate_45( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Down the left side. */
	for ( unsigned int sr = vertLow; sr < vertHigh; sr++ ) {
		unsigned int ir = sr;
		unsigned int ic = horzLow;

		PixelType dval = dilate_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_45( image, ir, ic ));
			dest[ir*cols+ic] = dval ;
		}
	}

	/* Accross the bottom */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertHigh-1;

		PixelType dval = dilate_point_45( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir--, ic++; ir >= vertLow && ic < horzHigh; ir--, ic++ ) {
			PixelType new_val = image[(ir - barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir + 1 + barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_45( image, ir, ic ));
			dest[ir*cols+ic] = dval ;
		}
	}
}

inline PixelType SegImpl::dilate_point_90( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = (ir-barSize)*cols + ic;
	unsigned int p_max = (ir+barSize)*cols + ic;

	for ( ; p <= p_max; p += cols ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

void SegImpl::dilate_90( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	for ( unsigned int ic = horzLow; ic < horzHigh; ic++ ) {
		unsigned int ir = vertLow;
		PixelType dval = dilate_point_90( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir++; ir < vertHigh; ir++ ) {
			PixelType new_val = image[(ir + barSize)*cols+ic];
			PixelType del_val = image[(ir - 1 - barSize)*cols+ic];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_90( image, ir, ic ));

			dest[ir*cols+ic] = dval ;
		}
	}
}

inline PixelType SegImpl::dilate_point_135( PixelType *image, unsigned int ir, unsigned int ic )
{
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 
	PixelType dval = 0;
	unsigned int p = (ir-barSize)*cols + ic-barSize;
	unsigned int p_max = (ir+barSize)*cols + ic+barSize;

	for ( ; p <= p_max; p = p + cols + 1 ) {
		if ( image[p] > dval )
			dval = image[p];
	}
	return dval;
}

void SegImpl::dilate_135( PixelType *dest, PixelType *image )
{
	memset( dest, 0, rows*cols*sizeof(PixelType) );
	const int barSize = this->GetMorphologicalOpeningBarSizePx(); 

	/* Up the left side. */
	for ( unsigned int sr = vertHigh-1; sr >= vertLow; sr-- ) {
		unsigned int ir = sr;
		unsigned int ic = horzLow;
		PixelType dval = dilate_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval ;
		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) :
					dilate_point_135( image, ir, ic ));
			dest[ir*cols+ic] = dval;
		}
	}

	/* Across the top. */
	for ( unsigned int sc = horzLow; sc < horzHigh; sc++ ) {
		unsigned int ic = sc;
		unsigned int ir = vertLow;
		PixelType dval = dilate_point_135( image, ir, ic );
		dest[ir*cols+ic] = dval;
		for ( ir++, ic++; ir < vertHigh && ic < horzHigh; ir++, ic++ ) {
			PixelType new_val = image[(ir + barSize)*cols+(ic + barSize)];
			PixelType del_val = image[(ir - 1 -barSize)*cols+(ic - 1 - barSize)];

			dval = new_val >= dval ? new_val :
					(del_val < dval ? max(dval, new_val) : 
					dilate_point_135( image, ir, ic ));
			dest[ir*cols+ic] = dval;
		}
	}
}

inline PixelType SegImpl::dilate_point( PixelType *image, unsigned int ir, unsigned int ic, 
                Item *shape, int slen )
{
	PixelType dval = 0;
	for ( int sp = 0; sp < slen; sp++ ) {
		unsigned int sr = ir + shape[sp].roff;
		unsigned int sc = ic + shape[sp].coff;
		if ( image[sr*cols+sc] > dval )
			dval = image[sr*cols+sc];
	}
	return dval;
}

void SegImpl::dilate_circle( PixelType *dest, PixelType *image )
{
	unsigned int slen = m_SegParams.mMorphologicalCircle.size();

	Item *shape = new Item[slen]; 

	for ( unsigned int i = 0; i < slen; i++ )
	{
		shape[i] = m_SegParams.mMorphologicalCircle[i]; 
	}

	/* Which elements stick around when you shift right? */
	int n = 0;

	bool *sr_exist = new bool[slen];	

	memset( sr_exist, 0, slen*sizeof(bool) );
	for ( int si = 0; si < slen; si++ ) 
	{
		if ( shape_contains( m_SegParams.mMorphologicalCircle, Item(m_SegParams.mMorphologicalCircle[si].roff, m_SegParams.mMorphologicalCircle[si].coff+1) ) )
			sr_exist[si] = true, n++;
	}
	//cout << "shift_exist: " << n << endl;

	Item *new_items = new Item[slen]; 
	Item *old_items = new Item[slen];
	
	int n_new_items = 0, n_old_items = 0;
	for ( int si = 0; si < slen; si++ ) {
		if ( sr_exist[si] )
			old_items[n_old_items++] = shape[si];
		else
			new_items[n_new_items++] = shape[si];
	}

	delete [] sr_exist; 

	memset( dest, 0, rows*cols*sizeof(PixelType) );
	for ( unsigned int ir = vertLow; ir < vertHigh; ir++ ) {
		unsigned int ic = horzLow;

		PixelType dval = dilate_point( image, ir, ic, shape, slen );
		PixelType last = dest[ir*cols+ic] = dval;
		
		for ( ic++; ic < horzHigh; ic++ ) {
			PixelType dval = dilate_point( image, ir, ic, new_items, n_new_items );

			if ( dval < last ) {
				for ( int sp = 0; sp < n_old_items; sp++ ) {
					unsigned int sr = ir + old_items[sp].roff;
					unsigned int sc = ic + old_items[sp].coff;
					if ( image[sr*cols+sc] > dval )
						dval = image[sr*cols+sc];

					if ( image[sr*cols+sc] == last )
						break;
				}
			}
			last = dest[ir*cols+ic] = dval ;
		}
	}
	delete [] new_items; 
	delete [] old_items; 
}

void SegImpl::subtract( PixelType *image, PixelType *vals )
{
	for ( unsigned int pos = 0; pos < rows*cols; pos++ )
		image[pos] = vals[pos] > image[pos] ? 0 : image[pos] - vals[pos];
	
}



void SegImpl::dynamicThresholding( PixelType *pixelArray)
{
	

	typedef unsigned char			PixelType; 
	const unsigned int				Dimension = 2; 

	// copy array of pixels into an image container
	typedef itk::Image< PixelType, Dimension > ImageType; 
	ImageType::Pointer image = ImageType::New(); 
	ImageType::SizeType size; 
	size[0] = cols; 
	size[1] = rows; 

	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0; 

	ImageType::RegionType wholeImage; 
	wholeImage.SetSize(size);
	wholeImage.SetIndex(start); 

	image->SetRegions(wholeImage); 
	image->Allocate(); 

	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType iter(image, image->GetRequestedRegion() ); 
	iter.GoToBegin(); 

	int count = 0; 	

	while( !iter.IsAtEnd())
	{
	iter.Set(pixelArray[count]);
	count++; 
	++iter;

	}

// mean set up
	// change region size
	typedef   unsigned char  InputPixelType;
	typedef   unsigned char  OutputPixelType;
	
	typedef itk::Image< InputPixelType,  2 >   InputImageType;
	typedef itk::Image< OutputPixelType, 2 >   OutputImageType;
	typedef itk::MeanImageFilter<InputImageType, OutputImageType >  FilterType;

	FilterType::Pointer meanFilter = FilterType::New();

	InputImageType::SizeType indexRadius;
  

	indexRadius[0] = 5; // radius along x
	indexRadius[1] = 8; // radius along y

	meanFilter->SetRadius( indexRadius );
 
// subtract filter set up
	typedef   unsigned char	SubtractPixelType;
	typedef itk::Image< SubtractPixelType,  2 >   SubtractResultsImageType;
	typedef itk::SubtractImageFilter<ImageType,OutputImageType,SubtractResultsImageType>SubtractionFilterType; 
	
	SubtractionFilterType::Pointer subtractionFilter = SubtractionFilterType::New(); 

//writer setup
	typedef itk::ImageFileWriter< ImageType > WriterType;
	WriterType::Pointer writeResult = WriterType::New();  // 
	writeResult->SetFileName("dynamicthresholding" + possibleFiducialsImageFilename);

//thresholding setup
	typedef   unsigned char ThresholdPixelType;
	typedef itk::Image< ThresholdPixelType, 2 > ThresholdResultImageType;
	typedef itk::BinaryThresholdImageFilter<SubtractResultsImageType,ThresholdResultImageType> ThresholdFilterType; 
	ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
	
//pipeline setup to incorporate mean and subtraction filters if desired, currently only 
//uses thresholding 

	//meanFilter->SetInput(image);
	//subtractionFilter->SetInput1(image);
	//subtractionFilter->SetInput2(meanFilter->GetOutput()); 
	//subtractionFilter->Update(); 
	
	/* 
	Threshold values based on the concept that after the opening operations
	the image has a black background with potential fiducials appearing less black
	*/  
	thresholdFilter->SetInput(image); 
	thresholdFilter->SetOutsideValue( 10);
	thresholdFilter->SetInsideValue( 255);
	thresholdFilter->SetLowerThreshold( 26); // 13
	thresholdFilter->SetUpperThreshold( 255);

	thresholdFilter->Update(); 
	//uncomment below if you wish to output the image
	// directly after thresholding, redundant if debug
	// mode is on 

	//writeResult->SetInput( thresholdFilter->GetOutput() );
	//writeResult->Update();

/*set working image pixel values to new thresholded image
tried using pixel buffer andmemcopy but couldn't figure
out how to do it*/ 
	
	ImageType::Pointer filteredImage = thresholdFilter->GetOutput(); 

	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType resultingImageIterator(filteredImage, filteredImage->GetRequestedRegion() ); 
	resultingImageIterator.GoToBegin(); 

	int counter = 0; 	

	while( !resultingImageIterator.IsAtEnd())
	{
	working[counter] = resultingImageIterator.Get();
	counter++; 
	++resultingImageIterator;

	}


	 

	
}
void SegImpl::WritePng(PixelType *modifiedImage, std::string outImageName, int cols, int rows) 
{

// output intermediate image
	
	typedef unsigned char          PixelType; // define type for pixel representation
	const unsigned int             Dimension = 2; 

	typedef itk::Image< PixelType, Dimension > ImageType;
	ImageType::Pointer modImage = ImageType::New(); 
	ImageType::SizeType size;
	size[0] = cols;
	size[1] = rows; 

	ImageType::IndexType start; 
	start[0] = 0; 
	start[1] = 0; 

	ImageType::RegionType wholeImage; 
	wholeImage.SetSize(size);
	wholeImage.SetIndex(start); 

	modImage->SetRegions(wholeImage); 
	modImage->Allocate(); 


	typedef itk::ImageFileWriter< ImageType > WriterType; 
	itk::PNGImageIO::Pointer pngImageIO = itk::PNGImageIO::New();
	pngImageIO->SetCompressionLevel(0); 

	WriterType::Pointer writer = WriterType::New();  
	writer->SetImageIO(pngImageIO); 
	writer->SetFileName(outImageName);   
	
	
	typedef itk::ImageRegionIterator<ImageType> IterType; 
	IterType iter(modImage, modImage->GetRequestedRegion() ); 
	iter.GoToBegin(); 

	int count = 0; 	

	while( !iter.IsAtEnd())
	{
	iter.Set(modifiedImage[count]);
	count++; 
	++iter;
	} 
	
	writer->SetInput( modImage );  // piping output of reader into input of writer
		try
	{
		writer->Update(); // change to writing if want writing feature
	}
	catch (itk::ExceptionObject & err) 
	{		
		std::cerr << " Exception! writer did not update" << std::endl; //ditto 
		std::cerr << err << std ::endl; 
		//return EXIT_GENERIC_FAILURE;
	}
	// end output

}






void SegImpl::morphological_operations()
{
  // Morphological operations with a stick-like structuring element
	
	if(debugOutput) 
	{
		WritePng(working,"seg01-initial.png", cols, rows); 
	}

	erode_0( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg02-morph-bar-deg0-erode.png", cols, rows); 
	}
	
	dilate_0( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg03-morph-bar-deg0-dilated.png", cols, rows); 
	}
	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg04-morph-bar-deg0-final.png", cols, rows); 
	}

	erode_45( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg05-morph-bar-deg45-erode.png", cols, rows); 
	}

	dilate_45( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg06-morph-bar-deg45-dilated.png", cols, rows); 
	}

	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg07-morph-bar-deg45-final.png", cols, rows); 
	}

	erode_90( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg08-morph-bar-deg90-erode.png", cols, rows); 
	}

	dilate_90( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg09-morph-bar-deg90-dilated.png", cols, rows); 
	}

	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg10-morph-bar-deg90-final.png", cols, rows); 
	}

	erode_135( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg11-morph-bar-deg135-erode.png", cols, rows); 
	}

	dilate_135( dilated, eroded );
	if(debugOutput) 
	{
		WritePng(dilated,"seg12-morph-bar-deg135-dilated.png", cols, rows); 
	}

	subtract( working, dilated );
	if(debugOutput) 
	{
		WritePng(working,"seg13-morph-bar-deg135-final.png", cols, rows); 
	}

	/* Circle operation. */
	erode_circle( eroded, working );
	if(debugOutput) 
	{
		WritePng(eroded,"seg14-morph-circle-erode.png", cols, rows); 
	}

	dilate_circle( working, eroded );
	if(debugOutput) 
	{
		WritePng(working,"seg15-morph-circle-final.png", cols, rows); 
	}
	
	//dynamicThresholding(working); 

	/*if(debugOutput) 
	{
		WritePng(working,"afterDT" + possibleFiducialsImageFilename); 
	}
	 */ 
	
}
