#pragma once

/**
	\page PageReadUseCases How to read structures from use cases
  
	Here is an example of how to read structures from the use case
	\code
	int TestUseCaseParser()
	{
		char useCaseName[256] ;

		cout << "Input a name of an use case : ";
		cin >> useCaseName;

		UseCaseParser parser(useCaseName);
		ScanParams scan;
		parser.ReadUseCaseStruct("ScanParams", 9, &scan, gScanParamsDef);

		static char buffer [2048];
		parser.PrintToCharArray(buffer, sizeof(buffer), scan, gScanParamsDef);

		cout << buffer << endl;

		return 0;
	}
	\endcode
	
	A bit of explanation.
	The function UseCaseParser::ReadUseCaseStruct can read an arbitrary 
	structure from an use case. 
	The structure has two types allowed: 
	\li UseCaseDouble
	\li UseCaseInt
	\li Data members are byte aligned.
	\li The structure inherits UseCaseVoidStruct

	Example for such a structure is:
	\code
	struct BFXmitParams : public UseCaseVoidStruct{
		UseCaseInt MidLevelModeSel;
		UseCaseInt BF_RcvSel;
	};
	\endcode
	
	Next the function needs a description of the structure.
	This description is provided as an array of type UseCaseStructDef
	For the abobe example, the description is
	\code
	// Create an array with descriptions
	UseCaseStructDef gBFXmitParamsDef [] = {
		{USECASE_INT, "MidLevelModeSel"},
		{USECASE_INT, "BF_RcvSel"},
		{USECASE_STOP, NULL}    // Must be last entry
	};
	\endcode
	
	Finally to read the structure one needs to create a parser and 
	to call it with the right entries:

	\code
	
	BFXmitParams xmit;

	UseCaseParser parser(useCaseName);
	parser.ReadUseCaseStruct("BFXmitParams", 0, &xmit, gBFXmitParamsDef);
	\endcode
 */


#if (defined(_MSC_VER) || defined(__GNUC__))
#pragma pack(push , 1)                           // We are reading and saving binary images. Need to ensure 32-bit/64-bit compatibility.
#endif



/// <summary>Selected values from ScanParams structure </summary>
/// <remarks> Remember always to update gScanParamsDef []</remarks>
struct ScanParams : public UseCaseVoidStruct{
	UseCaseInt ScanType;
	UseCaseInt CompoundNumb;
	UseCaseInt LineLengthRF;
	UseCaseInt StartLineNumQ;
	UseCaseInt StopLineNumQ;
	UseCaseInt CompoundTypeReal;

	UseCaseInt DOP_Sel;
	UseCaseInt CFM_Sel;
	UseCaseInt AcModSel;
};


/// <summary>	Bf transmit parameters.  </summary>
/// <remarks>  Remember alway to update gBFXmitParamsDef <remarks>
struct BFXmitParams : public UseCaseVoidStruct{
	UseCaseInt MidLevelModeSel;
	UseCaseInt BF_RcvSel;
};

/// <summary> Needed to calculate different buffer sizes </summary>
/// <remarks> Remember always to upadte the gMacroShotParamsDef [] </remarks>
struct MacroShotParams: public UseCaseVoidStruct {
	UseCaseInt TMT_PRT;
	UseCaseInt DOP_PRT;
	UseCaseInt SHOT_T;
	UseCaseInt SKIP_T;
	UseCaseInt ShotsPerEstimate;
	UseCaseInt C_InterleaveFactor;
	UseCaseInt BlocksPerMC_Line;
};


struct DopParams:public UseCaseVoidStruct{
	UseCaseInt FFT_Size;
	UseCaseInt AlternatingDoppler;
};


struct CfmParams: public UseCaseVoidStruct{
	UseCaseInt CFM_Mode;  
};


#ifndef __DEFINE_VARS__
extern UseCaseStructDef gScanParamsDef [];
extern UseCaseStructDef gBFXmitParamsDef[];
extern UseCaseStructDef gMacroShotParamsDef[];
extern UseCaseStructDef gDopParamsDef[];
extern UseCaseStructDef gCfmParamsDef[];

#else




UseCaseStructDef gScanParamsDef [] = {
	{USECASE_INT, "ScanType"},
	{USECASE_INT, "CompoundNumb"},
	{USECASE_INT, "LineLengthRF"},
	{USECASE_INT, "StartLineNumQ"},
	{USECASE_INT, "StopLineNumQ"},
	{USECASE_INT, "CompoundTypeReal"},
	{USECASE_INT, "DOP_Sel"},
	{USECASE_INT, "CFM_Sel"},
	{USECASE_INT, "AcModSel"},
	{USECASE_STOP, NULL},             // Must be last entry
	
}; 


UseCaseStructDef gBFXmitParamsDef [] = {
	{USECASE_INT, "MidLevelModeSel"},
	{USECASE_INT, "BF_RcvSel"},
	{USECASE_STOP, NULL}    // Must be last entry
};

UseCaseStructDef gMacroShotParamsDef[] = {
	{USECASE_INT, "TMT_PRT"},
	{USECASE_INT, "DOP_PRT"},
	{USECASE_INT, "SHOT_T"},
	{USECASE_INT, "SKIP_T"},
	{USECASE_INT, "ShotsPerEstimate"},
	{USECASE_INT, "C_InterleaveFactor"},
	{USECASE_INT, "BlocksPerMC_Line"},
	{USECASE_STOP, NULL}    // Must be last entry
};


UseCaseStructDef gDopParamsDef[] = {
	{USECASE_INT, "FFT_Size"},
	{USECASE_INT, "AlternatingDoppler"},
	{USECASE_STOP, NULL}    // Must be last entry
};

UseCaseStructDef gCfmParamsDef[] = {
	{USECASE_INT, "CFM_Mode"},
	{USECASE_STOP, NULL},
};

#endif



#if (defined(_MSC_VER) || defined(__GNUC__))
#pragma pack(pop)                           // We are reading and saving binary images. Need to ensure 32-bit/64-bit compatibility.
#endif
