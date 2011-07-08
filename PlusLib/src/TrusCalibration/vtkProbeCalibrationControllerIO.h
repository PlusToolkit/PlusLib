#ifndef __VTKPROBECALIBRATIONCONTROLLERIO_H
#define __VTKPROBECALIBRATIONCONTROLLERIO_H

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkXMLDataElement.h"
#include <vector>

#include "itkImage.h"

class vtkProbeCalibrationController; 


class vtkProbeCalibrationControllerIO : public vtkObject
{

public:
	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;

	static vtkProbeCalibrationControllerIO *New();
	vtkTypeRevisionMacro(vtkProbeCalibrationControllerIO, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 
	
	//! Description
	// Initialize the ProbeCalibrationController with the calibration controller instance
	virtual void Initialize( vtkProbeCalibrationController* calibrationController ); 

	//! Description
	// Draw the segmentation result to each individual image and save them into TIFF files
	virtual void SaveSegmentationResultToImage( const int imgNumber, const ImageType::Pointer& frame );

	//! Description
	// Save back projected wire positions to text file in gnuplot format 
	virtual void SaveSegmentedWirePositionsToFile(); 

	//! Description
	// Read probe calibration data element 
	virtual void ReadProbeCalibrationConfiguration(vtkXMLDataElement* probeCalibration); 

	//! Description
	// File the calibration results 
	// This operation writes the final calibration results to a file.
	virtual void SaveCalibrationResultsAndErrorReportsToXML(); 

	//! Description
	// Read in the ultrasound 3D beam profile data from a file
	virtual void ReadUs3DBeamwidthDataFromFile(); 
	
	//! Description
	// Load the ultrasound 3D beam profile data
	virtual void LoadUS3DBeamProfileData();

protected:
	vtkProbeCalibrationControllerIO();
	virtual ~vtkProbeCalibrationControllerIO();

	//! Attribute: a reference to the calibration controller
	vtkProbeCalibrationController* CalibrationController; 
private:
	vtkProbeCalibrationControllerIO(const vtkProbeCalibrationControllerIO&);
	void operator=(const vtkProbeCalibrationControllerIO&);
}; 

#endif // __VTKPROBECALIBRATIONCONTROLLERIO_H
