/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "itkFcsvReader.h"
#include "itkFcsvWriter.h"

namespace itk
{

FcsvWriter::FcsvWriter()
{
}

FcsvWriter::~FcsvWriter()
{
}


void FcsvWriter::Update()
{   
	itkDebugMacro(<<"Fiducial data header: color=  "<<m_FcsvDataObject.color[ 0 ]<<","<<m_FcsvDataObject.color[ 1 ]<<","<<m_FcsvDataObject.color[ 2 ])
	itkDebugMacro(<<"Fiducial points:");
	int numPoints=m_FcsvDataObject.points.size();
	for(int i=0; i<numPoints; i++)
	{
		itkDebugMacro(<<"\n"<<m_FcsvDataObject.points[ i ].label
					  <<"  "<<m_FcsvDataObject.points[ i ].position[0]
					  <<"  "<<m_FcsvDataObject.points[ i ].position[1]
					  <<"  "<<m_FcsvDataObject.points[ i ].position[2]
					  <<"  "<<m_FcsvDataObject.points[ i ].selected
					  <<"  "<<m_FcsvDataObject.points[ i ].visibility)
	}

	FILE * out = fopen(m_FileName.c_str(),"w");		//Open file on command line 

	if(out==NULL)
	{
		itkExceptionMacro("Failed to open file " << m_FileName );
	}

	const unsigned int dim = FcsvNDimensions;			//Dimension of the work space
	

	fprintf (out, "# Fiducial List file %s", m_FcsvDataObject.filePath.c_str() );
	fprintf (out, "# version = %f\n", m_FcsvDataObject.version );
	fprintf (out, "# name = %s", m_FcsvDataObject.name.c_str() );
	fprintf (out, "# numPoints = %d\n", numPoints );   
	fprintf (out, "# symbolScale = %d\n", m_FcsvDataObject.symbolScale );
	fprintf (out, "# symbolType = %d\n", m_FcsvDataObject.symbolType );
	fprintf (out, "# visibility = %d\n" , m_FcsvDataObject.visibility );
	fprintf (out, "# textScale = %f\n" , m_FcsvDataObject.textScale );
	fprintf (out, "# color = %f,%f,%f\n", m_FcsvDataObject.color[0], m_FcsvDataObject.color[1], m_FcsvDataObject.color[2] );
	fprintf (out, "# selectedColor = %f,%f,%f\n" , m_FcsvDataObject.selectedColor[0], m_FcsvDataObject.selectedColor[1], m_FcsvDataObject.selectedColor[2] );
	fprintf (out, "# opacity = %f\n" , m_FcsvDataObject.opacity );
	fprintf (out, "# ambient = %f\n" , m_FcsvDataObject.ambient );
	fprintf (out, "# diffuse = %f\n" , m_FcsvDataObject.diffuse );
	fprintf (out, "# specular = %f\n" , m_FcsvDataObject.specular );
	fprintf (out, "# power = %f\n" , m_FcsvDataObject.power );
	fprintf (out, "# locked = %d\n", m_FcsvDataObject.locked );
	fprintf (out, "# numberingScheme = %d\n", m_FcsvDataObject.numberingScheme);
	fprintf (out, "# columns = label,x,y,z,sel,vis\n"  );					//std::vector<std::string> columns;		//:TODO: use std<std::string,int> instead 	

	for( int i=0 ; i<numPoints ; i++ )		//Write a record 
	{
		fprintf (out, "%s" , m_FcsvDataObject.points[ i ].label.c_str()) ;
		fprintf (out, ",%f" , m_FcsvDataObject.points[ i ].position[0]) ;
		fprintf (out, ",%f" , m_FcsvDataObject.points[ i ].position[1]) ;
		fprintf (out, ",%f" , m_FcsvDataObject.points[ i ].position[2]) ;
		fprintf (out, ",%d" , m_FcsvDataObject.points[ i ].selected )  ;
		fprintf (out, ",%d\n" , m_FcsvDataObject.points[ i ].visibility) ;
	}
	
    fclose(out);

}

} // namespace itk
