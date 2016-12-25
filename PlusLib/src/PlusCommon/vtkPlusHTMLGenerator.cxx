/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusHTMLGenerator.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTable.h"
#include "vtkVariant.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusHTMLGenerator);

//----------------------------------------------------------------------------
vtkPlusHTMLGenerator::vtkPlusHTMLGenerator()
{
  this->Title = NULL;
  this->OutputDirectory = NULL;
  this->BaseFilename = NULL;
  this->SetBaseFilename("PlusReport");
  this->SetTitle("");
  this->HtmlBody.clear();
  this->SetOutputDirectory(vtkPlusConfig::GetInstance()->GetOutputDirectory().c_str());
}

//----------------------------------------------------------------------------
vtkPlusHTMLGenerator::~vtkPlusHTMLGenerator()
{

}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::AddHorizontalLine()
{
  LOG_TRACE("vtkPlusHTMLGenerator::AddHorizontalLine");
  this->HtmlBody << "<hr />" << std::endl;
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::AddImage(const char* imageSource, const char* alt, const int widthPx/*=0*/, const int heightPx/*=0*/)
{
  LOG_TRACE("vtkPlusHTMLGenerator::AddImage");
  if (imageSource == NULL)
  {
    LOG_ERROR("Unable to add imageSource to HTML document - imageSource is NULL!");
    return;
  }

  if (alt == NULL)
  {
    LOG_ERROR("Unable to add alterantive text to HTML document - alt is NULL!");
    return;
  }

  if (widthPx == 0 && heightPx == 0)
  {
    this->HtmlBody << "<img src='" << imageSource << "' alt='" << alt << "' />" << std::endl;
  }
  else
  {
    this->HtmlBody << "<img src='" << imageSource << "' alt='" << alt << "' width='" << widthPx << "' height='" << heightPx << "' />" << std::endl;
  }
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::AddParagraph(const char* paragraph)
{
  LOG_TRACE("vtkPlusHTMLGenerator::AddParagraph");
  if (paragraph == NULL)
  {
    LOG_ERROR("Unable to add paragraph to HTML document - paragraph is NULL!");
    return;
  }

  this->HtmlBody << "<p>" << paragraph << "</p>" << std::endl;
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::AddLink(const char* linkText, const char* url)
{
  LOG_TRACE("vtkPlusHTMLGenerator::AddLink");
  if (linkText == NULL)
  {
    LOG_ERROR("Unable to add link to HTML document - linkText is NULL!");
    return;
  }

  if (url == NULL)
  {
    LOG_ERROR("Unable to add link to HTML document - url is NULL!");
    return;
  }

  this->HtmlBody << "<a href='" << url << "'>" << linkText << "</a>" << std::endl;
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::AddText(const char* text, HEADINGS h, const char* style/*=NULL*/)
{
  LOG_TRACE("vtkPlusHTMLGenerator::AddText");
  if (text == NULL)
  {
    LOG_ERROR("Unable to add text to HTML document - input text is NULL!");
    return;
  }

  std::string openTag, closeTag;
  switch (h)
  {
    case H1:
    {
      if (style == NULL)
      {
        openTag  = "<h1>";
      }
      else
      {
        openTag  = "<h1 style='" + std::string(style) + "'>";
      }
      closeTag = "</h1>";
    }
    break;

    case H2:
    {
      if (style == NULL)
      {
        openTag  = "<h2>";
      }
      else
      {
        openTag  = "<h2 style='" + std::string(style) + "'>";
      }
      closeTag = "</h2>";
    }
    break;

    case H3:
    {
      if (style == NULL)
      {
        openTag  = "<h3>";
      }
      else
      {
        openTag  = "<h3 style='" + std::string(style) + "'>";
      }

      closeTag = "</h3>";
    }
    break;

    case H4:
    {
      if (style == NULL)
      {
        openTag  = "<h4>";
      }
      else
      {
        openTag  = "<h4 style='" + std::string(style) + "'>";
      }

      closeTag = "</h4>";
    }
    break;

    case H5:
    {
      if (style == NULL)
      {
        openTag  = "<h5>";
      }
      else
      {
        openTag  = "<h5 style='" + std::string(style) + "'>";
      }

      closeTag = "</h5>";
    }
    break;

    case H6:
    {
      if (style == NULL)
      {
        openTag  = "<h6>";
      }
      else
      {
        openTag  = "<h6 style='" + std::string(style) + "'>";
      }

      closeTag = "</h6>";
    }
    break;
  }

  this->HtmlBody << openTag << text << closeTag << std::endl;
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::AddTable(vtkTable* inputTable, int borderPx)
{
  LOG_TRACE("vtkPlusHTMLGenerator::AddTable");
  if (inputTable == NULL)
  {
    LOG_ERROR("Unable to add table to HTML document - table is NULL");
    return;
  }

  std::ostringstream openTag;
  openTag <<  "<table border='" << borderPx << "'>";
  std::string closeTag = "</table>";

  std::ostringstream table;

  // Create table header
  table << "<tr>";
  for (int c = 0; c < inputTable->GetNumberOfColumns(); ++c)
  {
    table << "<th>" << inputTable->GetColumnName(c) << "</th>";
  }
  table << "</tr>";

  for (int r = 0; r < inputTable->GetNumberOfRows(); ++r)
  {
    table << "<tr>";
    for (int c = 0; c < inputTable->GetNumberOfColumns(); ++c)
    {
      table << "<td>" << inputTable->GetValue(r, c).ToString() << "</td>";
    }
    table << "</tr>";
  }

  this->HtmlBody << openTag.str() << table.str() << closeTag << std::endl;
}

//----------------------------------------------------------------------------
std::string vtkPlusHTMLGenerator::GetHtmlBody()
{
  LOG_TRACE("vtkPlusHTMLGenerator::GetHtmlBody");
  return this->HtmlBody.str();
}

//----------------------------------------------------------------------------
std::string vtkPlusHTMLGenerator::GetHtmlPage()
{
  LOG_TRACE("vtkPlusHTMLGenerator::GetHtmlPage");
  std::ostringstream page;
  page << "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>" << std::endl;
  page << "<html>" << std::endl;
  page << "<head>" << std::endl;
  page << "<title>" << this->GetTitle() << "</title>" << std::endl;
  page << "</head>" << std::endl;
  page << "<body>" << std::endl;
  page << this->GetHtmlBody();
  page << "</body>" << std::endl;
  page << "</html>";
  page << std::ends;

  return page.str();
}

//----------------------------------------------------------------------------
void vtkPlusHTMLGenerator::SaveHtmlPage(const char* fileName)
{
  LOG_TRACE("vtkPlusHTMLGenerator::SaveHtmlPage");
  std::ofstream htmlpage;
  htmlpage.open(fileName, ios::out);
  htmlpage << this->GetHtmlPage();
  htmlpage.close();
}

//----------------------------------------------------------------------------
std::string vtkPlusHTMLGenerator::SaveHtmlPageAutoFilename()
{
  std::string fullPath;
  if (GetOutputDirectory())
  {
    fullPath += GetOutputDirectory();
    fullPath += "/";
  }
  fullPath += std::string(this->GetBaseFilename()) + "-" + vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp() + ".html";
  LOG_INFO("Write HTML report to " << fullPath);
  SaveHtmlPage(fullPath.c_str());
  return fullPath;
}

//----------------------------------------------------------------------------
std::string vtkPlusHTMLGenerator::AddImageAutoFilename(const char* filenamePostfix, const char* description, const int widthPx/*=0*/, const int heightPx/*=0*/)
{
  std::string fullPath;
  if (GetOutputDirectory())
  {
    fullPath += GetOutputDirectory();
    fullPath += "/";
  }
  fullPath += std::string(this->GetBaseFilename()) + "-" + vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp() + "-" + filenamePostfix;
  AddImage(fullPath.c_str(), description, widthPx, heightPx);
  return fullPath;
}