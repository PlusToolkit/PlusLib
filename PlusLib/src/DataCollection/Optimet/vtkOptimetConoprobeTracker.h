/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOptimetConoprobeTracker_h
#define __vtkOptimetConoprobeTracker_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
\class vtkOptimetConoProbeTracker 
\brief Interface for the Optimet ConoProbe

This class talks with Optimet ConoProbe over the Optimet Smart32 SDK

Requires PLUS_USE_OPTIMET_CONOPROBE option in CMake.

\ingroup PlusLibDataCollection
*/

class ISmart;

class vtkDataCollectionExport vtkOptimetConoProbeTracker : public vtkPlusDevice
{
public:

  static vtkOptimetConoProbeTracker *New();
  vtkTypeMacro( vtkOptimetConoProbeTracker,vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual bool IsTracker() const { return true; }

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Set frequency. */
  PlusStatus SetFrequency(int frequency);

protected:

  vtkOptimetConoProbeTracker();
  ~vtkOptimetConoProbeTracker();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
  PlusStatus InternalUpdate();

private:  // Functions.

  vtkOptimetConoProbeTracker( const vtkOptimetConoProbeTracker& );
  void operator=( const vtkOptimetConoProbeTracker& );

  /*! Get composite laser power. */
  unsigned short GetCompositeLaserPower();  

private:  // Variables.

	vtkPlusDataSource* DistanceTool;	

  /*! ConoProbe device handle. */
  ISmart* ConoProbe;

  /*! Delay between measurements. */
  UINT16 DelayBetweenMeasurements;

  /*! Frequency. */
  UINT16 Frequency;

  /*! Coarse laser power. */
  unsigned short CoarseLaserPower;

  /*! Fine laser power. */
  unsigned short FineLaserPower;
};

#endif