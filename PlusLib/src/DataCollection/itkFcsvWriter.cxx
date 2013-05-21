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

    for(std::vector<FcsvPoint>::iterator it = m_FcsvDataObject.points.begin(); it != m_FcsvDataObject.points.end(); ++it)
    {
      itkDebugMacro(
        << std::endl << it->label
        << "  " << it->position[0]
        << "  " << it->position[1]
        << "  " << it->position[2]
        << "  " << it->selected
        << "  " << it->visibility)
    }

    std::ofstream out(m_FileName.c_str());

    if( !out.is_open() )
    {
      itkExceptionMacro("Failed to open file " << m_FileName );
    }

    out << "# Fiducial List file " << m_FcsvDataObject.filePath << std::endl;
    out << "# version = " << m_FcsvDataObject.version << std::endl;
    out << "# name = " << m_FcsvDataObject.name << std::endl;
    out << "# numPoints = " << m_FcsvDataObject.points.size() << std::endl;   
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

    for(std::vector<FcsvPoint>::iterator it = m_FcsvDataObject.points.begin(); it != m_FcsvDataObject.points.end(); ++it)
    {
      out << it->label;
      out << "," << it->position[0];
      out << "," << it->position[1];
      out << "," << it->position[2];
      out << "," << it->selected;
      out << "," << it->visibility << std::endl;
    }
    out.close();
  }

} // namespace itk
