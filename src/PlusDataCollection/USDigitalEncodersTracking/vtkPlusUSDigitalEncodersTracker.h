/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _vtkUSDigitalA2EncodersTracker_h_
#define _vtkUSDigitalA2EncodersTracker_h_

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkIGSIOTransformRepository.h"

#include <map>

/*!
   \class vtkPlusUSDigitalEncodersTracker
   \brief Interface for multiple US Digital A2, A2T, A4, HBA2, HBA4 or HD25A encoders to generate pose information of a target object.

   This class communicates with multiple US Digital encoders through SEI (Serial Encoder Interface Bus) provided by US Digital.

   IDs are assigned to devices based on serial numbers: lower SN -> lower ID. IDs start at 0.

   \ingroup PlusLibDataCollection
 */

class vtkPlusDataCollectionExport vtkPlusUSDigitalEncodersTracker : public vtkPlusDevice
{
public:
  static const long INVALID_SEI_ADDRESS = -1;

  static vtkPlusUSDigitalEncodersTracker* New();
  vtkTypeMacro(vtkPlusUSDigitalEncodersTracker, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool IsTracker() const
  {
    return true;
  }

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*!
     Probe to see if the tracking system is present on the specified serial port.
     If the SerialPort is set to -1, then all serial ports will be checked.
   */
  PlusStatus Probe();

  /*!
     Get an update from the multiple USDigital encoders and push the new transforms
     to the tools.  This should only be used within vtkTracker.cxx.
     This method is called by the tracker thread.
   */
  PlusStatus InternalUpdate();

  /*! Return whether stepper is alive */
  virtual PlusStatus IsStepperAlive();

  /*! If the A2 is in strobe mode, it will take a position reading after receiving this command.*/
  PlusStatus SetUSDigitalA2EncodersStrobeMode();

  /*! Makes all A2's on the SEI bus go to sleep, the current consumption then drops below 0.6 mA / device */
  PlusStatus SetUSDigitalA2EncodersSleep();

  /*! Function: wakes up all A2's on the SEI bus, wait at least 5mSec before sending the next command */
  PlusStatus SetUSDigitalA2EncodersWakeup();

  /*! Sets the absolute zero to the current position, in single-turn mode the new position is stored in EEPROM */
  PlusStatus SetUSDigitalA2EncoderOriginWithID(long id);

  /*! Sets the absolute zero to the current position of all connected US Digital A2 Encoders*/
  PlusStatus SetAllUSDigitalA2EncoderOrigin();

  /*! Sets the mode of an A2 Encoder */
  PlusStatus SetUSDigitalA2EncoderModeWithID(long id, long mode);

  /*! Gets the mode of an A2 Encoder */
  PlusStatus GetUSDigitalA2EncoderModeWithID(long id, long* mode);

  /*! Sets the resolution of an A2 Encoder */
  PlusStatus SetUSDigitalA2EncoderResoultionWithID(long id, long res);

  /*! Gets the resolution of an A2 Encoder */
  PlusStatus GetUSDigitalA2EncoderResoultionWithID(long id, long* res);

  /*! Sets the Position of an A2 Encoder */
  PlusStatus SetUSDigitalA2EncoderPositionWithID(long id, long pos);

  /*! Gets the Position of an A2 Encoder */
  PlusStatus GetUSDigitalA2EncoderPositionWithID(long id, long* pos);

protected:
  vtkPlusUSDigitalEncodersTracker();
  ~vtkPlusUSDigitalEncodersTracker();

  /*!
     Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
     The device will only be reset if communication cannot be established without a reset.
   */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalStopRecording();

  class vtkPlusUSDigitalEncoderInfo;

protected:
  vtkSmartPointer<vtkIGSIOTransformRepository> TransformRepository
    = vtkSmartPointer<vtkIGSIOTransformRepository>::New();

  typedef std::map<long, vtkPlusUSDigitalEncoderInfo*> EncoderInfoMapType;
  EncoderInfoMapType EncoderMap;
  typedef std::list<vtkPlusUSDigitalEncoderInfo> EncoderListType;
  EncoderListType EncoderList;
  typedef std::map<long, long> IDtoAddressType;
  IDtoAddressType IdAddress;


public:
  vtkPlusUSDigitalEncodersTracker(const vtkPlusUSDigitalEncodersTracker&) = delete;
  void operator=(const vtkPlusUSDigitalEncodersTracker&) = delete;
};

#endif