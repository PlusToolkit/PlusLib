/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntuitiveDaVinciTracker_h
#define __vtkPlusIntuitiveDaVinciTracker_h

// Local includes
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// VTK includes
#include <vtkObjectFactory.h>

// Intuitive includes
#include "IntuitiveDaVinci.h"

class vtkMatrix4x4;
class IntuitiveDaVinci;
class vtkPlusIntuitiveDaVinciTracker;

/* This class talks with the da Vinci Surgical System. */
class vtkPlusDataCollectionExport vtkPlusIntuitiveDaVinciTracker : public vtkPlusDevice
{
public:
  static vtkPlusIntuitiveDaVinciTracker* New();
  vtkTypeMacro(vtkPlusIntuitiveDaVinciTracker, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const { return true; }

  /*! Probe to see if the tracking system is present. */
  virtual PlusStatus Probe();

  /*! Read da Vinci configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Write current da Vinci configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  IntuitiveDaVinci* GetDaVinci() const;

protected:
  vtkPlusIntuitiveDaVinciTracker();
  ~vtkPlusIntuitiveDaVinciTracker();

  /*! Connect to the tracker hardware */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from the tracker hardware */
  virtual PlusStatus InternalDisconnect();

  /*!  Start the tracking system. */
  virtual PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  virtual PlusStatus InternalStopRecording();

  /*! Update method */
  PlusStatus InternalUpdate();

  vtkSetMacro(Psm1DhTable, std::string);
  vtkSetMacro(Psm2DhTable, std::string);
  vtkSetMacro(EcmDhTable, std::string);
  vtkSetMacro(DebugSineWaveMode, bool);

protected:
  /*! Pointer to the MicronTrackerInterface class instance */
  IntuitiveDaVinci*   DaVinci;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long       LastFrameNumber;
  unsigned long       FrameNumber;

  std::string Psm1DhTable;
  std::string Psm2DhTable;
  std::string EcmDhTable;

  bool DebugSineWaveMode;

private:
  vtkPlusIntuitiveDaVinciTracker(const vtkPlusIntuitiveDaVinciTracker&);
  void operator=(const vtkPlusIntuitiveDaVinciTracker&);

  void ConvertIsiTransformToVtkMatrix(ISI_TRANSFORM* isiMatrix, vtkMatrix4x4& vtkMatrix);
  PlusStatus SetDhTablesFromStrings();
  void vtkPlusIntuitiveDaVinciTracker::ProcessDhString(std::string& str) const;

  vtkPlusDataSource* psm1Base;
  vtkPlusDataSource* psm2Base;
  vtkPlusDataSource* ecmBase;

  vtkPlusDataSource* psm1Frame1;
  vtkPlusDataSource* psm1Frame2;
  vtkPlusDataSource* psm1Frame3;
  vtkPlusDataSource* psm1Frame4;
  vtkPlusDataSource* psm1Frame5;
  vtkPlusDataSource* psm1Frame6;
  vtkPlusDataSource* psm1Frame7;

  vtkPlusDataSource* psm2Frame1;
  vtkPlusDataSource* psm2Frame2;
  vtkPlusDataSource* psm2Frame3;
  vtkPlusDataSource* psm2Frame4;
  vtkPlusDataSource* psm2Frame5;
  vtkPlusDataSource* psm2Frame6;
  vtkPlusDataSource* psm2Frame7;

  vtkPlusDataSource* ecmFrame1;
  vtkPlusDataSource* ecmFrame2;
  vtkPlusDataSource* ecmFrame3;
  vtkPlusDataSource* ecmFrame4;
  vtkPlusDataSource* ecmFrame5;
  vtkPlusDataSource* ecmFrame6;
  vtkPlusDataSource* ecmFrame7;
};

#endif
