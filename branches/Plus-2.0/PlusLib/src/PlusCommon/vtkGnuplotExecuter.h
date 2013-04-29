/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __VTKGNUPLOTEXECUTER_H
#define __VTKGNUPLOTEXECUTER_H

#include "vtkObject.h"
#include "vtkTable.h"
#include <vector>

/*!
  \class vtkGnuplotExecuter 
  \brief Class for executing gnuplot for diagram generation
  \ingroup PlusLibCommon
*/ 
class VTK_EXPORT vtkGnuplotExecuter : public vtkObject
{
public: 
  static vtkGnuplotExecuter* New();
  vtkTypeRevisionMacro(vtkGnuplotExecuter,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent); 

  /*! Execute gnuplot with the given arguments */
  PlusStatus Execute(); 

  /*! Add custom argument */
  void AddArgument(const char* argument); 

  /*! Clear arguments */
  void ClearArguments(); 

  /*! Dump vtkTable to text file in gnuplot format */
  static PlusStatus DumpTableToFileInGnuplotFormat( vtkTable* table, const char* filename); 

  /*! Set working directory */
  vtkSetStringMacro(WorkingDirectory); 
  /*! Get working directory */
  vtkGetStringMacro(WorkingDirectory); 

  /*! Set/get gnuplot command with path */
  vtkSetStringMacro(GnuplotCommand); 
  /*! Set/get gnuplot command with path */
  vtkGetStringMacro(GnuplotCommand); 

  /*! Set/get flag whether to hide gnuplot window on Windows. */
  vtkSetMacro(HideWindow, bool); 
  /*! Set/get flag whether to hide gnuplot window on Windows. */
  vtkGetMacro(HideWindow, bool); 
  /*! Set/get flag whether to hide gnuplot window on Windows. */
  vtkBooleanMacro(HideWindow, bool); 
  
  /*! Set execution timeout of command  */
  vtkSetMacro(Timeout, double); 
  /*! Get execution timeout of command  */
  vtkGetMacro(Timeout, double); 

protected:
  vtkGnuplotExecuter();
  virtual ~vtkGnuplotExecuter(); 

  char* WorkingDirectory; 
  char* GnuplotCommand; 
  std::vector<std::string> GnuplotArguments;

  bool HideWindow; 
  double Timeout; 

private:
  vtkGnuplotExecuter(const vtkGnuplotExecuter&);  // Not implemented.
  void operator=(const vtkGnuplotExecuter&);  // Not implemented.
}; 

#endif
