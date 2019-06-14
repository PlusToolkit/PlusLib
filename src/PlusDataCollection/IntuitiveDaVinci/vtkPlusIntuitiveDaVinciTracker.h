/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntuitiveDaVinciTracker_h
#define __vtkPlusIntuitiveDaVinciTracker_h

#include <vtkObjectFactory.h>

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

#include "IntuitiveDaVinci.h"

/* This class talks with the da Vinci Surgical System via the class IntuitiveDaVinci. */
class vtkPlusDataCollectionExport vtkPlusIntuitiveDaVinciTracker : public vtkPlusDevice
{
public:
  static vtkPlusIntuitiveDaVinciTracker* New();
  vtkTypeMacro(vtkPlusIntuitiveDaVinciTracker, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual PlusStatus NotifyConfigured();

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

  /*! Connect to the da Vinci API*/
  virtual PlusStatus InternalConnect();

  /*! Disconnect from the da Vinci API */
  virtual PlusStatus InternalDisconnect();

  /*!  Start the streaming of kinematics data and/or events. */
  virtual PlusStatus InternalStartRecording();

  /*! Stop the system and bring it back to its initial state. */
  virtual PlusStatus InternalStopRecording();

  /*! Update method for updating joint values, base frames, and kinematics transforms. */
  virtual PlusStatus InternalUpdate();

  vtkSetMacro(DebugSineWaveMode, bool);

protected:
  /*! Pointer to the IntuitiveDaVinci class instance */
  IntuitiveDaVinci*   DaVinci;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long       LastFrameNumber;

  /*! These are some additional flags that we can load from the xml to put the system into different modes. */
  bool DebugSineWaveMode;

private:
  vtkPlusIntuitiveDaVinciTracker(const vtkPlusIntuitiveDaVinciTracker&);
  void operator=(const vtkPlusIntuitiveDaVinciTracker&);

  /*! Convert very explicitly between the two represeations of transforms. */
  static inline void ConvertIsiTransformToVtkMatrix(ISI_TRANSFORM* isiMatrix, vtkMatrix4x4& vtkMatrix);

  /*! From three strings (likely obtained from the xml), set the robot DH tables. */
  PlusStatus SetDhTablesFromStrings(std::string psm1DhTable, std::string psm2DhTable, std::string ecmDhTable);

private:
  /*************** ROBOT JOINT VALUES ***************/

  /*! The 7 + 7 + 4 = 18 joint values of all manipulators stored and broadcasted in a string. */
  vtkPlusDataSource* jointValues;

  /*************** ROBOT BASE TRANSFORMS ***************/

  /*! Transform from PSM1 Base frame to the da Vinci world frame. */
  vtkPlusDataSource* psm1Base;

  /*! Transform from PSM2 Base frame to the da Vinci world frame. */
  vtkPlusDataSource* psm2Base;

  /*! Transform from ECM Base frame to the da Vinci world frame. */
  vtkPlusDataSource* ecmBase;

  /*************** PSM1 LINK TRANSFORMS ***************/

  /*! Transform from Frame1 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame1;

  /*! Transform from Frame2 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame2;

  /*! Transform from Frame3 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame3;

  /*! Transform from Frame4 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame4;

  /*! Transform from Frame5 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame5;

  /*! Transform from Frame6 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame6;

  /*! Transform from Frame7 of PSM1 to PSM1 Base. */
  vtkPlusDataSource* psm1Frame7;

  /*************** PSM2 LINK TRANSFORMS ***************/

  /*! Transform from Frame1 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame1;

  /*! Transform from Frame2 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame2;

  /*! Transform from Frame3 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame3;

  /*! Transform from Frame4 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame4;

  /*! Transform from Frame5 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame5;

  /*! Transform from Frame6 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame6;

  /*! Transform from Frame7 of PSM2 to PSM2 Base. */
  vtkPlusDataSource* psm2Frame7;

  /*************** ECM LINK TRANSFORMS ***************/

  /*! Transform from Frame1 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame1;

  /*! Transform from Frame2 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame2;

  /*! Transform from Frame3 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame3;

  /*! Transform from Frame4 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame4;

  /*! Transform from Frame5 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame5;

  /*! Transform from Frame6 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame6;

  /*! Transform from Frame7 of ECM to ECM Base. */
  vtkPlusDataSource* ecmFrame7;
};

// Macro to publish an isiTransform to a given tool. 
#define PUBLISH_ISI_TRANSFORM(tool, isiTransform) \
  if(tool!=NULL) \
  { \
    ConvertIsiTransformToVtkMatrix(isiTransform, *tmpVtkMatrix); \
    unsigned long frameNumber = tool->GetFrameNumber() + 1; \
    ToolTimeStampedUpdate(tool->GetId(), tmpVtkMatrix, TOOL_OK, frameNumber, toolTimestamp); \
  } \

#endif
