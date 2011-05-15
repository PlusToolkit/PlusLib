#ifndef __itkMetaImageSequenceIO_h
#define __itkMetaImageSequenceIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif 

#define PLUS_EXPORTS

#include "vtkPlusWin32Header.h"
#include "itkMetaImageIO.h"
#include "vtkMatrix4x4.h"

// The following coordinate frames are used with this metafile: 
// Image: image frame coordinate system, origin is the bottom-left corner, unit is pixel
// Tool: coordinate system of the DRB attached to the probe, unit is mm
// Reference: coordinate system of the DRB attached to the reference body, unit is mm
// Tracker: coordinate system of the tracker, unit is mm
// World: world coordinate system, orientation is usually patient RAS, unit is mm
//
// ImageToToolTransform: calibration matrix
// ToolToTrackerTransform: for each frame, pose of the image in the tracker coordinate system
// ReferenceToTrackerTransform: for each frame, pose of the reference body in the tracker coordinate system
// TrackerToWorldTransform: rarely used 

namespace itk
{
	enum SEQUENCE_IO_TYPE
	{
		IO_READ, 
		IO_WRITE
	};

	/** \class MetaImageIO
	*
	*  \brief Read MetaImage file format.
	*
	*  \ingroup IOFilters
	*/
	class PLUS_EXPORT MetaImageSequenceIO : public itk::MetaImageIO
	{
	public:

		// Standard class typedefs. 
		typedef MetaImageSequenceIO	Self;
		typedef MetaImageIO        	Superclass;
		typedef SmartPointer<Self> 	Pointer; 

		typedef std::map<std::string,std::string> UserFieldMap; 

		// Method for creation through the object factory. 
		itkNewMacro(Self);

		// Run-time type information (and related methods).
		itkTypeMacro(MetaImageSequenceIO, Superclass); 

		// Set/get the default frame transform name 
		itkSetStringMacro(DefaultFrameTransformName); 
		itkGetStringMacro(DefaultFrameTransformName); 

		// Set/get the Ultrasound Image Orientation
		// The ultrasound image axes are defined as follows:
		// - x axis: points towards the x coordinate increase direction
		// - y axis: points towards the y coordinate increase direction
		// The image orientation can be defined by specifying which transducer axis corresponds to the x and y image axes, respectively.
		// There are four possible orientations:
		// - UF: image x axis = unmarked transducer axis, image y axis = far transducer axis
		// - UN: image x axis = unmarked transducer axis, image y axis = near transducer axis
		// - MF: image x axis = marked transducer axis, image y axis = far transducer axis
		// - MN: image x axis = marked transducer axis, image y axis = near transducer axis
		virtual bool SetUltrasoundImageOrientation( const char* strOrientation); 
		itkGetStringMacro(UltrasoundImageOrientation); 

		// Get custom field names
		virtual std::vector<std::string> GetCustomFieldNames() { return m_CustomFieldNamesForReading; }

		// Get custom frame field names 
		virtual std::vector<std::string> GetCustomFrameFieldNames() { return m_CustomFrameFieldNamesForReading; } 

		// Set the spacing and dimension information for the set filename. 
		virtual void ReadImageInformation();

		// Writes the data to disk from the memory buffer provided. Make sure
		// that the IORegions has been set properly. 
		virtual void Write(const void* buffer);

		// Add custom field name that the reader should recognize
		void AddCustomFieldNameForReading(const char* fieldName); 

		// Get the value of the custom field
		// If we couldn't find it, return NULL
		virtual const char* GetCustomString( const char* fieldName ); 

		// Set custom string value to <fieldValue>
		virtual bool SetCustomString(const char* fieldName, const char* fieldValue); 

		// Get the custom transformation matrix from metafile by custom frame transform name
		// It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
		// Return false if the the field is missing 
		virtual bool GetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
		virtual bool GetCustomTransform( const char* frameTransformName, double* transformMatrix ); 

		// Set the custom transformation matrix from metafile by custom frame transform name
		// It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
		virtual void SetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
		virtual void SetCustomTransform( const char* frameTransformName, double* transformMatrix ); 

		//************************************************************************************

		// Add custom field name sequence that the reader should recognize
		// It will generate a field like: Seq_Frame0000_<fieldName>
		virtual void AddCustomFrameFieldNameForReading(const char* fieldName);  

		// Get the value of the custom frame field
		// If we couldn't find it, return NULL
		// It will search for a field like: Seq_Frame<frameNumber>_<fieldName>
		virtual const char* GetCustomFrameString( int frameNumber, const char* fieldName ); 

		// Set custom frame string value to <fieldValue>
		// It will search for a field like: Seq_Frame<frameNumber>_<fieldName>
		virtual bool SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue); 

		// Get the custom frame transformation matrix from metafile by frame number and custom frame transform name
		// It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
		// Return false if the the field is missing 
		virtual bool GetCustomFrameTransform( const int frameNumber, const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
		virtual bool GetCustomFrameTransform( const int frameNumber, const char* frameTransformName, double* transformMatrix ); 

		// Set the custom frame transformation matrix from metafile by frame number and custom frame transform name
		// It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
		virtual void SetCustomFrameTransform( const int frameNumber, const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
		virtual void SetCustomFrameTransform( const int frameNumber, const char* frameTransformName, double* transformMatrix ); 

		// Get the default frame transformation matrix from metafile by frame number 
		// It will search for a field like: Seq_Frame<frameNumber>_<DefaultFrameTransformName>
		// Return false if the the field is missing 
		virtual bool GetFrameTransform( const int frameNumber, vtkMatrix4x4* transformMatrix ); 
		virtual bool GetFrameTransform( const int frameNumber, double* transformMatrix ); 

		// Set the default frame transformation matrix from metafile by frame number
		// It will search for a field like: Seq_Frame<frameNumber>_<DefaultFrameTransformName>
		virtual void SetFrameTransform( const int frameNumber, vtkMatrix4x4* transformMatrix ); 
		virtual void SetFrameTransform( const int frameNumber, double* transformMatrix ); 

	protected:
		MetaImageSequenceIO();
		virtual ~MetaImageSequenceIO();
		void PrintSelf(std::ostream& os, Indent indent) const;

		// Read the number of frames from the header to create user field sequences
		virtual int ReadNumberOfFramesFromMetaData(const char *_fileName); 

		// Clear and add userfields from UserFieldMap to the MetaObject UserFields
		virtual void SetupUserFields(SEQUENCE_IO_TYPE ioType);

	private:
		typedef METAIO_STL::vector<MET_FieldRecordType *> FieldsContainerType;

		MetaImage *m_MetaImage;
		std::string m_DefaultFrameTransformName; 
		std::string m_UltrasoundImageOrientation; 

		std::vector<std::string> m_CustomFieldNamesForReading; 
		std::vector<std::string> m_CustomFrameFieldNamesForReading; 
		UserFieldMap m_UserFieldMap; 

		MetaImageSequenceIO(const Self&); //purposely not implemented
		void operator=(const Self&); //purposely not implemented

	};

} // end namespace itk

#endif // __itkMetaImageSequenceIO_h 