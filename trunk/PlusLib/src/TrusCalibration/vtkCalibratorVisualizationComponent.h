#ifndef __CALIBRATORVISUALIZATIONCOMPONENT_H
#define __CALIBRATORVISUALIZATIONCOMPONENT_H

#include "PlusConfigure.h"
//#include "vtkCalibrationController.h"
//#include "vtkProbeCalibrationController.h"
#include "vtkObject.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkCollection.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkXMLDataElement.h"
#include <vector>


class vtkIntArray;
class vtkDoubleArray;
class vtkFieldData;
class vtkDataObject;
class vtkXYPlotActor;
class vtkLookupTable;
class vtkPNGWriter;
class vtkScalarBarActor;
class vtkImageViewer;
class vtkProbeCalibrationController; 

//enum US_IMAGING_MODE
//{
//	TRANSVERSE, 
//	LONGITUDINAL
//}; 
//
//enum TEMPLATE_HOLE_REPRESENTATION
//{
//	HOLE_DISK, 
//	HOLE_SPHERE
//}; 
//
//enum TEMPLATE_LETTER_TYPE
//{
//	LETTER_CHAR, 
//	LETTER_NUM
//};
//
//struct TemplateModel
//{
//	std::string ID; 
//	double PositionX; 
//	double PositionY; 
//	double PositionZ; 
//	TEMPLATE_HOLE_REPRESENTATION Representation; 
//	TEMPLATE_LETTER_TYPE LetterType; 
//	double Radius;
//
//}; 

class vtkCalibratorVisualizationComponent : public vtkObject
{

public:

	static vtkCalibratorVisualizationComponent *New();
	vtkTypeRevisionMacro(vtkCalibratorVisualizationComponent, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 
	virtual void Initialize(); 
	virtual PlusStatus AddFrameToRealtimeRenderer(vtkImageData* frame); 
	virtual void SetTemplateVisibility( bool visibility ); 

	virtual void OverlayCenterOfRotation(); 
	virtual void HideCenterOfRotation() { this->CenterOfRotationActor->VisibilityOff(); }; 
	virtual void ShowCenterOfRotation() { this->CenterOfRotationActor->VisibilityOn();}; 

	virtual void OverlayPhantomWires(); 
	virtual void HidePhantomWires() { this->PhantomWiresActor->VisibilityOff(); }; 
	virtual void ShowPhantomWires() { this->PhantomWiresActor->VisibilityOn(); }; 

  virtual PlusStatus ReadTemplateModelConfiguration(vtkXMLDataElement *configTemplateModel); 

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
  //vtkSetObjectMacro(CalibrationController, vtkProbeCalibrationController);
	
	vtkGetStringMacro(OutputPath); 
	vtkSetStringMacro(OutputPath); 

	//! Description 
	// Flag to show the initialized state
	vtkGetMacro(Initialized, bool);
	vtkSetMacro(Initialized, bool);
	vtkBooleanMacro(Initialized, bool);
	
protected:
	vtkCalibratorVisualizationComponent();
	virtual ~vtkCalibratorVisualizationComponent();

	virtual void SetupTemplateOverlay(); 

	virtual void CreateTemplateGridActors(); 
	
	//! Attributes: Realtime renderer visualization components
	vtkRenderer* RealtimeRenderer; 
	vtkImageActor* RealtimeImageActor; 
	vtkCamera* ImageCamera; 
	vtkCamera* TemplateCamera; 

	//! Attribute: a reference to the calibration controller
	vtkProbeCalibrationController* CalibrationController; 

	//! Attribute: Flag to show the initialized state
	bool Initialized; 

	char *OutputPath; 

  vtkActor* CenterOfRotationActor; 
	vtkActor* PhantomWiresActor; 

	vtkTransform* TransformVtkImageToTemplate; 

	//std::vector<TemplateModel> TemplateModelLetters; 
	//std::vector<TemplateModel> TemplateModelHoles; 

  vtkCollection* TemplateLetterActors;
  vtkActor* TemplateHolesActor; 

private:
	vtkCalibratorVisualizationComponent(const vtkCalibratorVisualizationComponent&);
	void operator=(const vtkCalibratorVisualizationComponent&);

}; 

#endif //__CALIBRATORVISUALIZATIONCOMPONENT_H
