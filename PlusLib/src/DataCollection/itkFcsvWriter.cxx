/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include <string>
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

    std::ofstream out(m_FileName.c_str());

    if( !out.is_open() )
    {
      itkExceptionMacro("Failed to open file " << m_FileName );
    }

    const unsigned int dim = FcsvNDimensions;      //Dimension of the work space

    out << "# Fiducial List file " << m_FcsvDataObject.filePath;
    out << "# version = " << m_FcsvDataObject.version << std::endl;
    out << "# name = " << m_FcsvDataObject.name << std::endl;
    out << "# numPoints = " << numPoints << std::endl;   
    out << "# symbolScale = " << m_FcsvDataObject.symbolScale << std::endl;
    out << "# symbolType = " << m_FcsvDataObject.symbolType << std::endl;
    out << "# visibility = " << m_FcsvDataObject.visibility << std::endl;
    out << "# textScale = " << m_FcsvDataObject.textScale << std::endl;
    out << "# color = " << m_FcsvDataObject.color[0] << "," << m_FcsvDataObject.color[1] << "," << m_FcsvDataObject.color[2] << std::endl;
    out << "# selectedColor = " << m_FcsvDataObject.selectedColor[0] << "," << m_FcsvDataObject.selectedColor[1] << "," << m_FcsvDataObject.selectedColor[2] << std::endl;
    out << "# opacity = " << m_FcsvDataObject.opacity << std::endl;
    out << "# ambient = " << m_FcsvDataObject.ambient << std::endl;
    out << "# diffuse = " << m_FcsvDataObject.diffuse << std::endl;
    out << "# specular = " << m_FcsvDataObject.specular << std::endl;
    out << "# power = " << m_FcsvDataObject.power << std::endl;
    out << "# locked = " << m_FcsvDataObject.locked << std::endl;
    out << "# numberingScheme = " << m_FcsvDataObject.numberingScheme << std::endl;
    out << "# columns = label,x,y,z,sel,vis" << std::endl;          //std::vector<std::string> columns;    //:TODO: use std<std::string,int> instead   

    for( int i=0 ; i<numPoints ; i++ )    //Write a record 
    {
      out << m_FcsvDataObject.points[ i ].label;
      out << "," << m_FcsvDataObject.points[ i ].position[0];
      out << "," << m_FcsvDataObject.points[ i ].position[1];
      out << "," << m_FcsvDataObject.points[ i ].position[2];
      out << "," << m_FcsvDataObject.points[ i ].selected;
      out << "," << m_FcsvDataObject.points[ i ].visibility << std::endl;
    }
    out.close();
  }

} // namespace itk
