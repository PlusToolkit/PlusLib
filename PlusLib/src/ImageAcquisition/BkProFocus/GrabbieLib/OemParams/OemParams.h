/*!\file OemParams.h
 * \brief Header file with declarations for oemArray and functions for reading and writing from and to files.
 * 
 * 
 */


#pragma once    /* Make sure that the file is included only once */

#include <stdio.h>
#include <vector>




// The query image_mode returns a number. To detect
// the active image mode, one must compare the number with a 
// mask. Below are the values of the mask
#define MASK_MODE_B      0x0001    ///< B-mode (returned by "query:image_mode;")  
#define MASK_MODE_C      0x0002    ///< CFM    (returned by "query:image_mode;") 
#define MASK_MODE_P      0x0004    ///< Power  (returned by "query:image_mode;") 
#define MASK_MODE_D      0x0008    ///< PW Doppler Mode (returned by "query:image_mode;") 
#define MASK_MODE_CW     0x0010    ///< CW-Mode (returned by "query:image_mode;") 
#define MASK_MODE_M      0x0020    ///< M-Mode  (returned by "query:image_mode;") 


/// <summary> Structure elaborating on what modes are active in the scanner</summary>
struct ScannerMode
{
	bool b_split;              ///< Split mode ?

	bool simultaneous_split;   ///< Simultaneous split ?

	bool present_A;     ///< Is there b image in View A ?
	bool present_B;     ///< Is there b image in View B ?

	struct {
		bool present_compound;      ///< Compound present ?
		bool present_cfm;           ///< CFM present ?
		bool present_power;         ///< Power Doppler
		bool present_pwd;           ///< Pulsed Wave Doppler
		bool present_cwd;           ///< Continu
		bool present_m;             ///< M-mode
	}A;  ///< View A

	struct {
		bool present_compound;      ///< Compound present ?
		bool present_cfm;           ///< CFM present ?
		bool present_power;         ///< Power Doppler
		bool present_pwd;           ///< Pulsed Wave Doppler
		bool present_cwd;           ///< Continu
		bool present_m;             ///< M-mode
	}B;  ///< View B

	/// <summary> Default constructor. Set all to false</summary>
	ScannerMode()
	{
		b_split = false;
		simultaneous_split = false;
		A.present_cfm = false;
		A.present_compound = false;
		A.present_cwd = false;
		A.present_m = false;
		A.present_power = false;
		A.present_pwd = false;

		B.present_cfm = false;
		B.present_compound = false;
		B.present_cwd = false;
		B.present_m = false;
		B.present_power = false;
		B.present_pwd = false;
	}
};


/*!
 * \brief
 * The type of oemArray data.
 */
typedef enum {oemUNKNOWN, oemDOUBLE, oemINT, oemCHAR, oemCELL} oemTypeID;



/*!
 * \brief
 * A class to handle OEM parameters.
 * 
 * This is a class to store OEM parameters. It can be configured
 * to be either a matrix or a vector. The elements of oemArray
 * can be either characters, integer numbers or floating-point 
 * double-precision numbers. Furthermore, an oemArray can be 
 * an array of oemArrays, thus allowing for heterogeneous data (structures)
 * to be stored.
 *
 *  The oemArray can read data from QUERYs or can format queries. 
 *  Functions are provided that can write oemArray objects to files and 
 *  read them back.
 *
 */
class oemArray{
public:
	oemTypeID type;     //!<  Data type - typeDOUBLE, typeINT, typeCHAR, typeCELL
	bool scalar;
	int M, N;          //!<   M - number of rows,  N - number of columns. Vectors are column vectors, i.e. N=1 
					
	char* oem_name;        //!< Parameter name on the scanner
	size_t oem_name_len;   //!< Length of the name 
	char  view;

	union{
		double *pDouble;  //!< Pointer to double. Not NULL only when type==typeDOUBLE  
		int *pInt;        //!< Pointer to integer 
		char *pChar;      //!< Pointer to characters 
		oemArray* pArray; //!< Pointer to other arrays 
		double vDouble;   //!< Value with double precision 
		int vInt;         //!< Integer value 
		char vChar;       //!< A single character 
	} data;               //!< The actual data
	
	/* Constructors */
	oemArray();
	oemArray(char* name, oemTypeID Type=oemINT);    /* Initialize the oem name at creation */
	oemArray(char val);
	oemArray(int val);
	oemArray(double val);
	
	void freeMem();        /* Free memory allocated for the matrix*/
	
	
	void setView(char v);       /* Control the view */
	char getView();

