/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "itkFcsvReader.h"
#include <sstream>
#include <string>
#include <vector>

#define MAXFLDS 30     /* maximum possible number of fields */
#define MAXFLDSIZE 150   /* longest possible field + 1 = 31 byte field */

namespace itk
{

  //-------------------------------------------------------
  FcsvReader::FcsvReader()
  {
  }

  //-------------------------------------------------------
  FcsvReader::~FcsvReader()
  {
  }

  //-------------------------------------------------------
  void FcsvReader::Update()
  {
    std::ifstream in(m_FileName.c_str());

    if( !in.is_open() )
    {
      itkExceptionMacro("Failed to open file " << m_FileName );
    }

    m_FcsvDataObject.columns.resize(MAXFLDS);

    std::string line;
    while( std::getline(in,line) )
    {
      if( line[0] == '#' && line.find('=') != std::string::npos )
      {
        // Header, process it
        std::vector<std::string> elems;
        this->SplitStringIntoTokens(line, '=', elems);
        this->StripCharsFromString(elems[0], " #");
        elems[1] = PlusCommon::Trim(elems[1].c_str());
        this->ProcessHeaderEntry(elems[0], elems[1]);
      }
      else if( line[0] == '#' )
      {
        // Fiducial List file entry
        std::vector<std::string> elems;
        this->SplitStringIntoTokens(line, ' ', elems);
        m_FcsvDataObject.filePath = elems[4];
      }
      else
      {
        // It's a record, read it as such
        std::vector<std::string> elems;
        this->SplitStringIntoTokens(line, ',', elems);

        itk::FcsvPoint point;
        point.label        = elems[0];
        point.position[0]  = atof(elems[1].c_str());
        point.position[1]  = atof(elems[2].c_str());
        point.position[2]  = atof(elems[3].c_str());
        point.selected     = atoi(elems[4].c_str());
        point.visibility   = atoi(elems[5].c_str());

        m_FcsvDataObject.points.push_back(point);
      }     
    }

    itkDebugMacro(<<"Fiducial data header: color=  "<<m_FcsvDataObject.color[ 0 ]<<","<<m_FcsvDataObject.color[ 1 ]<<","<<m_FcsvDataObject.color[ 2 ])
      itkDebugMacro(<<"Fiducial points:");
    for(std::vector<FcsvPoint>::iterator it = m_FcsvDataObject.points.begin(); it != m_FcsvDataObject.points.end(); ++it)
    {
      itkDebugMacro(<< std::endl << it->label
        << "  " << it->position[0]
        << "  " << it->position[1]
        << "  " << it->position[2]
        << "  " << it->selected
        << "  " << it->visibility)
    }
    in.close();
  }

  //-------------------------------------------------------
  std::vector<std::string>& FcsvReader::SplitStringIntoTokens(const std::string &s, char delim, std::vector<std::string> &elems)
  {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  }

  //-------------------------------------------------------
  void FcsvReader::ProcessHeaderEntry( const std::string &headerEntry, const std::string& headerEntryValue )
  {
    if( headerEntry.compare("numPoints") == 0 )
    {
      m_FcsvDataObject.numPoints = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("version") == 0 )
    {
      m_FcsvDataObject.version = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("name") == 0 )
    {
      m_FcsvDataObject.name = headerEntryValue;
    }
    else if( headerEntry.compare("symbolScale") == 0 )
    {
      m_FcsvDataObject.symbolScale = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("symbolType") == 0 )
    {
      m_FcsvDataObject.symbolType = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("visibility") == 0 )
    {
      m_FcsvDataObject.visibility = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("textScale") == 0 )
    {
      m_FcsvDataObject.textScale = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("opacity") == 0 )
    {
      m_FcsvDataObject.opacity = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("ambient") == 0 )
    {
      m_FcsvDataObject.ambient = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("diffuse") == 0 )
    {
      m_FcsvDataObject.diffuse = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("specular") == 0 )
    {
      m_FcsvDataObject.specular = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("power") == 0 )
    {
      m_FcsvDataObject.power = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("locked") == 0 )
    {
      m_FcsvDataObject.locked = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("numberingScheme") == 0 )
    {
      m_FcsvDataObject.numberingScheme = atof( headerEntryValue.c_str() );
    }
    else if( headerEntry.compare("color") == 0 )
    {
      std::vector<std::string> elems;
      this->SplitStringIntoTokens(headerEntryValue, ',', elems);

      m_FcsvDataObject.color[0] = atof( elems[0].c_str() );
      m_FcsvDataObject.color[1] = atof( elems[1].c_str() );
      m_FcsvDataObject.color[2] = atof( elems[2].c_str() );
    }
    else if( headerEntry.compare("selectedColor") == 0 )
    {
      std::vector<std::string> elems;
      this->SplitStringIntoTokens(headerEntryValue, ',', elems);

      m_FcsvDataObject.selectedColor[0] = atof( elems[0].c_str() );
      m_FcsvDataObject.selectedColor[1] = atof( elems[1].c_str() );
      m_FcsvDataObject.selectedColor[2] = atof( elems[2].c_str() );
    }
    else if( headerEntry.compare("columns") == 0 )
    {
      std::vector<std::string> elems;
      this->SplitStringIntoTokens(headerEntryValue, ' ', elems);
    }
  }

  //-------------------------------------------------------
  void FcsvReader::StripCharsFromString(std::string& str, const std::string& chars)
  {
    for (unsigned int i = 0; i < chars.size(); ++i)
    {
      // you need include <algorithm> to use general algorithms like std::remove()
      str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
    }
  }
}
