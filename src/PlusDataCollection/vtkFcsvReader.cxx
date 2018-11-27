/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkFcsvReader.h"

// STL includes
#include <sstream>
#include <string>
#include <vector>

// VTK includes
#include <vtkObjectFactory.h>

#define MAXFLDS 30      // maximum possible number of fields
#define MAXFLDSIZE 150  // longest possible field + 1 = 31 byte field

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkFcsvReader);

//----------------------------------------------------------------------------
vtkFcsvReader::vtkFcsvReader()
{
}

//----------------------------------------------------------------------------
vtkFcsvReader::~vtkFcsvReader()
{
}

//----------------------------------------------------------------------------
void vtkFcsvReader::PrintSelf(ostream& os, vtkIndent indent)
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
void vtkFcsvReader::Update()
{
  std::ifstream in(FileName.c_str());
  if (!in.is_open())
  {
    LOG_ERROR("Failed to open file " << FileName);
  }

  FcsvDataObject.columns.resize(MAXFLDS);

  std::string line;
  while (std::getline(in, line))
  {
    if (line[0] == '#' && line.find('=') != std::string::npos)
    {
      // Header, process it
      std::vector<std::string> elems;
      igsioCommon::SplitStringIntoTokens(line, '=', elems);
      this->StripCharsFromString(elems[0], " #");
      elems[1] = igsioCommon::Trim(elems[1]);
      this->ProcessHeaderEntry(elems[0], elems[1]);
    }
    else if (line[0] == '#')
    {
      // Fiducial List file entry
      std::vector<std::string> elems;
      igsioCommon::SplitStringIntoTokens(line, ' ', elems);
      FcsvDataObject.filePath = elems[4];
    }
    else
    {
      // It's a record, read it as such
      std::vector<std::string> elems;
      igsioCommon::SplitStringIntoTokens(line, ',', elems);

      FcsvPoint point;
      point.label        = elems[0];
      point.position[0]  = atof(elems[1].c_str());
      point.position[1]  = atof(elems[2].c_str());
      point.position[2]  = atof(elems[3].c_str());
      point.selected     = atoi(elems[4].c_str());
      point.visibility   = atoi(elems[5].c_str());

      FcsvDataObject.points.push_back(point);
    }
  }

  in.close();
}

//----------------------------------------------------------------------------
void vtkFcsvReader::ProcessHeaderEntry(const std::string& headerEntry, const std::string& headerEntryValue)
{
  if (igsioCommon::IsEqualInsensitive(headerEntry, "numPoints"))
  {
    FcsvDataObject.numPoints = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "version"))
  {
    FcsvDataObject.version = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "name"))
  {
    FcsvDataObject.name = headerEntryValue;
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "symbolScale"))
  {
    FcsvDataObject.symbolScale = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "symbolType"))
  {
    FcsvDataObject.symbolType = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "visibility"))
  {
    FcsvDataObject.visibility = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "textScale"))
  {
    FcsvDataObject.textScale = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "opacity"))
  {
    FcsvDataObject.opacity = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "ambient"))
  {
    FcsvDataObject.ambient = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "diffuse"))
  {
    FcsvDataObject.diffuse = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "specular"))
  {
    FcsvDataObject.specular = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "power"))
  {
    FcsvDataObject.power = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "locked"))
  {
    FcsvDataObject.locked = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "numberingScheme"))
  {
    FcsvDataObject.numberingScheme = atof(headerEntryValue.c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "color"))
  {
    std::vector<std::string> elems;
    igsioCommon::SplitStringIntoTokens(headerEntryValue, ',', elems);

    FcsvDataObject.color[0] = atof(elems[0].c_str());
    FcsvDataObject.color[1] = atof(elems[1].c_str());
    FcsvDataObject.color[2] = atof(elems[2].c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "selectedColor"))
  {
    std::vector<std::string> elems;
    igsioCommon::SplitStringIntoTokens(headerEntryValue, ',', elems);

    FcsvDataObject.selectedColor[0] = atof(elems[0].c_str());
    FcsvDataObject.selectedColor[1] = atof(elems[1].c_str());
    FcsvDataObject.selectedColor[2] = atof(elems[2].c_str());
  }
  else if (igsioCommon::IsEqualInsensitive(headerEntry, "columns"))
  {
    std::vector<std::string> elems;
    igsioCommon::SplitStringIntoTokens(headerEntryValue, ' ', elems);
  }
}

//----------------------------------------------------------------------------
void vtkFcsvReader::StripCharsFromString(std::string& str, const std::string& chars)
{
  for (unsigned int i = 0; i < chars.size(); ++i)
  {
    // you need include <algorithm> to use general algorithms like std::remove()
    str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
  }
}