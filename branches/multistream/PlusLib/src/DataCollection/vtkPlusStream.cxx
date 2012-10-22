/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPlusStream.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusStream, "$Revision: 1.0$");
vtkStandardNewMacro(vtkPlusStream);

//----------------------------------------------------------------------------

vtkPlusStream::vtkPlusStream(void)
{
}

//----------------------------------------------------------------------------

vtkPlusStream::~vtkPlusStream(void)
{
  StreamBuffers.clear();
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusStream::ReadConfiguration( vtkXMLDataElement* aStreamElement )
{
  // Read the stream element, build the stream
  return PLUS_SUCCESS;
}
