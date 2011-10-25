/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/


#include "vtkOpenIGTLinkPlusController.h"
#include "vtkObjectFactory.h"


vtkCxxRevisionMacro( vtkOpenIGTLinkPlusController, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkOpenIGTLinkPlusController ); 



void
vtkOpenIGTLinkPlusController
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}



vtkOpenIGTLinkPlusController
::vtkOpenIGTLinkPlusController()
{

}



vtkOpenIGTLinkPlusController
::~vtkOpenIGTLinkPlusController()
{

}



void
vtkOpenIGTLinkPlusController
::SetDataCollector( vtkDataCollector* dataCollector )
{
  this->DataCollector = dataCollector;
}



vtkOpenIGTLinkPlusController::ReturnValue
vtkOpenIGTLinkPlusController
::Initialize( std::string &strError )
{
  
  return RETURN_SUCCESS;
}


