
/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================

Module:    $RCSfile: vtkBKOEMVideoSource.h,v $
Author:  Siddharth Vikal, Queens School Of Computing

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Author: Danielle Pace
        Robarts Research Institute and The University of Western Ontario


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Queen's University nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#ifndef __vtkBKOEMVideoSource_h
#define __vtkBKOEMVideoSource_h

// PLUS Includes
#include "PlusConfigure.h"
#include "vtkBKOEMVideoSource.h"

// forward declarations
class PlusBKOEMReceiver;

//BTX

class VTK_EXPORT vtkBKOEMVideoSource;

/*!
\class vtkBKOEMVideoSourceCleanup 
\brief Class that cleans up (deletes singleton instance of) vtkBKOEMVideoSource when destroyed
\ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkBKOEMVideoSourceCleanup
{
public:
  vtkBKOEMVideoSourceCleanup();
  ~vtkBKOEMVideoSourceCleanup();
};
//ETX

class VTK_EXPORT vtkBKOEMVideoSource : public vtkBKOEMVideoSource, public IAcquisitionDataReceiver
{
public:
  vtkTypeRevisionMacro(vtkBKOEMVideoSource,vtkBKOEMVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 

  /*!
    This is a singleton pattern New.  There will only be ONE
    reference to a vtkOutputWindow object per process.  Clients that
    call this must call Delete on the object so that the reference
    counting will work.   The single instance will be unreferenced when
    the program exits.
  */
  static vtkBKOEMVideoSource* New();
  
  /*! Return the singleton instance with no reference counting. */
  static vtkBKOEMVideoSource* GetInstance();

  /*!
    Supply a user defined output window. Call ->Delete() on the supplied
    instance after setting it.
  */
  static void SetInstance(vtkBKOEMVideoSource *instance);

protected:
  /*! Constructor */
  vtkBKOEMVideoSource();
  /*! Destructor */
  virtual ~vtkBKOEMVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*!
    Record incoming video.  The recording
    continues indefinitely until StopRecording() is called. 
  */
  virtual PlusStatus InternalStartRecording();

  /*! Stop recording or playing */
  virtual PlusStatus InternalStopRecording();

  /*! Get the last error string returned by Ulterius */
  std::string GetLastUlteriusError();

  ////////////////////////
    
private:
 
  static vtkBKOEMVideoSource* Instance;

  static bool vtkBKOEMVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum);
  vtkBKOEMVideoSource(const vtkBKOEMVideoSource&);  // Not implemented.
  void operator=(const vtkBKOEMVideoSource&);  // Not implemented.

  PlusBKOEMReceiver* DataReceiver;
};

#endif