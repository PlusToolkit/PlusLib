#include "stdafx.h"
#include "UseCaseParser.h"
#include <iostream>
#include <assert.h>


#define __DEFINE_VARS__
#include "UseCaseStructs.h"
#undef  __DEFINE_VARS__

/// <summary>	
///  Private implementation of UseCaseParser.
///  See the documentation of UseCaseParser 
/// </summary>
class UseCaseParserImp
{
public:
	UseCaseParserImp(char* UseCaseName);
	~UseCaseParserImp();

	/// <summary> Returns a vector with the numbers of the active mid level modes.
	///  The algorithm loads the macro routine, and finds which basic modes
	///  are used. The end of the search is at END_OF_MACRO. 
	///  Then the routine reads the basic modes, and from their selectors 
	///  determines which mid-level modes are active.
	/// </summary>
	/// <param name="mlm">	[out] If non-null, the mlm. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	/// <remarks> The active mid level modes are appended to the vector mlm. 
	/// 		  The caller is responsible to create an object of Int32Vector
	/// 		  and to destroy it after it has been used.
	/// </remarks>
	bool GetActiveMidLevelModes(Int32Vector * mlm);  

	/// <summary> Read a structure from an UseCase File.
	/// The result is stored in the output \c var. \c var is a structure
	/// that has been inherited from  UseCaseVoidStruct, is aligned at a byte, 
	/// and has only int32_t and double data members. 
	/// </summary>
	/// <param name="name"> 	[in] Name of the structure. </param>
	/// <param name="index">	Index for the structure. If this is not an array, then index  must be = -1 </param>
	/// <param name="var">  	[out] Pointer to a UseCase Structure </param>
	/// <param name="def">  	Array of definitions. For details, see the main page for the class. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	bool ReadUseCaseStruct(char *name, int index, UseCaseVoidStruct* var, const UseCaseStructDef def[]);

	/// <summary>	Writes a use case structure to Windows Ini File.  </summary>
	/// </summary>
	/// <param name="name"> 	[in] Name of the structure. </param>
	/// <param name="index">	Index for the structure. If this is not an array, then index  must be = -1 </param>
	/// <param name="var">  	[out] Pointer to a UseCase Structure </param>
	/// <param name="def">  	Array of definitions. For details, see the main page for the class. </param>
	/// <returns>	true if it succeeds, false if it fails. </returns>
	void WriteUseCaseStruct(char *name, int index, UseCaseVoidStruct* var, const UseCaseStructDef def[]);

	/// <summary> The opposite of ReadUseCaseStruct. Writes in a character array
	/// what one would expect to find in the use case for a given structure.
	/// Useful for debugging and visualization </summary>
	/// <param name="buf">   	[out] Pre-allocated character buffer </param>
	/// <param name="bufLen">	Length of the buffer. </param>
	/// <param name="var">   	The UseCase structure to be output. </param>
	/// <param name="def">   	Definition of the fields in the use-case structure </param>
	/// <returns>	. </returns>
	size_t PrintToCharArray(char * buf, size_t bufLen, const UseCaseVoidStruct& var, const UseCaseStructDef def[]);
private:
	char * useCaseName;  ///!< Name of the use case to parse

};




#ifdef _MSC_VER  
/// <summary> Microsoft uses sprintf_s for secure printing to a string. C99 standard specifies snprintf.
/// 		  Instead of using a fully-fledged implementation of snprintf, a short implementation is
/// 		  provided here, which makes it possible to use snprintf with GCC and MSVC  </summary>
/// <param name="buffer">	[out] A buffer where text is written to </param>
/// <param name="count"> 	Size of buffer including place for '\0'. </param>
/// <param name="fmt">   	Format string. </param>
/// <returns> Number of successfully written characters. Negative upon failure. </returns>
static int snprintf(char *buffer, size_t count, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = _vsnprintf_s(buffer,count, count-1, fmt, ap);
	if (ret < 0)
		buffer[count-1] = '\0';
	va_end(ap);
	return ret;
}
#endif



#ifdef _MSC_VER
/// <summary> Makes sure that in MSVC, strncpy_s is used. Needed to avoid warnings .</summary>
/// <param name="dst">  	Destination string. </param>
/// <param name="src">  	Source string. </param>
/// <param name="count">	Maximum number of characters to copy. <b> count must be less or equal to sizeof(dst) </b> </param>
#define strncpy(dst, src, count)   strncpy_s(dst, count, src, count)    /* Assumes that dst is indeed at least count long */
#endif



/* 
 *  ___________________________________________________________
 *
 *                   UseCaseParserImp
 *  ___________________________________________________________
 *
 */
UseCaseParserImp::UseCaseParserImp(char* UseCaseName)
{	
	this->useCaseName = new char[strlen(UseCaseName)+1];
	strcpy_s(this->useCaseName,strlen(UseCaseName)+1, UseCaseName);
}



UseCaseParserImp::~UseCaseParserImp()
{
	delete [] useCaseName;
}



