#pragma  once

/**
 *  \file OemParamsList.h 
 *  \brief Contains lists of OEM parameters
 *  
 *   These are both declared and defined.
 *   If you want the parameters defined you need to define __DEFINE_VARS__ :
 *
 * \code
 *  #define  __DEFINE_VARS__
 *     #include "OemParamsList.h"
 *  #undef   __DEFINE_VARS__
 * \endcode
 */

/**
 \page PageReadOemParamList How to read lists of OEM parameters
 The oemArray class is polymorphic. It is therefore possible to 
 read any of the OEM parameters using objects of this class.
 
 The functions that directly operate with oemArray objects are
 found in the classes ParamSyncConnection and ParamSyncConnectionImp

  The lowest level of automation is to read a single Oem parameter using the following
  function:
  \code
  bool ParamSyncConnection::GetSingleOemParamVal(oemArray *oemArr, char view='\0');
  \endcode

  The first argument to the function is a pointer to an <b>allocated</b> oemArray object.
  The function generates a query string, sends it to the scanner and subsequently 
  fills the values of oemArray object.  The second argument concerns the query - does
  it have a view associated with the parameter or not. The list of parameters
  can be found from the OEM Interface product specification PS12640. If view == '\0'
  no view is added to the query.

  Here is an example of how to use GetSingleOemParamVal
  \code
	int GetOemParameters()
	{
	// Create a list with the parameters we want to extract
	static OemDefinition oemParamsTable [] = {
		{"IMAGE_MODE", oemINT,  true},
		{"TRANSDUCER", oemCHAR, true},
		{"TRANSDUCER_LIST", oemCHAR, false},
		{"B_SCANLINES_COUNT", oemINT, false},
		{NULL, oemUNKNOWN , false}
	};


	WSAIF wsaif;    // Microsoft windows sockets interface

	TcpClient oemClient(&wsaif, 2*1024*1024, 7915, "10.200.28.45");   // Client to OEM interfacce
	TcpClient tbxClient(&wsaif, 2*1024*1024, 5001, "10.200.28.45");   // Client to Toolbox
	ParamSyncConnection paramSync(&oemClient, &tbxClient);            // Connection for parameter synchronization


	bool connected = paramSync.ConnectOEMInterface();
	if (!connected) return -1;
	

	int numOemCommands = sizeof(oemParamsTable)/sizeof(OemDefinition) - 1;

	oemArray *var;
	static char datastring[2048];

	for (int n = 0; n < numOemCommands; n++)
	{
		var = new oemArray(oemParamsTable[n].name, // Name is needed for the query
						   oemParamsTable[n].type  // Type is not really needed
						   );

		if (oemParamsTable[n].needsView)
		{
			var->setView('A');
		}

		paramSync.GetSingleOemParamVal(var);
		cout << "var [" << var->oem_name << "] is of size (" << var->M << " x " << var->N << ")" << endl;

		memset( datastring, 0, sizeof(datastring));  
		var->fillDataString(datastring, sizeof(datastring));   // Textual description of values
		cout << datastring << endl;
	}

	return 0;
	}
	\endcode
    
	A higher-level way of reading lists of parameters is implemented in
	\code
	  bool ParamSyncConnectionImpl::GetOemVectorArrayFromList(OemParamDefStruct list[], bool getA, bool getB, OemArrayVector *vec);
	\endcode

	The first argument is a list of parameters, which is terminated by {NULL, oemUNKNOWN , false} 
	as in the example above. This is followed by two boolean inputs - whether to read view A, and view B.
	This will typically depend on ScannerMode (defined in OemParams.h)
	Finally there is a pointer to a OemArrayVector object. 
	The function will create and append members to OemArrayVector. The caller must release the memory.

	List of OEM parameters are declared and <b> defined </b> in OemParamsList.h.
	You can easily expand these or add new ones. 

	Here is an excerpt actual code that uses lists

	\code
	bool ParamSyncConnectionImp::GetAllOemParams(OemArrayVector *vec)
	{

		ScannerMode mode = this->GetScannerModeOem(); // Get which views and modes are active
		bool success;

        // There is always B-mode. So we can just get it.
		// mode will have the right bits set
		success = GetOemVectorArrayFromList(gOemBmodeParamsTable, // gOemBmodeParamsTable is defined in OemParamsList.h
		                                    mode.present_A, 
											mode.present_B, 
											vec                   // vec is an existing object
											);

		if (!success) return(success);
	

		if (mode.A.present_cfm || mode.B.present_cfm)
		{
			success = GetOemVectorArrayFromList(gOemCpParamsTable, mode.A.present_cfm, mode.B.present_cfm, vec);
			success = success & GetOemVectorArrayFromList(gOemCmodeParamsTable, mode.A.present_cfm, mode.B.present_cfm, vec);
		}
		...
	}
	\endcode
*/

/// <summary> A structure used to describe the queries to the OEM interface. 
/// The parameters are described in the OEM interface product specification PS12640
/// </summary>
typedef struct{
	char* name;       ///< String with the ID of the parameter. 
	bool withView;    ///< The parameter relates to a given view, or is general for the whole setup.
} OemParamDefStruct;






#ifndef __DEFINE_VARS__

