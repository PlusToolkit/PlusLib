/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================

  Program:   Heart Signal Box for VTK
  Module:    $RCSfile: vtkHeartSignalBox.h,v $
  Creator:   Chris Wedlake <cwedlake@imaging.robarts.ca>
  Language:  C++
  Author:    $Author: dpace $
  Date:      $Date: 2008/09/19 17:31:37 $
  Version:   $Revision: 1.3 $
             Rearranged September 10, 2008 by dpace to have all signal
             boxes inherit from a parent class

==========================================================================

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
vBE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkHeartSignalBox - signal box for the heart phantom

#ifndef __vtkHeartSignalBox_h
#define __vtkHeartSignalBox_h

#include "vtkSignalBox.h"

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

  int signalChannel1; //pin ID 10
  int signalChannel2; //pin ID 13
                      //Ground is Pin ID 24
  int basePort;
  int Channel1Pin;
  int Channel2Pin;


private:
  vtkHeartSignalBox(const vtkHeartSignalBox&);
  void operator=(const vtkHeartSignalBox&);  
};

#endif

