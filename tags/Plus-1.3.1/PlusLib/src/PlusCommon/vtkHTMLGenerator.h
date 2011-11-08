/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// .NAME vtkHTMLGenerator - class for generating basic html tags
// .SECTION Description
// This class is used for generating basic html tags 

#ifndef __vtkHTMLGenerator_H
#define __vtkHTMLGenerator_H

#include "vtkObject.h"
#include <vector>
#include <sstream>

class vtkTable; 

class VTK_EXPORT vtkHTMLGenerator : public vtkObject
{
public: 
	
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

	// Description: 
	// Get the html page source
	virtual void SaveHtmlPage( const char * fileName); 

	// Description: 
	// Add image to document
	virtual void AddImage(const char* imageSource, const char* alt, const int widthPx=0, const int heightPx=0); 

	// Description: 
	// Add paragraph to document
	virtual void AddParagraph( const char* paragraph ); 
	
	// Description: 
	// Add text to document 
	virtual void AddText( const char* text, HEADINGS h, const char* style=NULL); 

  // Description: 
  // Add table to document
  virtual void AddTable( vtkTable* table, int borderPx); 

	// Description: 
	// 
	virtual void AddLink( const char* linkText, const char* url ); 

	// Description: 
	// Add horizontal line
	virtual void AddHorizontalLine(); 

	// Description: 
	// Set/get page title 
	vtkSetStringMacro(Title); 
	vtkGetStringMacro(Title); 

protected:

	// Description: 
	// Get the body 
	virtual std::string GetHtmlBody(); 

	// Description: 
	// Get the html page source
	virtual std::string GetHtmlPage(); 

	vtkHTMLGenerator();
	virtual ~vtkHTMLGenerator(); 

	char* Title; 

	std::ostringstream HtmlBody; 

private:
	vtkHTMLGenerator(const vtkHTMLGenerator&);  // Not implemented.
	void operator=(const vtkHTMLGenerator&);  // Not implemented.
}; 

#endif