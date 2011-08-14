#ifndef __CALIBRATORVISUALIZATIONCOMPONENT_H
#define __CALIBRATORVISUALIZATIONCOMPONENT_H

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkCollection.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkXMLDataElement.h"
#include <vector>

class vtkProbeCalibrationController; 

class vtkIntArray;
class vtkDoubleArray;
class vtkFieldData;
class vtkDataObject;
class vtkXYPlotActor;
class vtkLookupTable;
class vtkPNGWriter;
class vtkScalarBarActor;
class vtkImageViewer;

enum US_IMAGING_MODE
{
	TRANSVERSE, 
	LONGITUDINAL
}; 

enum TEMPLATE_HOLE_REPRESENTATION
{
	HOLE_DISK, 
	HOLE_SPHERE
}; 

enum TEMPLATE_LETTER_TYPE
{
	LETTER_CHAR, 
	LETTER_NUM
};

struct TemplateModel
{
	std::string ID; 
	double PositionX; 
	double PositionY; 
	double PositionZ; 
	TEMPLATE_HOLE_REPRESENTATION Representation; 
	TEMPLATE_LETTER_TYPE LetterType; 
	double Radius;

}; 

struct TemplateGridActors
{
	vtkSmartPointer<vtkCollection> TransverseGridActors;
	vtkSmartPointer<vtkCollection> LongitudinalGridActors;
	vtkSmartPointer<vtkCollection> TransverseLetterActors; 
	vtkSmartPointer<vtkCollection> LongitudinalLetterActors;
}; 

class vtkCalibratorVisualizationComponent : public vtkObject
{

public:

	static vtkCalibratorVisualizationComponent *New();
	vtkTypeRevisionMacro(vtkCalibratorVisualizationComponent, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 
	virtual void Initialize( vtkProbeCalibrationController* calibrationController ); 
	virtual PlusStatus AddFrameToRealtimeRenderer(vtkImageData* frame); 
	virtual void SaveFrameToFile(vtkImageData* frame, char* fileName); 
	virtual void OverlayTemplate(); 
	virtual void RemoveTemplate(); 

	virtual void OverlayCenterOfRotation(); 
	virtual void HideCenterOfRotation() { this->CenterOfRotationActor->VisibilityOff(); }; 
	virtual void ShowCenterOfRotation() { this->CenterOfRotationActor->VisibilityOn();}; 

	virtual void OverlayPhantomWires(); 
	virtual void HidePhantomWires() { this->PhantomWiresActor->VisibilityOff(); }; 
	virtual void ShowPhantomWires() { this->PhantomWiresActor->VisibilityOn(); }; 


	// This operation will draw the 3D Point Reconstruction Error (PRE3D)
	// distribution in realtime immediately after the validation
	// is performed on the calibration iteration.
	virtual void PlotPRE3Ddistribution(); 

	// Update the plot components when the iterative calibration is in progress
	virtual void UpdatePlotComponents(); 

	// Save the plot window to an image (i.e., in PNG format)
	virtual void SavePRE3DplotToImage(); 

	//! Operations: map the PRE3D distribution on US image
	// IMPORTANT:
	// We use the defined lookup table to display the PRE3D
	// distributions on the US image at the positions of the
	// validation data set, for X, Y and Z axis, respectively:
	// NOTE: the range value is an approximated value.
	// - Yellow:		     PRE3D <= 0.5 mm
	// - Green:		0.5 mm < PRE3D <= 1.0 mm
	// - Cyan:		1.0 mm < PRE3D <= 1.5 mm
	// - Blue:		1.5 mm < PRE3D <= 2.0 mm
	// - Magenta:	2.0 mm < PRE3D <= 2.5 mm
	// - Red:		2.5 mm < PRE3D
	virtual void MapPRE3DdistributionToUSImage ( unsigned char* imageData );

	vtkGetObjectMacro(PlotRenderer, vtkRenderer);
	vtkSetObjectMacro(PlotRenderer, vtkRenderer);

	vtkGetObjectMacro(RealtimeRenderer, vtkRenderer);
  void SetRealtimeRenderer( vtkRenderer* renderer ); 


	vtkGetObjectMacro(ImageCamera, vtkCamera);
	vtkSetObjectMacro(ImageCamera, vtkCamera);

	vtkGetObjectMacro(TemplateCamera, vtkCamera);
	vtkSetObjectMacro(TemplateCamera, vtkCamera);

	vtkGetObjectMacro(RealtimeImageActor, vtkImageActor);
	vtkSetObjectMacro(RealtimeImageActor, vtkImageActor);

	vtkGetObjectMacro(CenterOfRotationActor, vtkActor);
	vtkSetObjectMacro(CenterOfRotationActor, vtkActor);

	vtkGetObjectMacro(PhantomWiresActor, vtkActor);
	vtkSetObjectMacro(PhantomWiresActor, vtkActor);
	
	vtkGetObjectMacro(TransformVtkImageToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformVtkImageToTemplate, vtkTransform);
			
	vtkGetObjectMacro(CalibrationController, vtkProbeCalibrationController);
	
	vtkGetStringMacro(OutputPath); 
	vtkSetStringMacro(OutputPath); 

	vtkGetStringMacro(TemplateModelConfigFileName); 
	vtkSetStringMacro(TemplateModelConfigFileName); 

	//! Description 
	// Flag to show the initialized state
	vtkGetMacro(Initialized, bool);
	vtkSetMacro(Initialized, bool);
	vtkBooleanMacro(Initialized, bool);
	
	// Color map to display PRE3D distribution
	// - Green:		         PRE3D <= 0.5 mm
	// - Yellow:	0.5 mm < PRE3D <= 1.0 mm
	// - Blue:		1.0 mm < PRE3D <= 2.0 mm
	// - Magenta:	2.0 mm < PRE3D <= 3.0 mm
	// - Red:		3.0 mm < PRE3D
	static const int mColorGreen[3];
	static const int mColorYellow[3];
	static const int mColorBlue[3];
	static const int mColorCyan[3];
	static const int mColorRed[3];

protected:
	vtkCalibratorVisualizationComponent();
	virtual ~vtkCalibratorVisualizationComponent();

	virtual void SetupDataObjects(); 
	virtual void SetupPlotActors(); 
	virtual void SetupColorLUT(); 
	virtual void SetupColorBars(); 
	virtual void SetupImageViewers(); 
	virtual void SetupTemplateOverlay(); 

	virtual void CreateTemplateGridActors(); 

	virtual void DisplayTemplateGridInLongitudinalMode(); 
	virtual void DisplayTemplateGridInTransverseMode(); 
	virtual void DisplayTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors); 
	
