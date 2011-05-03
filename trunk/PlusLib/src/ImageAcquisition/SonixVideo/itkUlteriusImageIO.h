/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkUlteriusImageIO.h,v $
  Language:  C++
  Date:      $Date: 2009-06-14 12:30:13 $
  Version:   $Revision: 1.38 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkUlteriusImageIO_h
#define __itkUlteriusImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include "itkImageIOBase.h"

#include "ulterius_def.h"

namespace itk
{

/** \class UlteriusImageIO
 *
 *  \brief Read UlteriusImage file format.
 *
 *  \ingroup IOFilters
 *
 * Image dimensions: widht, height, frames
 * Limitations: spacing is always 1.0 mm, assume that no endianness change is needed, writing is not supported
 *
 */
class ITK_EXPORT UlteriusImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef UlteriusImageIO        Self;
  typedef ImageIOBase        Superclass;
  typedef SmartPointer<Self> Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(UlteriusImageIO, Superclass);

  /** The different types of ImageIO's can support data of varying
   * dimensionality. For example, some file formats are strictly 2D
   * while others can support 2D, 3D, or even n-D. This method returns
   * true/false as to whether the ImageIO can support the dimension
   * indicated. */
  virtual bool SupportsDimension(unsigned long )
    {
    return true;
    }

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();
  
  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);
 
  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();
  
  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);
  
protected:
  UlteriusImageIO();
  ~UlteriusImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  bool ReadHeader(const char* filename);

  uFileHeader m_FileHeader;

private:
  
  UlteriusImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#endif // __itkUlteriusImageIO_h