	bool reformat(oemTypeID ID, int m=1, int n=1, char* name=NULL);     /* Change the type and size of oemArray */
	bool setType(oemTypeID t);          
	bool toScalar();                               /* Convert to scalar. Data from matrix is lost */
	bool createIntMatrix(int m, int n);
	bool createDoubleMatrix(int m, int n);
	bool createCharMatrix(int m, int n);
	bool createCellMatrix(int m, int n);
	
	char* setOemName(char* name);

	double getScalar();
	
	int getInt();       /* m is the index in the array */
	double getDouble();   /* m is the index in the array */
	char getChar();       /* m is the index in the array */

	int setInt(int val);           /*val is the value in the array */
	double setDouble(double val);    /*m is the index in the array   */
	char setChar(char val);


	int getIntAt(int m, int n=0);       /* m is the index in the array */
	double getDoubleAt(int m, int n=0);   /* m is the index in the array */
	char getCharAt(int m, int n=0);       /* m is the index in the array */
	oemArray* getOemArrayAt(int m, int n=0);    /* Returns a pointer to an array in the cell structure */


	int setIntAt(int val, int m, int n=0);           /*val is the value in the array */
	double setDoubleAt(double val, int m, int n=0);    /*m is the index in the array   */
	char setCharAt(char val, int m, int n=0);
	oemArray *setOemArrayAt(oemArray* pval, int m, int n=0);  /* The actual value pointed to by pval will be copied! */


    char* setSzStringAt(char* src, int m=0);
	char* getSzStringAt(int m=0);
	char* getSzString();



	int fillQueryString(char* dest, size_t len, char* arg=NULL, bool with_view=true);          /* Fill in a query string  */
	int fillCommandString(char* dest, size_t len, bool with_view=true);       /* Return the number of characters in the string */


	char* setFromDataString(char* data_str, size_t len);        /* Fill in the values from a Data string */
	int fillDataString(char* dest, size_t len, bool bin_format=false);  /*Returns the number of bytes in the string */

	operator int() { return getInt();};
	operator double() {return getDouble();};
	operator char() {return getChar();};
	~oemArray();
}; 



int append_oem_array_to_file(FILE* outfile, oemArray* var);
int read_oem_array_from_file(oemArray *var, FILE* infile);
int fill_oem_array_from_data_str(oemArray *var, char* data_str, size_t data_str_len);

bool cmp_oem_array_eq(const oemArray& a, const oemArray& b);




/// <summary>Used to dynamically allocate oemArrays depending the mode </summary>
typedef std::vector<oemArray*> OemArrayVector;




/**
  \mainpage Short tutorial with OemArrays

  oemArray is strongly inspired by the Matlab mxArray. The idea was
  to implement a polymorphic data type without inheritance.
  This has resulted in a semi-C++ code. The class has the 
  capability to 
  \li Parse data strings from the OEM interface
  \li Type cast
  \li Read/write to a file without loss of precision.

  There is a complementary PDF document with an additional tutorial.
  The user is also encouraged to inspect the implementation 
  of ParametersLibrary. Here is an example of how to initialize 
  an array of oemArray

  \code
int TestCreattionOfOemParameters()
{

	static char* data_str[] = {
		"DATA:LANGUAGE ENGLISH;",
		"DATA:IMAGE_MODE:A 1;",
		"DATA:TRANSDUCER:A \"C\",\"8811\";",
		"DATA:TRANSDUCER_LIST \"8662\",\"C\",\"\",\"\",\"8811\",\"L\",\"\",\"\";",
		"DATA:B_SCANLINES_COUNT:A 269;",
		"DATA:B_GEOMETRY_SCANAREA:A 0.023075,-1.16415e-10,1.5708,-5.411669e-006,-0.023075,-1.16415e-010,1.5708,0.0399383;",
		"DATA:B_SPLIT:A \"A\";",
		"DATA:B_SIMULTANEOUS_SPLIT: \"OFF\";",
	};

	int numVars = sizeof(data_str) / sizeof(char*);
	oemArray* var = new oemArray [numVars];


	
	for (int n = 0; n < numVars; ++n)
	{
		static char localdatastring[256];   // We need a copy of the data string, because the function 
		                                    //fill_oem_array_from_data_str writes to the data string
		static char outdatastring[256];

		strncpy_s(localdatastring, sizeof(localdatastring), data_str[n], strlen(data_str[n]) );
		fill_oem_array_from_data_str(&var[n], localdatastring, strlen(localdatastring));
		var[n].fillDataString(outdatastring, sizeof(outdatastring));

		cout << " In : " << data_str[n] << endl;
		cout << "Out : " << outdatastring << endl;
	}

	delete [] var;
	return 0;
}

  \endcode

 */
