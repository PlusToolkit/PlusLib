#include "PlusConfigure.h"
#include "vtkHTMLGenerator.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx" 


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkHTMLGenerator, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkHTMLGenerator);

//----------------------------------------------------------------------------
vtkHTMLGenerator::vtkHTMLGenerator()
{
	this->Title = NULL; 
	this->HtmlBody.str(""); 
}

//----------------------------------------------------------------------------
vtkHTMLGenerator::~vtkHTMLGenerator()
{

}


//----------------------------------------------------------------------------
void vtkHTMLGenerator::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
void vtkHTMLGenerator::AddHorizontalLine()
{
	this->HtmlBody << "<hr />" << std::endl; 
}

//----------------------------------------------------------------------------
void vtkHTMLGenerator::AddImage(const char* imageSource, const char* alt, const int widthPx/*=0*/, const int heightPx/*=0*/)
{
	if ( widthPx == 0 && heightPx == 0 )
	{
		this->HtmlBody << "<img src='" << imageSource << "' alt='" << alt << "' />" << std::endl; 
	}
	else 
	{
		this->HtmlBody << "<img src='" << imageSource << "' alt='" << alt << "' width='" << widthPx << "' height='" << heightPx << "' />" << std::endl; 
	}
}

//----------------------------------------------------------------------------
void vtkHTMLGenerator::AddParagraph( const char* paragraph )
{
	this->HtmlBody << "<p>" << paragraph << "</p>" << std::endl; 
}

//----------------------------------------------------------------------------
void vtkHTMLGenerator::AddLink( const char* linkText, const char* url )
{
	this->HtmlBody << "<a href='" << url << "'>" << linkText << "</a>" << std::endl; 
}

//----------------------------------------------------------------------------
void vtkHTMLGenerator::AddText( const char* text, HEADINGS h, const char* style/*=NULL*/)
{
	std::string openTag, closeTag; 
	switch(h)
	{
	case H1: 
		{
			if ( style == NULL )
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
			if ( style == NULL )
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
			if ( style == NULL )
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
			if ( style == NULL )
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
			if ( style == NULL )
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
			if ( style == NULL )
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
std::string vtkHTMLGenerator::GetHtmlBody()
{
	return this->HtmlBody.str(); 
}

//----------------------------------------------------------------------------
std::string vtkHTMLGenerator::GetHtmlPage()
{

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
void vtkHTMLGenerator::SaveHtmlPage(const char *fileName)
{
	std::ofstream htmlpage;
	htmlpage.open (fileName, ios::out);
	htmlpage << this->GetHtmlPage(); 
	htmlpage.close(); 
}