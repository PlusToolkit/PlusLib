/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualStreamDiscCapture_h
#define __vtkVirtualStreamDiscCapture_h

#include "vtkMetaImageSequenceIO.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStream.h"
#include <string>

/*!
\class vtkVirtualStreamDiscCapture 
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualStreamDiscCapture : public vtkPlusDevice
{
public:
  static vtkVirtualStreamDiscCapture *New();
  vtkTypeRevisionMacro(vtkVirtualStreamDiscCapture, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  virtual PlusStatus InternalUpdate();

  virtual PlusStatus NotifyConfigured();

  /*! Output file name */
  virtual void SetFilename(const char* filename) { m_Filename=filename; }

  /*! Open the output file for writing */
  virtual PlusStatus OpenFile();

  /*! Close the output file */
  virtual PlusStatus CloseFile();

  /*! Enables capturing frames. It can be used for pausing the recording. */
  vtkGetMacro(EnableCapturing, bool);
  vtkSetMacro(EnableCapturing, bool);

protected:
  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  virtual PlusStatus BuildNewTrackedFrameList();

  /*! Read the sequence metafile, re-write it with compression */
  PlusStatus CompressFile();

  vtkVirtualStreamDiscCapture();
  virtual ~vtkVirtualStreamDiscCapture();

  /*! Recorded tracked frame list */
  vtkTrackedFrameList* m_RecordedFrames;

  /*! Timestamp of last recorded frame (the tracked frames acquired since this timestamp will be recorded) */
  double m_LastRecordedFrameTimestamp;

  /*! File to write */
  std::string m_Filename;

  /*! Meta sequence to write to */
  vtkMetaImageSequenceIO* m_Writer;

  /*! When closing the file, re-read the data from file, and write it compressed */
  bool m_EnableFileCompression;

  /*! Preparing the header requires image data already collected, this flag makes the header preparation wait until valid data is collected */
  bool m_HeaderPrepared;

  /*! Record the number of frames captured */
  long int m_TotalFramesRecorded;  // hard drive will probably fill up before a regular int is hit, but still...

  bool EnableCapturing;

  /*! Mutex instance simultaneous access of writer (writer may be accessed from command processing thread and also the internal update thread) */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> WriterAccessMutex;

private:
  vtkVirtualStreamDiscCapture(const vtkVirtualStreamDiscCapture&);  // Not implemented.
  void operator=(const vtkVirtualStreamDiscCapture&);  // Not implemented. 
};

#endif