bool UseCaseParserImp::GetActiveMidLevelModes(Int32Vector * mlm)
{
	char section[] = "MacroRoutine";
	char defaultValue[] = "end";
	char val[80];

	char key[10];
	char delim[] = ", ";   // Delimiters for parser	
	char *tok;

	unsigned int n = 0;
	bool done = false;

	Int32Vector activeBasicModes;

	while(!done)
	{
		snprintf(key,sizeof(key),"%d", n);
		GetPrivateProfileString(section, key, defaultValue, val, sizeof(val), this->useCaseName);

		if (!strncmp(val, defaultValue,strlen(defaultValue)))
		{
			done = true;
		}
		else
		{
			tok = strtok(val, delim);
			assert(tok != NULL);

			int cmd = atoi(tok);
			if (cmd == 10)   // End of main
			{	
				done = true;
			}
			else if (cmd == 4)
			{    // This is a fire command
				tok = strtok(NULL, delim);   // Next token is basic mode
				assert(tok != NULL);
				activeBasicModes.push_back(atoi(tok));
				// std::cout << "Found active basic " <<  atoi(tok) << std::endl;
			}
			++n;
		}
	}

	if (activeBasicModes.size() == 0)
	{
		return false;
	}

	// Now read all active basic modes

	BFXmitParams bfXmit;
	bool mlmAlreadyAdded = false;

	for (n = 0; n < activeBasicModes.size(); n++)
	{
		bool success = this->ReadUseCaseStruct("BFXmitParams", activeBasicModes[n], &bfXmit, gBFXmitParamsDef);
		if (success)
		{
			int32_t mlm_sel = (int32_t) bfXmit.MidLevelModeSel;
			// Do not repeat entries
			mlmAlreadyAdded = false;
			for (unsigned int k = 0; k < mlm->size(); ++k)
			{
				if (mlm->at(k) == mlm_sel)
				{
					mlmAlreadyAdded = true;
					break;
				}
			}
			if (! mlmAlreadyAdded)
			{
				mlm->push_back(mlm_sel);
			}
		}
	}

	return true;
}



bool UseCaseParserImp::ReadUseCaseStruct(char *name, int index, UseCaseVoidStruct* var, const UseCaseStructDef def[])
{
	int n = 0;
	
	char *ptr = (char*) var;
	
	UseCaseDouble *pDouble = (UseCaseDouble *) var;
	UseCaseInt *pInt = (UseCaseInt *) var;
	UseCaseFloat *pFloat = (UseCaseFloat *) var;

	char structName [256];
	memset(structName, 0, sizeof(structName));

	char fieldVal[128];


	if (index < 0)
	{
		sprintf_s(structName, sizeof(structName), "%s", name);
	}
	else
	{
		sprintf_s(structName, sizeof(structName), "%s.%d", name, index);
	}

	while( def[n].type!=USECASE_STOP )
	{	
		switch(def[n].type)
		{
		case USECASE_INT:
			pInt = (UseCaseInt *) ptr;
			*pInt = GetPrivateProfileInt(structName, def[n].key, 0, this->useCaseName);
			ptr += sizeof(UseCaseInt);

			break;

		case USECASE_DOUBLE:
			GetPrivateProfileString(structName, def[n].key, "0.0", fieldVal, sizeof(fieldVal), this->useCaseName);
			pDouble = (UseCaseDouble *) ptr;
			*pDouble = atof(fieldVal);
			ptr += sizeof(UseCaseDouble);
			break;

		case USECASE_FLOAT:
			GetPrivateProfileString(structName, def[n].key, "0.0", fieldVal, sizeof(fieldVal), this->useCaseName);
			pFloat = (UseCaseFloat *) ptr;
			*pFloat = static_cast<float>(atof(fieldVal));
			ptr += sizeof(UseCaseFloat);
			
		default:
			assert(false);
			break;
		}
		++n;
	}
	return true;

}


size_t UseCaseParserImp::PrintToCharArray(char * buf, size_t bufLen, const UseCaseVoidStruct& var, const UseCaseStructDef def[])
{
	char * ptr = (char*) &var;
	UseCaseInt *pInt = (UseCaseInt*) ptr;
	UseCaseDouble *pDouble = (UseCaseDouble *) ptr;
	UseCaseFloat *pFloat = (UseCaseFloat *) ptr;

	size_t remainBuf = bufLen;
	size_t numPrinted = 0;
	size_t lastPrinted = 0;

	int n = 0;
	
	while(remainBuf > 0 && def[n].type != USECASE_STOP)
	{
		switch(def[n].type)
		{
		case USECASE_INT:
			pInt = (UseCaseInt *) ptr;
			lastPrinted = snprintf(&buf[numPrinted], remainBuf, "%s = %d\n", def[n].key, *pInt);
			ptr += sizeof(UseCaseInt);
			remainBuf -= lastPrinted;

			break;

		case USECASE_DOUBLE:
			pDouble = (UseCaseDouble *) ptr;
			lastPrinted = snprintf(&buf[numPrinted], remainBuf, "%s = %d\n", def[n].key, *pDouble);
			ptr += sizeof(UseCaseDouble);
			remainBuf -= lastPrinted;

			break;
		case USECASE_FLOAT:
			pFloat = (UseCaseFloat *) ptr;
			lastPrinted = snprintf(&buf[numPrinted], remainBuf, "%s = %d\n", def[n].key, *pFloat);
			ptr += sizeof(UseCaseFloat);
			remainBuf -= lastPrinted;

			break;

		default:
			assert(false);
		}
		
		numPrinted += lastPrinted;
		++n;
	}

	return numPrinted;
}



/* 
 *  ___________________________________________________________
 *
 *                   UseCaseParser 
 *  ___________________________________________________________
 *
 */

UseCaseParser::UseCaseParser(char *UseCaseName): 
impl(new UseCaseParserImp(UseCaseName))
{
	
}


bool UseCaseParser::GetActiveMidlevelModes(Int32Vector *mlm)
{
	
	return impl->GetActiveMidLevelModes(mlm);
}

bool UseCaseParser::ReadUseCaseStruct(char *name, int index, UseCaseVoidStruct* var, const UseCaseStructDef def[])
{
	return this->impl->ReadUseCaseStruct(name, index, var, def);
}




size_t UseCaseParser::PrintToCharArray(char * buf, size_t bufLen, const UseCaseVoidStruct& var, const UseCaseStructDef def[])
{
	return this->impl->PrintToCharArray(buf, bufLen, var, def);
}

UseCaseParser::~UseCaseParser()
{
	delete impl;
}