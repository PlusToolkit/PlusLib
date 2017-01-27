/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkFcsvWriter.h"

// STL includes
#include <string>

// VTK includes
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkFcsvWriter);

//----------------------------------------------------------------------------
vtkFcsvWriter::vtkFcsvWriter()
{
}

//----------------------------------------------------------------------------
vtkFcsvWriter::~vtkFcsvWriter()
{
}

//----------------------------------------------------------------------------
void vtkFcsvWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Filename: " << FileName << std::endl;
  os << indent << "FcsvDataObject.filePath: " << FcsvDataObject.filePath << std::endl;
  os << indent << "FcsvDataObject.name: " << FcsvDataObject.name << std::endl;
  os << indent << "FcsvDataObject.numPoints: " << FcsvDataObject.numPoints << std::endl;
  os << indent << "FcsvDataObject.symbolScale: " << FcsvDataObject.symbolScale << std::endl;
  os << indent << "FcsvDataObject.symbolType: " << FcsvDataObject.symbolType << std::endl;
  os << indent << "FcsvDataObject.visibility: " << FcsvDataObject.visibility << std::endl;
  os << indent << "FcsvDataObject.version: " << FcsvDataObject.version << std::endl;
  os << indent << "FcsvDataObject.textScale: " << FcsvDataObject.textScale << std::endl;
  os << indent << "FcsvDataObject.color[3]: " << FcsvDataObject.color[0] << ", " << FcsvDataObject.color[1] << ", " << FcsvDataObject.color[2] << std::endl;
  os << indent << "FcsvDataObject.selectedColor[3]: " << FcsvDataObject.selectedColor[0] << ", " << FcsvDataObject.selectedColor[1] << ", " << FcsvDataObject.selectedColor[2] << std::endl;
  os << indent << "FcsvDataObject.opacity: " << FcsvDataObject.opacity << std::endl;
  os << indent << "FcsvDataObject.ambient: " << FcsvDataObject.ambient << std::endl;
  os << indent << "FcsvDataObject.diffuse: " << FcsvDataObject.diffuse << std::endl;
  os << indent << "FcsvDataObject.specular: " << FcsvDataObject.specular << std::endl;
  os << indent << "FcsvDataObject.power: " << FcsvDataObject.power << std::endl;
  os << indent << "FcsvDataObject.locked: " << FcsvDataObject.locked << std::endl;
  os << indent << "FcsvDataObject.numberingScheme: " << FcsvDataObject.numberingScheme << std::endl;
  for (unsigned int i = 0; i < FcsvDataObject.columns.size(); ++i)
  {
    os << indent << "FcsvDataObject.column " << i + 1 << ": " << FcsvDataObject.columns[i] << std::endl;
  }
  for (unsigned int i = 0; i < FcsvDataObject.points.size(); ++i)
  {
    os << indent << "FcsvDataObject.point " << FcsvDataObject.points[i].label << ": " << FcsvDataObject.points[i].position[0] << ", " << FcsvDataObject.points[i].position[1] << ", " << FcsvDataObject.points[i].position[2] << std::endl;
  }
}

//----------------------------------------------------------------------------
void vtkFcsvWriter::Update()
{
  std::ofstream out(FileName.c_str());
  if (!out.is_open())
  {
    LOG_ERROR("Failed to open file " << FileName);
  }

  out << "# Fiducial List file " << FcsvDataObject.filePath << std::endl;
  out << "# version = " << FcsvDataObject.version << std::endl;
  out << "# name = " << FcsvDataObject.name << std::endl;
  out << "# numPoints = " << FcsvDataObject.points.size() << std::endl;
  out << "# symbolScale = " << FcsvDataObject.symbolScale << std::endl;
  out << "# symbolType = " << FcsvDataObject.symbolType << std::endl;
  out << "# visibility = " << FcsvDataObject.visibility << std::endl;
  out << "# textScale = " << FcsvDataObject.textScale << std::endl;
  out << "# color = " << FcsvDataObject.color[0] << "," << FcsvDataObject.color[1] << "," << FcsvDataObject.color[2] << std::endl;
  out << "# selectedColor = " << FcsvDataObject.selectedColor[0] << "," << FcsvDataObject.selectedColor[1] << "," << FcsvDataObject.selectedColor[2] << std::endl;
  out << "# opacity = " << FcsvDataObject.opacity << std::endl;
  out << "# ambient = " << FcsvDataObject.ambient << std::endl;
  out << "# diffuse = " << FcsvDataObject.diffuse << std::endl;
  out << "# specular = " << FcsvDataObject.specular << std::endl;
  out << "# power = " << FcsvDataObject.power << std::endl;
  out << "# locked = " << FcsvDataObject.locked << std::endl;
  out << "# numberingScheme = " << FcsvDataObject.numberingScheme << std::endl;
  out << "# columns = label,x,y,z,sel,vis" << std::endl;

  for (std::vector<FcsvPoint>::iterator it = FcsvDataObject.points.begin(); it != FcsvDataObject.points.end(); ++it)
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

//----------------------------------------------------------------------------
void vtkFcsvWriter::CopyFcsvDataObject(const FcsvData& dataObject)
{
  FcsvDataObject = dataObject;
}