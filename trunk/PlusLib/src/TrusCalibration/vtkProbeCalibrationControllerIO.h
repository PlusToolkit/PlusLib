#ifndef __VTKPROBECALIBRATIONCONTROLLERIO_H
#define __VTKPROBECALIBRATIONCONTROLLERIO_H

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkXMLDataElement.h"
#include <vector>

#include "itkImage.h"

class vtkCalibrationController; 
class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 


class vtkProbeCalibrationControllerIO : public vtkObject
{

public:
	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;

	static vtkProbeCalibrationControllerIO *New();
	vtkTypeRevisionMacro(vtkProbeCalibrationControllerIO, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 
	
	// Initialize the CalibrationController with the calibration controller instance
	virtual void Initialize( vtkCalibrationController* calibrationController ); 

	// Read probe calibration data element 
	virtual PlusStatus ReadProbeCalibrationConfiguration(vtkXMLDataElement* rootElement); 

	// Draw the segmentation result to each individual image and save them into TIFF files
	virtual void SaveSegmentationResultToImage( const int imgNumber, const ImageType::Pointer& frame );

	// Save back projected wire positions to text file in gnuplot format 
	virtual void SaveSegmentedWirePositionsToFile(); 

	// File the calibration results 
	// This operation writes the final calibration results to a file.
	virtual void SaveCalibrationResultsAndErrorReportsToXML(); 

	// Read in the ultrasound 3D beam profile data from a file
	virtual void ReadUs3DBeamwidthDataFromFile(); 
	
	// Load the ultrasound 3D beam profile data
	virtual void LoadUS3DBeamProfileData();

	// Add generated html report from final calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateProbeCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

protected:
	vtkProbeCalibrationControllerIO();
	virtual ~vtkProbeCalibrationControllerIO();

	//! Attribute: a reference to the calibration controller
	vtkCalibrationController* CalibrationController;

private:
	vtkProbeCalibrationControllerIO(const vtkProbeCalibrationControllerIO&);
	void operator=(const vtkProbeCalibrationControllerIO&);
}; 

#endif // __VTKPROBECALIBRATIONCONTROLLERIO_H
