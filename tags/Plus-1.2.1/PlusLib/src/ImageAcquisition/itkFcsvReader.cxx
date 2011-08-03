//The following include to make Visual Studio 6 happy

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "itkFcsvReader.h"

#define MAXFLDS 30     /* maximum possible number of fields */
#define MAXFLDSIZE 150   /* longest possible field + 1 = 31 byte field */

namespace itk
{

FcsvReader::FcsvReader()
{
}

FcsvReader::~FcsvReader()
{
}


void FcsvReader::Update()
{   
	FILE *in=fopen(m_FileName.c_str(),"r");         //Open file on command line 

	if(in==NULL)
	{
		itkExceptionMacro("Failed to open file " << m_FileName );
	}
	
	char *delimiter=", =";
	char line[1024]={0x0};
	char data[MAXFLDS][MAXFLDSIZE]={0x0};  
	const unsigned int dim = FcsvNDimensions;			//Dimension of the work space

	m_FcsvDataObject.columns.resize(MAXFLDS);

	int RecordCnt = 0;
	while(fgets(line,sizeof(line)-1,in)!=0)		//Read a record 
	{
		char *p = strtok(line , delimiter);
    int fldcnt = 0;

		while(p)
		{
			strncpy( data[fldcnt] , p, MAXFLDSIZE-1);
			data[fldcnt][MAXFLDSIZE]='\0';
			fldcnt++;
			p=strtok('\0',delimiter);
		}        

		//------------------------------------- lines which start with "#" ----------------------------------
		if( !strcmp(data[0],"#") )	
		{
			if( (!strcmp(data[1],"Fiducial")) && (!strcmp(data[2],"List")) && (!strcmp(data[3],"file")) )
			{
				m_FcsvDataObject.filePath = data[ 4 ];
			}
			else if( !strcmp(data[1],"numPoints") )
			{
				m_FcsvDataObject.numPoints = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"version"))
			{
				m_FcsvDataObject.version = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"name"))
			{
				m_FcsvDataObject.name = data[ 2 ];
			}
			else if(!strcmp(data[1],"symbolScale"))
			{
				m_FcsvDataObject.symbolScale = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"symbolType"))
			{
				m_FcsvDataObject.symbolType = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"visibility"))
			{
				m_FcsvDataObject.visibility = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"textScale"))
			{
				m_FcsvDataObject.textScale = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"opacity"))
			{
				m_FcsvDataObject.opacity = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"ambient"))
			{
				m_FcsvDataObject.ambient = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"diffuse"))
			{
				m_FcsvDataObject.diffuse = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"specular"))
			{
				m_FcsvDataObject.specular = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"power"))
			{
				m_FcsvDataObject.power = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"locked"))
			{
				m_FcsvDataObject.locked = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"numberingScheme"))
			{
				m_FcsvDataObject.numberingScheme = atof(data[ 2 ]);
			}
			else if(!strcmp(data[1],"color"))
			{
				m_FcsvDataObject.color[0] = atof(data[ 2 ]);
				m_FcsvDataObject.color[1] = atof(data[ 3 ]);
				m_FcsvDataObject.color[2] = atof(data[ 4 ]);
			}
			else if(!strcmp(data[1],"selectedColor"))
			{
				m_FcsvDataObject.selectedColor[0] = atof(data[ 2 ]);
				m_FcsvDataObject.selectedColor[1] = atof(data[ 3 ]);
				m_FcsvDataObject.selectedColor[2] = atof(data[ 4 ]);
			}
			else if(!strcmp(data[1],"columns"))
			{				
				itkDebugMacro("Column header: ");
				for(int k=2; k<fldcnt ; k++)
				{
					m_FcsvDataObject.columns[k] = data[ k ];
					itkDebugMacro(<<m_FcsvDataObject.columns[k]<<" ");
				}
			}
			else 	continue;
		}

		//-------------------------------- lines which do NOT start with "#" --------------------------------
		else	
		{
			itk::FcsvPoint point;
			m_FcsvDataObject.points.push_back(point);
			m_FcsvDataObject.points[ RecordCnt ].label		=	   data[ 0 ];
			m_FcsvDataObject.points[ RecordCnt ].position[0] = atof(data[ 1 ]);
			m_FcsvDataObject.points[ RecordCnt ].position[1] = atof(data[ 2 ]);
			m_FcsvDataObject.points[ RecordCnt ].position[2] = atof(data[ 3 ]);
			m_FcsvDataObject.points[ RecordCnt ].selected	= atoi(data[ 4 ]);
			m_FcsvDataObject.points[ RecordCnt ].visibility	= atoi(data[ 5 ]);

			RecordCnt++;
		}
	}
	
	itkDebugMacro(<<"Fiducial data header: color=  "<<m_FcsvDataObject.color[ 0 ]<<","<<m_FcsvDataObject.color[ 1 ]<<","<<m_FcsvDataObject.color[ 2 ])
	itkDebugMacro(<<"Fiducial points:");
	for(int i=0; i<RecordCnt; i++)
	{
		itkDebugMacro(<<"\n"<<m_FcsvDataObject.points[ i ].label
			<<"  "<<m_FcsvDataObject.points[ i ].position[0]
			<<"  "<<m_FcsvDataObject.points[ i ].position[1]
			<<"  "<<m_FcsvDataObject.points[ i ].position[2]
			<<"  "<<m_FcsvDataObject.points[ i ].selected
			<<"  "<<m_FcsvDataObject.points[ i ].visibility)
	}

    fclose(in);

}

} // namespace itk
