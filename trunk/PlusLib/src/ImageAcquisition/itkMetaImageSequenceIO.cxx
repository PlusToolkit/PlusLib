#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif 

#include "itkMetaImageSequenceIO.h"
#include <iomanip>
#include <iostream>



namespace itk
{

	static const char* SEQMETA_FIELD_US_IMG_ORIENT = "UltrasoundImageOrientation"; 
	static const char* SEQMETA_FIELD_DEF_FRAME_TRANS = "DefaultFrameTransformName"; 
	static const char* SEQMETA_FIELD_CUSTOM_FIELD_NAMES = "CustomFieldNames"; 
	static const char* SEQMETA_FIELD_CUSTOM_FRAME_FIELD_NAMES = "CustomFrameFieldNames"; 

	//----------------------------------------------------------------------------
	MetaImageSequenceIO::MetaImageSequenceIO()
	{ 
		this->m_MetaImage = Superclass::GetMetaImagePointer(); 
		this->m_DefaultFrameTransformName = "ToolToTrackerTransform"; 
		this->m_UltrasoundImageOrientation = "XX"; 
	} 

	//----------------------------------------------------------------------------
	MetaImageSequenceIO::~MetaImageSequenceIO()
	{
		this->m_UserFieldMap.clear(); 
		this->m_CustomFrameFieldNamesForReading.clear(); 
		this->m_CustomFieldNamesForReading.clear(); 

		this->m_MetaImage->ClearFields();
		this->m_MetaImage->ClearUserFields(); 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::AddCustomFrameFieldNameForReading(const char* fieldName) 
	{ 
		// check whether we already added to the list or not
		if ( std::find(m_CustomFrameFieldNamesForReading.begin(), m_CustomFrameFieldNamesForReading.end(), fieldName) 
			== m_CustomFrameFieldNamesForReading.end() )
		{
				m_CustomFrameFieldNamesForReading.push_back(fieldName); 
		}
	} 

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::AddCustomFieldNameForReading(const char* fieldName) 
	{ 
		// check whether we already added to the list or not
		if ( std::find(m_CustomFieldNamesForReading.begin(), m_CustomFieldNamesForReading.end(), fieldName) 
			== m_CustomFieldNamesForReading.end() )
		{
				m_CustomFieldNamesForReading.push_back(fieldName); 
		}
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::SetUltrasoundImageOrientation( const char* strOrientation)
	{
		LOG_TRACE("MetaImageSequenceIO::SetUltrasoundImageOrientation"); 
		bool retValue(false); 

		if ( strOrientation == NULL || STRCASECMP(strOrientation, "XX") == 0 )
		{
      LOG_WARNING("Ultrasound image orientation is not defined in " << this->GetFileName()); 
			this->m_UltrasoundImageOrientation = "XX"; 
			retValue = true; 
		}
		else if ( STRCASECMP(strOrientation, "UF") == 0 )
		{
			this->m_UltrasoundImageOrientation = "UF"; 
			retValue = true; 
		}
		else if ( STRCASECMP(strOrientation, "UN") == 0 )
		{
			this->m_UltrasoundImageOrientation = "UN"; 
			retValue = true; 
		}
		else if ( STRCASECMP(strOrientation, "MF") == 0 )
		{
			this->m_UltrasoundImageOrientation = "MF"; 
			retValue = true; 
		}
		else if ( STRCASECMP(strOrientation, "MN") == 0 )
		{
			this->m_UltrasoundImageOrientation = "MN"; 
			retValue = true; 
		}
		else
		{
			LOG_ERROR("Unable to recognize ultrasound image orientation: " << strOrientation);
			this->m_UltrasoundImageOrientation = "XX"; 
			retValue = false; 
		}

		return retValue; 
	}
	
	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue)
	{
		std::ostringstream seqFieldName; 
		seqFieldName << "Seq_Frame" << std::setfill('0') << std::setw(4) << frameNumber << "_" << fieldName << std::ends; 
			
		// check whether we already added to the list or not
		UserFieldMap::iterator fieldIterator = this->m_UserFieldMap.find(seqFieldName.str()); 
		if ( fieldIterator == this->m_UserFieldMap.end() )
		{
			this->m_UserFieldMap[seqFieldName.str()] = fieldValue; 
			this->AddCustomFrameFieldNameForReading(fieldName); 

			return true; 
		}

		return false; 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::SetCustomString(const char* fieldName, const char* fieldValue)
	{
		// check whether we already added to the list or not
		UserFieldMap::iterator fieldIterator = this->m_UserFieldMap.find(fieldName); 
		if ( fieldIterator == this->m_UserFieldMap.end() )
		{
			this->m_UserFieldMap[fieldName] = fieldValue; 
			this->AddCustomFieldNameForReading(fieldName); 

			return true; 
		}

		return false; 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::PrintSelf(std::ostream& os, Indent indent) const
	{
		Superclass::PrintSelf(os, indent);
		
		os << indent << "Metadata User Fields:" << std::endl;
		for ( UserFieldMap::const_iterator it = this->m_UserFieldMap.begin(); it != this->m_UserFieldMap.end(); it++) 
		{
			os << indent << it->first << " = " << it->second << std::endl; 
		}
	}

	//----------------------------------------------------------------------------
	int MetaImageSequenceIO::ReadNumberOfFramesFromMetaData(const char *_fileName)
	{
		int dimSize[3] = {-1,-1,-1}; 

		MetaObject metaObj(3); 

		METAIO_STREAM::ifstream * tmpReadStream = new METAIO_STREAM::ifstream;
		tmpReadStream->open(_fileName, METAIO_STREAM::ios::binary | 
			METAIO_STREAM::ios::in);
		
		if(!tmpReadStream->rdbuf()->is_open())
		{
			delete tmpReadStream;
			return -1;
		}

		fflush(NULL);

		FieldsContainerType fields; 

		MET_FieldRecordType * mF;

		mF = new MET_FieldRecordType;
		MET_InitReadField(mF, "DimSize", MET_INT_ARRAY, true, -1, 3);
		mF->required = true;
		fields.push_back(mF);

		// TODO: after removing deprecated functions change it back to required true!
		mF = new MET_FieldRecordType;
		MET_InitReadField(mF, SEQMETA_FIELD_CUSTOM_FIELD_NAMES, MET_STRING, false);
		mF->required = false;
		fields.push_back(mF);

		// TODO: after removing deprecated functions change it back to required true!
		mF = new MET_FieldRecordType;
		MET_InitReadField(mF, SEQMETA_FIELD_CUSTOM_FRAME_FIELD_NAMES, MET_STRING, false);
		mF->required = false;
		fields.push_back(mF);

		if(!MET_Read(*tmpReadStream, & fields, '=', false, false) ) 
		{
			METAIO_STREAM::cerr << "MetaObject: Read: MET_Read Failed" 
				<< METAIO_STREAM::endl;
			return false;
		}

		mF = MET_GetFieldRecord("DimSize", &fields);
		if(mF && mF->defined)
		{
			int i;
			for(i=0; i<3; i++)
			{
				dimSize[i] = (int)mF->value[i];
			}
		}

		mF = MET_GetFieldRecord(SEQMETA_FIELD_CUSTOM_FIELD_NAMES, &fields);
		if(mF && mF->defined)
		{
			std::istringstream customFieldNames((char *)mF->value); 
			std::string item; 
			int i = 0; 
			while ( customFieldNames >> item )
			{
				this->AddCustomFieldNameForReading(item.c_str()); 
			}
		}

		mF = MET_GetFieldRecord(SEQMETA_FIELD_CUSTOM_FRAME_FIELD_NAMES, &fields);
		if(mF && mF->defined)
		{
			std::istringstream customFrameFieldNames((char *)mF->value); 
			std::string item; 
			int i = 0; 
			while ( customFrameFieldNames >> item )
			{
				this->AddCustomFrameFieldNameForReading(item.c_str()); 
			}
		}


		tmpReadStream->close();
		delete tmpReadStream;

		metaObj.Clear();
		metaObj.ClearUserFields();

		return dimSize[2]; 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::ReadImageInformation()
	{ 
		// First we need to read the number of frames 
		int numOfFrames = this->ReadNumberOfFramesFromMetaData(this->GetFileName()); 

		// Add the necessary custom frame fields that we should read
		UserFieldMap::iterator fieldIterator; 
		for ( int i = 0; i < numOfFrames; i++ ) 
		{
			for ( int fields = 0 ; fields < m_CustomFrameFieldNamesForReading.size(); fields++)
			{
				std::ostringstream seqFieldName; 
				seqFieldName << "Seq_Frame" << std::setfill('0') << std::setw(4) << i << "_" << m_CustomFrameFieldNamesForReading[fields] << std::ends; 
				
				// Avoid duplicate entries
				fieldIterator = this->m_UserFieldMap.find(seqFieldName.str()); 
				if ( fieldIterator == this->m_UserFieldMap.end() )
				{
					this->m_UserFieldMap[seqFieldName.str()].clear(); 
				}
			}
		}

		// Add the necessary custom fields that we should read
		for ( int fields = 0 ; fields < m_CustomFieldNamesForReading.size(); fields++)
		{
			std::ostringstream seqFieldName; 
			seqFieldName << m_CustomFieldNamesForReading[fields] << std::ends; 

			// Avoid duplicate entries
			fieldIterator = this->m_UserFieldMap.find(seqFieldName.str()); 
			if ( fieldIterator == this->m_UserFieldMap.end() )
			{
				this->m_UserFieldMap[seqFieldName.str()].clear(); 
			}
		}

		// Add UserFields to MetaObject
		this->SetupUserFields(IO_READ);

		// Read the informations now with UserFields from metadata
		Superclass::ReadImageInformation(); 

		// Add read userfields to map
		for ( UserFieldMap::const_iterator it = this->m_UserFieldMap.begin(); it != this->m_UserFieldMap.end(); it++) 
		{
			char* value = static_cast<char*>( m_MetaImage->GetUserField(it->first.c_str() ) ); 
			this->m_UserFieldMap[it->first] = value; 
			delete[] value; 
		}
		
		// Set the default frame transform name from metafile
		this->SetDefaultFrameTransformName( this->GetCustomString(SEQMETA_FIELD_DEF_FRAME_TRANS) ); 

		// Set the ultrasound image orientation from metafile
		this->SetUltrasoundImageOrientation( this->GetCustomString(SEQMETA_FIELD_US_IMG_ORIENT) ); 
		
		// Clear and add again the userfields for ImageData reading
		this->SetupUserFields(IO_READ);
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetupUserFields(SEQUENCE_IO_TYPE ioType)
	{
		// Clear and add userfields to list
		this->m_MetaImage->Clear();
		this->m_MetaImage->ClearUserFields(); 

		if ( ioType == IO_READ )
		{
			// TODO: after removing deprecated functions change it back to required true!
			this->m_MetaImage->AddUserField(SEQMETA_FIELD_CUSTOM_FIELD_NAMES , MET_STRING , 0, false); 
			this->m_MetaImage->AddUserField(SEQMETA_FIELD_CUSTOM_FRAME_FIELD_NAMES , MET_STRING , 0, false); 
		}

		for ( UserFieldMap::const_iterator it = this->m_UserFieldMap.begin(); it != this->m_UserFieldMap.end(); it++) 
		{
			switch (ioType)
			{
			case IO_READ:
				this->m_MetaImage->AddUserField(it->first.c_str(), MET_STRING , 0, false); 
				break;
			case IO_WRITE: 
				this->m_MetaImage->AddUserField(it->first.c_str(), MET_STRING, strlen(it->second.c_str()), it->second.c_str(), false); 
				break;
			}
		}
	}
	
	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::Write( const void* buffer) 
	{
		this->SetCustomString(SEQMETA_FIELD_DEF_FRAME_TRANS, this->GetDefaultFrameTransformName()); 
		this->SetCustomString(SEQMETA_FIELD_US_IMG_ORIENT, this->GetUltrasoundImageOrientation()); 

		// Save custom field names to header
		std::ostringstream customFieldNames; 
		for ( int fields = 0 ; fields < m_CustomFieldNamesForReading.size(); fields++)
		{
			customFieldNames << m_CustomFieldNamesForReading[fields] << " "; 
		}
		customFieldNames << std::ends; 
		this->SetCustomString(SEQMETA_FIELD_CUSTOM_FIELD_NAMES, customFieldNames.str().c_str()); 
		
		// Save custom frame field names to header
		std::ostringstream customFrameFieldNames; 
		for ( int fields = 0 ; fields < m_CustomFrameFieldNamesForReading.size(); fields++)
		{
			customFrameFieldNames << m_CustomFrameFieldNamesForReading[fields] << " "; 
		}
		customFrameFieldNames << std::ends; 
		this->SetCustomString(SEQMETA_FIELD_CUSTOM_FRAME_FIELD_NAMES, customFrameFieldNames.str().c_str()); 

		// Add UserFields to MetaObject
		this->SetupUserFields(IO_WRITE);

		// write fields to metadata
		Superclass::Write(buffer); 
	}

	//----------------------------------------------------------------------------
	const char* MetaImageSequenceIO::GetCustomFrameString( int frameNumber, const char* fieldName)
	{
		UserFieldMap::iterator fieldIterator; 

		std::ostringstream userField; 
		userField << "Seq_Frame" << std::setfill('0') << std::setw(4) << frameNumber << "_" << fieldName << std::ends;

		fieldIterator = this->m_UserFieldMap.find(userField.str()); 

		if ( fieldIterator != this->m_UserFieldMap.end() )
		{
			return fieldIterator->second.c_str();	
		}

		return NULL; 
	}

	//----------------------------------------------------------------------------
	const char* MetaImageSequenceIO::GetCustomString( const char* fieldName )
	{
		UserFieldMap::iterator fieldIterator; 
		std::ostringstream userField;
		userField << fieldName << std::ends; 
		
		fieldIterator = this->m_UserFieldMap.find(userField.str()); 

		if ( fieldIterator != this->m_UserFieldMap.end() )
		{
			return fieldIterator->second.c_str(); 
		}

		return NULL; 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::GetCustomFrameTransform( const int frameNumber, const char* frameTransformName, double* transformMatrix )
	{
		const char* customFrameString = this->GetCustomFrameString(frameNumber, frameTransformName); 

		if ( customFrameString == NULL )
		{
			return false; 
		}

		std::istringstream transformFieldValue(customFrameString); 

		double item; 
		int i = 0; 
		while ( transformFieldValue >> item )
		{
			transformMatrix[i++] = item; 
		}

		return true; 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::GetCustomFrameTransform( const int frameNumber, const char* frameTransformName, vtkMatrix4x4* transformMatrix )
	{
		double transform[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 }; 
		const bool retValue = this->GetCustomFrameTransform(frameNumber, frameTransformName, transform);
		transformMatrix->DeepCopy(transform);

		return retValue; 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::GetFrameTransform( const int frameNumber, double* transformMatrix )
	{
		return this->GetCustomFrameTransform(frameNumber, this->GetDefaultFrameTransformName(), transformMatrix); 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::GetFrameTransform( const int frameNumber, vtkMatrix4x4* transformMatrix )
	{
		 return this->GetCustomFrameTransform(frameNumber, this->GetDefaultFrameTransformName(), transformMatrix ); 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetCustomFrameTransform( const int frameNumber, const char* frameTransformName, double* transformMatrix )
	{
		std::ostringstream transform; 

		transform	<< transformMatrix[0]  << " " << transformMatrix[1]  << " " << transformMatrix[2]  << " " << transformMatrix[3]  << " " 
					<< transformMatrix[4]  << " " << transformMatrix[5]  << " " << transformMatrix[6]  << " " << transformMatrix[7]  << " " 
					<< transformMatrix[8]  << " " << transformMatrix[9]  << " " << transformMatrix[10] << " " << transformMatrix[11] << " " 
					<< transformMatrix[12] << " " << transformMatrix[13] << " " << transformMatrix[14] << " " << transformMatrix[15] << " "; 

		this->SetCustomFrameString(frameNumber, frameTransformName, transform.str().c_str()); 

	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetCustomFrameTransform( const int frameNumber, const char* frameTransformName, vtkMatrix4x4* transformMatrix )
	{
		double transform[16]; 
		for ( int i = 0; i < 4; i++ ) 
		{
			for ( int j = 0; j < 4; j++ ) 
			{
				transform[i*4+j] = transformMatrix->GetElement(i, j); 
			}
		}

		this->SetCustomFrameTransform(frameNumber, frameTransformName, transform); 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetFrameTransform( const int frameNumber, double* transformMatrix )
	{
		this->SetCustomFrameTransform(frameNumber, this->GetDefaultFrameTransformName(), transformMatrix); 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetFrameTransform( const int frameNumber, vtkMatrix4x4* transformMatrix )
	{
		this->SetCustomFrameTransform(frameNumber, this->GetDefaultFrameTransformName(), transformMatrix); 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::GetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix )
	{
		double transform[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		const bool retValue = this->GetCustomTransform(frameTransformName, transform); 
		transformMatrix->DeepCopy(transform); 

		return retValue; 
	}

	//----------------------------------------------------------------------------
	bool MetaImageSequenceIO::GetCustomTransform( const char* frameTransformName, double* transformMatrix )
	{
		const char* customString = this->GetCustomString(frameTransformName); 
		
		if ( customString == NULL )
		{
			return false; 
		}

		std::istringstream transformFieldValue(customString); 

		double item; 
		int i = 0; 
		while ( transformFieldValue >> item )
		{
			transformMatrix[i++] = item; 
		}

		return true; 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix )
	{
		double transform[16]; 
		for ( int i = 0; i < 4; i++ ) 
		{
			for ( int j = 0; j < 4; j++ ) 
			{
				transform[i*4+j] = transformMatrix->GetElement(i, j); 
			}
		}

		this->SetCustomTransform(frameTransformName, transform); 
	}

	//----------------------------------------------------------------------------
	void MetaImageSequenceIO::SetCustomTransform( const char* frameTransformName, double* transformMatrix )
	{
		std::ostringstream transform; 

		transform	<< transformMatrix[0]  << " " << transformMatrix[1]  << " " << transformMatrix[2]  << " " << transformMatrix[3]  << " " 
					<< transformMatrix[4]  << " " << transformMatrix[5]  << " " << transformMatrix[6]  << " " << transformMatrix[7]  << " " 
					<< transformMatrix[8]  << " " << transformMatrix[9]  << " " << transformMatrix[10] << " " << transformMatrix[11] << " " 
					<< transformMatrix[12] << " " << transformMatrix[13] << " " << transformMatrix[14] << " " << transformMatrix[15] << " "; 

		this->SetCustomString(frameTransformName, transform.str().c_str()); 
	}

} // end namespace itk