extern OemParamDefStruct gOemBmodeParamsTable [];  ///< Table with parameters for B-mode
extern OemParamDefStruct gOemCmodeParamsTable [];  ///< Table with parameters for CFM
extern OemParamDefStruct gOemCpParamsTable [];     ///< Table with parameters for CFM and Power doppler
extern OemParamDefStruct gOemPmodeParamsTable [];  ///< Table with parameters for Power Doppler
extern OemParamDefStruct gOemPwDopParamsTable[];   ///< Table with parameters for Pulsed Wave Doppler
extern OemParamDefStruct gOemCwDopParamsTable [];  ///< Table with parameters for Continuous Wave Doppler
extern OemParamDefStruct gOemMmodeParamsTable [];  ///< Table with parameters for M-mode  
extern OemParamDefStruct gOemTgcParamsTable [];    ///< Table with parameters for TGC 
extern OemParamDefStruct gOemGeneralParamsTable[]; ///< Table with parameters outside other categories

#else

/// <summary>Table with parameters related to Bmode</summary>
OemParamDefStruct gOemBmodeParamsTable[] = {
	{"B_SPLIT", false},
	{"B_SIMULTANEOUS_SPLIT", false},
	{"B_GAIN", true},
	{"B_GAIN_DB", true},
	{"B_DYN_RANGE", true},
	{"B_MFI", true},
	{"B_MFI_REAL", true},
	{"B_GEOMETRY_TISSUE", true},
	{"B_FRAMERATE", true},
	{"B_HARMONIC_ACTIVATED", true },
	{"B_HARMONIC_MODE", true},
	{"B_BUBBLE_BURST", true},
	{"B_EXTENDED_RESOLUTION", true },
	{"B_MULTI_BEAM", true },
	{"B_SAMPLE_FREQUENCY", true },
	{"B_GEOMETRY_SCANAREA", true},
	{"B_RF_LINE_LENGTH", true},
	{"B_EXPANDED_SECTOR", true },
	{"B_COMPOUND", true},
	{"B_SCANLINES_COUNT", true},
	{NULL, false}
};

/// <summary> Table with parameters related to Cmode </summary>
OemParamDefStruct gOemCmodeParamsTable [] = 
{
	{"c_invert", true},
	{"c_prf", true},
	{"c_wall_filter", true},
	{"c_sub_mode", true},
	{"c_shots_per_estimate", true},
	{"c_base_line", true},
	{"c_multi_beam", true},
	{"c_mfi", true},
	{"c_mfi_real", true},
	{"c_gain", true},
	{NULL, false}
};

/// <summary> Table with parameters related to both C and P mode. </summary>
OemParamDefStruct gOemCpParamsTable [] = {
	{"cp_sample_frequency",  true },
	{"cp_geometry_scanarea", true },
	{"cp_scanlines_count",  true },
	{"cp_interleave_factor", true},
	{NULL, false}
};


/// <summary> Table with parameters related to Power Doppler (CFM without direction) </summary>
OemParamDefStruct gOemPmodeParamsTable [] = {
	{"p_multi_beam", true},
	{"p_mfi", true},
    {"p_mfi_real", true},
	{"p_gain", true},
	{"p_shots_per_estimate", true},
	{"p_prf", true},
	{"p_wal_filter", true},
	{"p_sub_mode", true},
	{"p_invert", true},
	{NULL, false}
};



/// <summary> Table with parameters related to pulsed-wave spectral doppler (PW Doppler).  </summary>
OemParamDefStruct gOemPwDopParamsTable [] = {
	{"d_gain", false},
	{"d_base_line", false},
	{"d_prf", false},
	{"d_wall_filter", false},
	{"d_invert", false},
	{"d_active_angle_correction", false},
	{"d_adjust_angle_correction", false},
	{"d_gate_position", false},
	{"d_sample_frequency", false},
	{"d_geometry_scanarea", false},
	{"d_update_mode", false},
	{"d_mfi_real",false},
	{NULL, false}
}; 



/// <summary> Table with parameters related to continuous-wave doppler </summary>
OemParamDefStruct gOemCwDopParamsTable [] = {
	{"cw_gain", false},
	{"cw_base_line", false},
	{"cw_scale", false},
	{"cw_wall_filter", false},
	{"cw_invert", false},
	{"cw_gate_position", false},
	{"cw_sample_frequency", false},
	{"cw_mfi_real", false},
	{"cw_geometry_scanarea", false},
	{NULL, false}
};



/// <summary> Table with parameters related to M-mode. </summary>
OemParamDefStruct gOemMmodeParamsTable [] = {
	{"m_gain", false},
	{"m_dyn_range", false},
	{"m_line_position", false},
	{NULL,false}
};



/// <summary> Parameters related to the TGC settings </summary>
OemParamDefStruct gOemTgcParamsTable [] = {
	{"tgc1", false},
	{"tgc2", false},
	{"tgc3", false},
	{"tgc4", false},
	{"tgc5", false},
	{"tgc6", false},
	{"tgc7", false},
	{"tgc8", false},
	{"tgc_curve_length", false},
	{"tgc_curve", true},
	{NULL, false}
};

/// <summary> Parameters that do not relate to a imaging mode </summary>
OemParamDefStruct gOemGeneralParamsTable[] = {
	{"transducer", true},
	{"scan_plane", true},
	{"scanner_model", false},
	{"steering_angle", true},  // Available for CFM, PWDop CWDop for linear arrays
	{"thermal_index_type", true},
	{"thermal_index_type", true},
	{"thermal_index_limit", true},
	{"thermal_index", true},
	{"mechanical_index_limit", true},
	{"mechanical_index", true},
	{"versions", false},
	{"date_time", false},
	{NULL, false}
};

#endif
