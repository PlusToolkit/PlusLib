/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Authors include: Chris Wedlake <cwedlake@imaging.robarts.ca>, Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/  

#ifndef __vtkHeartSignalBox_h
#define __vtkHeartSignalBox_h

#include "vtkSignalBox.h"

/*!
  \class vtkHeartSignalBox
  \brief Signal box for the heart phantom
  \ingroup PlusLibSignaling
*/
class VTK_EXPORT vtkHeartSignalBox : public vtkSignalBox
{
public:
  static vtkHeartSignalBox *New();
  vtkTypeMacro(vtkHeartSignalBox,vtkSignalBox);
  void PrintSelf(ostream& os, vtkIndent indent);

  void Start();

  int GetSignal1(void);
  int GetSignal2(void);

  void SwapSignals();
  void SetBasePort(int value);
  int GetBasePort();

protected:
  vtkHeartSignalBox();
  ~vtkHeartSignalBox();

  int GetECG(void);

  /*! pin ID 10 (ground is Pin ID 24) */
  int signalChannel1; 
  /*! pin ID 13 (ground is Pin ID 24) */
  int signalChannel2; 
                      
  int basePort;
  int Channel1Pin;
  int Channel2Pin;

private:
  vtkHeartSignalBox(const vtkHeartSignalBox&);
  void operator=(const vtkHeartSignalBox&);  
};

#endif

