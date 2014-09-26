/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkHTMLGenerator_H
#define __vtkHTMLGenerator_H

#include "vtkPlusCommonExport.h"

#include "vtkObject.h"
#include <vector>
#include <sstream>

class vtkTable; 

/*!
  \class vtkHTMLGenerator 
  \brief class for generating basic html tags
  \ingroup PlusLibCommon
*/ 
class vtkPlusCommonExport vtkHTMLGenerator : public vtkObject
{
public: 
  
  /*! HTML headings  */
  enum HEADINGS
  {
    H1, 
    H2, 
    H3, 
    H4, 
    H5, 
    H6
  }; 

  static vtkHTMLGenerator* New();
  vtkTypeRevisionMacro(vtkHTMLGenerator,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent); 

  /*! Write the report to html file. Deprecated - use SaveHtmlPageAutoFilename instead. */
  virtual void SaveHtmlPage( const char * fileName); 

  /*! Write the report to html file. Create file in the output directory, including the application start time in the filename. */
  virtual std::string SaveHtmlPageAutoFilename( const char * baseName);

  /*! Add image to document */
  virtual void AddImage(const char* imageSource, const char* alt, const int widthPx=0, const int heightPx=0); 

  /*! Add paragraph to document */
  virtual void AddParagraph( const char* paragraph ); 
  
  /*! Add text to document */
  virtual void AddText( const char* text, HEADINGS h, const char* style=NULL); 

  /*! Add table to document */
  virtual void AddTable( vtkTable* table, int borderPx); 

  /*! Add a link to the document*/
  virtual void AddLink( const char* linkText, const char* url ); 

  /*! Add horizontal line separator to the document */
  virtual void AddHorizontalLine(); 

  /*! Set the page title */
  vtkSetStringMacro(Title);
  /*! Get the page title */
  vtkGetStringMacro(Title); 

  /*! Set the output directory */
  vtkSetStringMacro(OutputDirectory);
  /*! Get the output directory */
  vtkGetStringMacro(OutputDirectory);   

protected:

  /*! Get the body */
  virtual std::string GetHtmlBody(); 

  /*! Get the html page source */
  virtual std::string GetHtmlPage(); 

  vtkHTMLGenerator();
  virtual ~vtkHTMLGenerator(); 

  char* Title; 
  char* OutputDirectory; 

  std::ostringstream HtmlBody; 

private:
  vtkHTMLGenerator(const vtkHTMLGenerator&);  // Not implemented.
  void operator=(const vtkHTMLGenerator&);  // Not implemented.
}; 

#endif