	virtual void RemoveTemplateGridInLongitudinalMode(); 
	virtual void RemoveTemplateGridInTransverseMode(); 
	virtual void RemoveTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors); 
	
	virtual void ReadTemplateModelConfiguration(vtkXMLDataElement *configTemplateModel); 
	
	//! Operation: manually map the PRE3D value to a RGB value
	// This is a manual mapping the colors (5 of them) to
	// the PRE3D value.
	// Note: Do not use unless absolutely necessary.  This 
	// method generates color space that is much less sampled
	// than that of MapPRE3DdistributionToUSImage() which uses 
	// a vtk lookup table for more smooth color mapping.
	virtual void GetColorForPRE3Ddistribution( const double PRE3DabsValue, int* ColorInRGBarray ); 

	virtual void PlotPRE3D(); 

	//! Attributes: PRE3D on US image visualization components
	vtkLookupTable* mptrPRE3DColorLookupTable;
	vtkScalarBarActor* mptrPRE3DScalarBarActorX;
	vtkScalarBarActor* mptrPRE3DScalarBarActorY;
	vtkScalarBarActor* mptrPRE3DScalarBarActorZ;
	vtkImageViewer* mptrPRE3DonUSImageViewerX;
	vtkImageViewer* mptrPRE3DonUSImageViewerY;
	vtkImageViewer* mptrPRE3DonUSImageViewerZ;
	vtkRenderer* PlotRenderer;		


	//! Attributes: Realtime renderer visualization components
	vtkRenderer* RealtimeRenderer; 
	vtkImageActor* RealtimeImageActor; 
	vtkCamera* ImageCamera; 
	vtkCamera* TemplateCamera; 

	//! Attributes: PRE3D Plot pipeline components
	vtkIntArray* mptrNumberOfData; 
	vtkDoubleArray* mptrPRE3DinXAxis;
	vtkDoubleArray* mptrPRE3DinYAxis;
	vtkDoubleArray* mptrPRE3DinZAxis;
	vtkFieldData* mptrFieldDataPRE3DinXAxis;
	vtkFieldData* mptrFieldDataPRE3DinYAxis;
	vtkFieldData* mptrFieldDataPRE3DinZAxis;
	vtkDataObject* mptrDataObjectPRE3DinXAxis;
	vtkDataObject* mptrDataObjectPRE3DinYAxis;
	vtkDataObject* mptrDataObjectPRE3DinZAxis;
	vtkXYPlotActor* mptrPlotActor;
	vtkLookupTable* mptrPlotLookupTable;

	//! Attribute: a reference to the calibration controller
	vtkProbeCalibrationController* CalibrationController; 

	//! Attribute: Flag to show the initialized state
	bool Initialized; 

	char *OutputPath; 
	char* TemplateModelConfigFileName; 

	TemplateGridActors TemplateGridActors; 

	vtkActor* CenterOfRotationActor; 

	vtkActor* PhantomWiresActor; 

	vtkTransform* TransformVtkImageToTemplate; 
	
	std::vector<TemplateModel> TemplateModelLetters; 
	std::vector<TemplateModel> TemplateModelHoles; 

private:
	vtkCalibratorVisualizationComponent(const vtkCalibratorVisualizationComponent&);
	void operator=(const vtkCalibratorVisualizationComponent&);

}; 

#endif //__CALIBRATORVISUALIZATIONCOMPONENT_H
