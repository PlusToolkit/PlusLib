#pragma once

#include "stdint.h"
#include <vector>


typedef double  UseCaseDouble;   ///< Custom float type. Cross platform compatibility
typedef float UseCaseFloat;      ///< 32-bit floating point. 
typedef int32_t UseCaseInt;      ///< Custom int type. Cross platform compatibility 

/// <summary> Structures will contain</summary>
typedef enum {USECASE_STOP = 0, USECASE_INT = 1, USECASE_DOUBLE = 2, USECASE_FLOAT = 3} UseCaseClassId;

/// <summary> Used to describe a structure from the use case </summary>
struct UseCaseStructDef
{
	UseCaseClassId type;   ///< Type of member field. Used to calculate offset
	char* key;             ///< UseCases are windows INI files. key is used to search for fields
};

/// <summary>
/// Dummy structure which is inherited by the actual UseCase structures.
/// This structure is used by the function UseCaseParser::ReadUseCaseStruct.
/// It gets a pointer to a "generic" use case structure. 
/// 
/// Such a structure is characterized by the following traits:
/// \li The data members are only of type \c int32_t or \c double
/// \li The alignment of data members is byte
/// 
/// UseCaseVoidStruct is used as an abstract structure. 
/// Concrete structures are both declared and defined in the header file
/// UseCaseStructs.h
/// </summary>
struct UseCaseVoidStruct
{
};



/// <summary> Vector consisting of 32-bit integers </summary>
typedef std::vector<int32_t> Int32Vector;

/// <summary> Private implementation of UseCaseParser. 
///  In this respect UseCaseParser servers as an interface only.
///  The actual implementation can be substituted by another implementation
///  UseCaseParserImp is declared and implemented in UseCaseParser.cpp
/// </summary>
class UseCaseParserImp;   

/// <summary> Parser of use cases.
/// Use case is a file saved by the BK software in a format like the 
/// Windows INI files. Every structure that is saved has a name which 
/// defines a Section. The data members of the structure define keys in 
/// that section. 
/// 
/// For example
/// \code 
/// struct ScanParams 
/// {
///    double C_Sound;
///    int32_t StartLineNum
///    ...
/// };
/// \endcode
/// will be saved in the UseCase file as
/// \code
/// [ScanParams]
/// C_Sound = 1540.0
/// StartLineNum = 1
/// ...
/// \endcode
/// 
/// In some cases there are arrays of structures present in the use case.
/// In C/C++ code they may, for example, be
/// \code
/// ScanParams scan[5];
/// scan[0].C_Sound = 1480; 
/// ...
/// \endcode
/// In the use case, the index is substituted by a dot and a number.
/// The 0th element of ScanParams will be saved as
/// 
/// \code
/// [ScanParams.0]
/// C_Sound = 1480
/// \endcode
/// 
/// The key functionality of the class is in the function 
/// UseCaseParser::ReadUseCaseStruct(char *name, int index, UseCaseVoidStruct* var, const UseCaseStructDef def[]);
/// The first argument is the name of the structure, which in the above
/// example is ScanParams. The second argument is the index, which in the above
/// example is 0. If there is no need for index, it must set to -1.
/// The third parameters is a descendant of UseCaseVoidStruct.
/// Finally the function needs description of structure. This is stored in 
/// an array of type UseCaseStructDef. The array contains basically
/// pairs of {TYPE}, {NAME}. The last element in the array must contain two 
/// ZEROS (0, NULL). 
/// Here is small example: 
/// \code
/// struct BFXmitParams : public UseCaseVoidStruct{
///     UseCaseInt MidLevelModeSel;
///     UseCaseInt BF_RcvSel;
/// } xmit;
///
///UseCaseStructDef gBFXmitParamsDef [] = {
///     {USECASE_INT, "MidLevelModeSel"},
///     {USECASE_INT, "BF_RcvSel"},
///     {USECASE_STOP, NULL}    // Must be last entry
/// } descr;
/// 
/// UseCaseParser parser("Dummy.dat");
/// 
/// parser.ReadUseCaseStruct("BFXmitParams", 0,&xmit, descr);
/// 
/// 
/// \endcode
/// 
/// </summary>

class UseCaseParser
{
public:

	/// <summary>Constructor. </summary>
	/// <param name="UseCaseName">	[in] Name of the use case to parse </param>
	UseCaseParser(char *UseCaseName);

	/// <summary>	Finaliser. </summary>
	virtual ~UseCaseParser();

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
	bool GetActiveMidlevelModes(Int32Vector *mlm);

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
	UseCaseParserImp *impl;   ///< Private implementation of the parser
};

