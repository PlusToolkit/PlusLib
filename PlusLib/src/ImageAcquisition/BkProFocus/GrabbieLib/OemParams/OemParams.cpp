/*!
 * <File comment goes here!!>
 * 
 * Copyright (c) 2005 by <your name/ organization here>
 */


#include "OemParams.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <assert.h>
#include <iostream>

/*!
* \brief
* If no parameters are given, then the oemArray object
* is of unknown type, all pointers are set to NULL,
* and the object is set to be a scalar with a value of 0.
* The view on the scanner, associated with the object is 'A'
*/
oemArray::oemArray()
{

	type=oemUNKNOWN;
	data.pDouble = NULL;
	data.vDouble = 0.0;    /* Double is longest of all */
	M = 1;
	N = 1;
	scalar = true;
	oem_name = NULL;
	oem_name_len = 0;
	view = 'A';
}



/*!
* \brief
* Create a scalar character value.
*/
oemArray::oemArray(char chr)
{
	type = oemCHAR;
	scalar = true;
	M = N = 1;
	data.vChar = chr;
	oem_name = NULL;
	oem_name_len = 0;
	view = 'A';
}



/*!
* \brief
* Create a scalar double-precision value
*/
oemArray::oemArray(double val)
{
	type = oemDOUBLE;
	scalar = true;
	M=N=1;
	data.vDouble = val;
	oem_name = NULL;
	oem_name_len = 0;
	view = 'A';
}



/*!
* \brief
* Create a scal integer value
*/
oemArray::oemArray(int val)
{
	type = oemINT;
	scalar = true;
	M=N=1;
	data.vInt = val;
	oem_name = NULL;
	oem_name_len = 0;
	view = 'A';
}



/*!
* \brief
* Create a scalar value of a given Type, which corresponds to
* a OEM query name name
* 
* \param name
*      OEM name such as CP_SCANLINES_COUNT
*
* \param Type 
*     Type of the data - oemCHAR, oemINT, oemDOUBLE or oemCELL
* 
*/
oemArray::oemArray(char* name, oemTypeID Type)
{
	type = Type;
	scalar = true;
	M=N=1;
	oem_name = new char[strlen(name)+1];
	oem_name_len = strlen(name);
	strcpy_s(oem_name, oem_name_len+1,  name);

	view = 'A';
}



/*!
* \brief
*  Release the allocated memory.
*/
void oemArray::freeMem()
{
	if (scalar == true) return;

	switch (type){
			case oemDOUBLE:
				delete [] data.pDouble;
				break;

			case oemINT:
				delete [] data.pInt;
				break;

			case oemCHAR:
				delete [] data.pChar;
				break;

			case oemCELL:
				delete [] data.pArray;
				break;

			default:
				break;
	}
}




/* Release memory and change to scalar */
/*!
* \brief
*  Release memory and convert to scalar, if the 
*  data is allocated as a matrix and the dimensions are 1x1
* 
* \returns
*   true upon success
* 
*/
bool oemArray::toScalar()
{
	if (scalar == true) return true;
	freeMem();
	M = N = 1;
	scalar = true;
	return true;
}




/*!
* \brief
*  Change the type, the dimensions and the name of oemArray object.
* 
* \param ID
*    The new type for the oemArray
*
* \param m, n
*    New dimensions
* 
* \param name
*    New OEM name
* 
* \returns
*   true upon success
* 
*/
bool oemArray::reformat(oemTypeID ID, int m, int n, char* name)
{
	bool result = false;
	if (m*n>1){
		switch(ID){
			case oemDOUBLE:
				result = createDoubleMatrix(m,n);
				break;

			case oemINT: 
				result = createIntMatrix(m,n);
				break;

			case oemCHAR:
				result = createCharMatrix(m,n);
				break;

			case oemCELL:
				result = createCellMatrix(m,n);
				break;
		}
	}else{
		result = toScalar();
		setType(ID);
	}
	if (name!=NULL) setOemName(name);
	return result;
}



/*!
* \brief
* Release allocated memory
*/
oemArray::~oemArray()
{
	if (scalar == false){
		freeMem();
	}
	if (oem_name!=NULL){
		delete [] oem_name;
	}
}



/*!
* \brief
*  Return the integer value of a scalar.
* 
*/
int oemArray::getInt()
{
	int val = this->data.vInt;
	
	if (this->scalar == true)
	{
		switch(this->type)
		{
		case oemINT:
			val = this->data.vInt;
			break;

		case oemDOUBLE:
			val = static_cast<int>(this->data.vDouble);
			break;

		case oemCHAR:
			val = static_cast<int>(this->data.vChar);
			break;

		}
	}
	else
	{
		switch(this->type)
		{
		case oemINT:
			val = this->data.pInt[0];
			break;
		case oemDOUBLE:
			val = static_cast<int>(this->data.pDouble[0]);
			break;
		case oemCHAR:
			val = static_cast<int>(this->data.pChar[0]);
			break;

		}
	}

	return val;
}






/*!
* \brief
* Return the double-precision floating point value of a scalar.
*/
double oemArray::getDouble()
{
	double val = this->data.vDouble;

	if (this->scalar == true)
	{
		switch(this->type)
		{
		case oemINT:
			val = static_cast<double>(this->data.vInt);
			break;

		case oemDOUBLE:
			val = this->data.vDouble;
			break;

		case oemCHAR:
			val = static_cast<double>(this->data.vChar);
			break;
		}
	}
	else
	{
		switch(this->type)
		{
		case oemINT:
			val = static_cast<double>(this->data.pInt[0]);
			break;
		case oemDOUBLE:
			val = this->data.pDouble[0];
			break;
		case oemCHAR:
			val = this->data.pChar[0];
			break;
		}
	}
	return val;
}



/*!
* \brief
* Return the value of a scalar which is a single character
* 
*/
char oemArray::getChar()
{
	char val = data.vChar;

	if (this->scalar == true)
	{
		switch (this->type)
		{
		case oemINT:
			val = static_cast<char>(this->data.vInt);
			break;

		case oemDOUBLE:
			val = static_cast<char>(this->data.vDouble);
			break;
		}
	}
	else
	{
		switch(this->type)
		{
		case oemINT:
			val = static_cast<char>(this->data.pInt[0]);
			break;
		case oemDOUBLE:
			val = static_cast<char>(this->data.pDouble[0]);
			break;
		case oemCHAR:
			val = this->data.pChar[0];
			break;
		}
	}
	return val;		
}



/*!
* \brief
* Set the integer value of an integer scalar oemArray object.
*/
int oemArray::setInt(int val)
{
	assert(this->type == oemINT);
	return (data.vInt = val);
}


/*!
 * \brief
 * Set the double value of a scalar oemArray
 */
double oemArray::setDouble(double val)
{
	assert(this->type == oemDOUBLE);
	return data.vDouble = val;
}



/*!
* \brief
* Write brief comment for scanFillHeader here.
* Set the value of a character scalar
*/
char oemArray::setChar(char val)
{
	assert(this->type == oemCHAR);
	return data.vChar = val;
}



/*!
* \brief
* Get an element from an integer matrix.
* 
* \param m
*  Row
* 
* \param n
*   Column
*
* \returns
* The value stored at (m,n). 
* 
*/
int oemArray::getIntAt(int m, int n)
{
	if (scalar == true){
		return data.vInt; 
	}else if(m >=0 && n>=0 && (m*n)<(M*N)){
		return data.pInt[m + n*N];
	}
	return 0;
}



/*!
* \brief
* Get an element from an double matrix.
* 
* \param m
*  Row
* 
* \param n
*   Column
*
* \returns
* The value stored at (m,n). 
* 
*/
double oemArray::getDoubleAt(int m, int n)
{
	if (scalar == true){
		return data.vDouble;
	}else if(m>=0 && n>=0 && (m*n)<(M*N)){
		return data.pDouble[m+n*N];
	}
	return 0;
}


/*!
* \brief
* Get an element from an character matrix.
* 
* \param m
*  Row
* 
* \param n
*   Column
*
* \returns
* The value stored at (m,n). 
* 
*/
char oemArray::getCharAt(int m, int n)
{
	if (scalar == true){
		return data.vChar;
	}else if(m>=0 && n>=0 && (m*n)<(M*N)){
		return data.pChar[n+m*N];
	}
	return 0;
}




/*!
* \brief
* Set the value of a matrix
* 
* \param val
*   The new value
* 
* \param m
*    The row number
*
* \param n
*    The column number
* 
*/
int oemArray::setIntAt(int val, int m, int n)
{
	if (scalar == true){
		return data.vInt = val;
	}else if(m>=0 && n>=0 && (m*n)<(M*N)){
		return data.pInt[m+n*N] = val;
	}
	return 0;
}



/*!
* \brief
* Set the value of a matrix
* 
* \param val
*   The new value
* 
* \param m
*    The row number
*
* \param n
*    The column number
* 
*/
double oemArray::setDoubleAt(double val, int m, int n)
{
	if (scalar == true){
		return data.vDouble = val;
	}else if (m>=0 && n>=0 && (m*n)<(M*N)){
		return data.pDouble[m+n*N] = val;
	}
	return 0;
}


/*!
* \brief
* Set the value of a matrix
* 
* \param val
*   The new value
* 
* \param m
*    The row number
*
* \param n
*    The column number
* 
*/
char oemArray::setCharAt(char val, int m, int n)
{
	if (scalar == true){
		return data.vChar = val;
	}else if(m>=0 && n>= 0 && (m*n)<(M*N)){
		return data.pChar[n+m*N] = val;
	}
	return 0;
}



/*!
* \brief
* Change the OEM name
* 
* \param name
*  New name
* 
* \returns
*  Pointer to the name
*/
char* oemArray::setOemName(char* name)
{
	oem_name = new char[(oem_name_len=strlen(name))+1];
	memset(oem_name,0, oem_name_len+1);
	strcpy_s(oem_name,oem_name_len+1,  name);
	return oem_name;
}




/*!
* \brief
* Allocate space for a matrix and set the type.
* 
* \param m
*    Number of rows
*
* \param n
*     Number of columns
* 
* \returns
*     True if successful
* 
*/
bool oemArray::createIntMatrix(int m, int n)
{
	if (m<1 || n<1)
		return false;

	if (scalar == false)
		freeMem();



	if ((data.pInt = new int [m*n])==NULL)
		return false;
	
	memset(data.pDouble, 0, sizeof(int)*m*n);


	scalar = false;
	M = m;
	N=n;
	type = oemINT;
	view = 'A';

	return true;
}



/*!
* \brief
* Allocate space for a matrix and set the type.
* 
* \param m
*    Number of rows
*
* \param n
*     Number of columns
* 
* \returns
*     True if successful
* 
*/
bool oemArray::createDoubleMatrix(int m, int n)
{
	if (m<1 || n<1)
		return false;

	if (scalar == false)
		freeMem();



	if ((data.pDouble = new double [m*n])==NULL)
		return false;

	memset(data.pDouble, 0, sizeof(double)*m*n);

	scalar = false;
	M = m;
	N=n;
	type = oemDOUBLE;
	view = 'A';

	return true;
}




/*!
* \brief
* Allocate space for a matrix and set the type.
* 
* \param m
*    Number of rows
*
* \param n
*     Number of columns
* 
* \returns
*     True if successful
* 
*/
bool oemArray::createCharMatrix(int m, int n)
{
	if (m<1 || n<1)
		return false;

	if (scalar == false)
		freeMem();



	if ((data.pChar = new char [m*n])==NULL)
		return false;

	memset(data.pChar, 0, sizeof(char)*m*n);

	scalar = false;
	M = m;
	N=n;
	type = oemCHAR;
	view = 'A';

	return true;
}



/*!
* \brief
* Allocate space for a matrix and set the type.
* 
* \param m
*    Number of rows
*
* \param n
*     Number of columns
* 
* \returns
*     True if successful
* 
*/
bool oemArray::createCellMatrix(int m, int n)
{
	if (m<1 || n<1)
		return false;

	if (scalar == false)
		freeMem();

	if ((data.pArray = new oemArray[m*n])==NULL)
		return false;
	
	scalar = false;
	M = m;
	N=n;
	type = oemCELL;
	view = 'A';
	return true;
}


/*!
* \brief
* Change the type of a oemArray
* 
*/
bool oemArray::setType(oemTypeID t)
{
	bool retval;
	retval = (type==oemUNKNOWN);  // Always success when the old type is unknown
	type = t;                   // We will not argue
	return true;
}



/*!
 * \brief
 * Change the view associated with oemArray
 */
void oemArray::setView(char v)
{
	view = v;
}




/*!
* \brief
* Get the view associated with the oemArray
* 
*/
char oemArray::getView()
{
	return view;

}





/*!
* \brief
* Get the value of oemArray at a given position.
* The value can be another oemArray (CELL matrix in Matlab)
* 
* \param m
*   Row
*
* \param n
*   Column
*
* \remarks
*  There is no type-check in the function
*/
oemArray* oemArray::getOemArrayAt(int m, int n)
{
	if (scalar == true){
		return NULL;
	}else if(m>=0 && n>=0 && (m*n)<(M*N)){
		return &data.pArray[m + n*N];
	}
	return 0;
}



/*!
 * \brief
 * Set the value of an element of a oemArray to point  to another oemArray.
 *
 * \param pval
 *   Pointer to object of type oemArray
 *
 * \param m,n
 *    Row and column in oemArray
 *
 */
oemArray* oemArray::setOemArrayAt(oemArray *pval, int m, int n)
{
	if (scalar == true){
		return NULL;
	}else if (m>=0 && n>=0 && (m*n)<(M*N)){
		memcpy(&data.pArray[n+m*M], pval, sizeof(oemArray));
		return &data.pArray[n+m*M];
	}
	return 0;
}



/*
  dest - memory buffer to fill in the query string
  len - length of the query string
 */
/*!
* \brief
*  Generate a query string from the information contained
* in oemArray object
* 
* \param dest
*    Text buffer
*
*  \param len
*    Length of the text buffer
*
*  \param arg
*     Pointer to text with arguments. Some queries require
*     arguments, and they are not stored in a oemArray object
* 
*  \param with_view
*    if with_view!=0, the view is added to the query string.
* 
* \returns
*   The number of characters in the query string.
*
*/
int oemArray::fillQueryString(char* dest, size_t len, char* arg, bool with_view)
{
	int no_chars;
	bool string_arg = false;
	

	if (arg!= NULL)
		if (isalpha(arg[0])) string_arg = true;

	if (view!='A' && view!='B') with_view = false;
		
	memset(dest,0,len);
    
	if (len < 10+oem_name_len) return NULL;
	if (with_view == true){
		if (arg==NULL){
			no_chars = sprintf_s(dest,len,"QUERY:%s:%c;", oem_name, view);	
		}else if (string_arg == false){
			no_chars = sprintf_s(dest,len,"QUERY:%s:%c %s;", oem_name, view, arg);	
		}else{
			no_chars = sprintf_s(dest,len,"QUERY:%s:%c \"%s\";", oem_name, view, arg);	
		}
	}else{
		if (arg==NULL){
			no_chars = sprintf_s(dest,len,"QUERY:%s;", oem_name, view);	
		}else if (string_arg == false){
			no_chars = sprintf_s(dest,len,"QUERY:%s %s;", oem_name, view, arg);	
		}else{
			no_chars = sprintf_s(dest,len,"QUERY:%s \"%s\";", oem_name, view, arg);	
		}	
	}

	return no_chars;
}




/* Set a zero-terminated string in a given row of the matrix.
   Remember that strings are internally stored opposite to 
   numerical arrays - that is their position are transposed, and
   the data is continuous along columns. 

   The indexing is similar to that of ordinary arrays.

 */


/*!
* \brief
* Set a zero-terminated string in a given row of the matrix.
*
* \param src
*   The new string to be set in the oemArray object
*
* \param m
*    Row where to store the value of the string.
* 
* \returns
*  A pointer to the internally stored string.
* 
* 
* Remember that strings are internally stored opposite to 
* numerical arrays - that is their position are transposed, and
* the data is continuous along columns. 
*
* 
* 
* \remarks
* The indexing is similar to that of ordinary arrays.
* 

*/
char* oemArray::setSzStringAt(char* src, int m)
{
	
	char *dest;

	if (m<0 || m>(M-1)) return NULL;   /*In correct index */

	dest = data.pChar + m*N;    /*N columns = maximum length of string */
	memset(dest, 0, N);    /* Make sure that the memory is cleared before use */
	strncpy_s(dest,N,src,N-1); 
	return dest;
}




/*!
* \brief
*   Return a zero-terminated string, at a given row.
* 
* \param m
*  Row number
* 
* \returns
* Pointer to the string
* 
* \remarks
*  Remember that strings are stored in a transposed manner relative to the
*  numerical arrays.
* 
*/
char* oemArray::getSzStringAt(int m)
{

	if (m<-1 || m>N) return NULL;
	return data.pChar + m*N;
}




/*!
* \brief
*  Return a pointer to the ONLY string in oemArray.
*/
char* oemArray::getSzString()
{
	return getSzStringAt(0);
}



/*
 */

/*!
* \brief
* Parse a data string and set the values of an oemArray object.
* Supports comma-separated values. Values starting with (#) are
* treated as binary data. 
* 
* \param data_str
*    A text string with the data
* 
* \param len
*    The length of the data string
*
* \returns
*   A pointer to where data_str points. IF there is an error NULL is returned.
* 
* \remarks
*  This function assumes that only values have been left in the data string.
*  Two types are supported. The values can be given in a text format:
* 
*   value1, value2, 
* If they are text strings, then the text string is surrounded by inverted 
* commas:
*  "This is a text string"
*/
char* oemArray::setFromDataString(char* data_str, size_t len)
{
	char tmp[2048];
	char* ptmp;
	int index;
	int i;

	while (*data_str==' '){
		data_str++;           /* Remove white space*/
		len--;
	}

	index = 0;

	if(type==oemINT || type==oemDOUBLE){
		while (len > 0 && index < M*N){
			while (*data_str == ' ' || *data_str ==',' || *data_str == ';') {data_str++; len--; };

			if (*data_str == '#'){                   /* The rest is directly a numeric value */
				int no_digits = *(++data_str)-'0';
				int data_size = 0;
				int memsize = 0;
				int numElemsSet = 0;    // Number of elements set in this particular operation

				data_str ++;
				for (i = 0; i < no_digits; i++)
				{
					data_size = 10*data_size + *data_str++ - '0';
				}

				switch(type)
				{
				case oemINT:
					memsize = M*N*sizeof(int);
					numElemsSet = int(ceil(double(data_size) / sizeof(int)));
					break;

				case oemDOUBLE:
					memsize = M*N*sizeof(double);
					numElemsSet = int(ceil(double(data_size) / sizeof(double)));
					break;

				case oemCHAR:
					memsize = M*N;
					numElemsSet = data_size;
					break;
				}

				if (scalar == true)
				{
					if (memsize != data_size)
					{
						assert(false);
						data.vDouble = 0;
					}
					else
					{
						memcpy(&data.vChar, data_str, memsize);
					}
					
					data_str += data_size;
					return data_str;

				}
				else
				{
					if (memsize >= data_size)
					{
						memcpy(data.pChar, data_str,data_size);
					}
					else
					{
						memcpy(data.pChar, data_str, memsize);
						return data_str;
					}
					index += numElemsSet;
					data_str +=  data_size;
				}
				//return data_str;
				   
			}else if(*data_str == '"'){
				return data_str;         /* We have reached a text string*/
			}else{
				memset(tmp,0,2048);
				ptmp = tmp;
				
				while(*data_str!=' ' && *data_str!=',' && *data_str!=';' && len>0){
					*ptmp++ = *data_str++;
					len--;
				}
				if (type == oemDOUBLE){
					setDoubleAt(atof(tmp), index++);
				}else{
					setIntAt(atoi(tmp), index++);
				}
			}
		}
		return data_str;

	}else if(type == oemCHAR){
		if (scalar == true)
		{
			data.vChar = data_str[0];
			data_str = data_str + 1;
			return data_str;
		}
		while (len>1 && index < M){          /* N - number of characters in a string; M number of strings */
			while(*data_str == ' ' || *data_str == ',') {data_str++; len--;}; /* Skip delimiters */		
			if (*data_str == '#'){
				data_str ++;
				int no_digits = (*data_str++)-'0';
				int data_size = 0;
				int memsize = 0;

				for (i = 0; i < no_digits; i++)
					data_size = 10*data_size + *data_str++ - '0';

				memsize = M*N;
			
				if (memsize >= data_size){
					memset(data.pChar, 0, memsize);
					memcpy(data.pChar, data_str, data_size);
					data_str +=  data_size;
					return data_str;
				}

			}else if (*data_str == '"'){
				data_str ++;
				len --;

				memset(tmp,0,2048);
				ptmp = tmp;
				while(*data_str!='"' && len >0){
					*ptmp++ = *data_str++;
					len--;
				}
				
				data_str++;
				len--;    /* Skip last double quote (") */
				setSzStringAt (tmp, index);
				index++;
			}else{
				// This is the case when the tokens are not surrounded by (")
				memset(tmp, 0, 2028);
				ptmp = tmp;
				while(isalnum(*data_str)){
					*ptmp ++ = *data_str ++;
					len --;
				}
				setSzStringAt (tmp, index);
				index++;
			}

		}
	}else if(type==oemCELL){
		char* plast;
		plast = data_str+len;

		for (index=0; index < M*N; index++){
			int len1 = (int)(plast - data_str);
			if (len1 > 0){
				data_str = data.pArray[index].setFromDataString(data_str, len1);
			}
		}
	}else{
		return NULL; /* Unknown type*/
	}
	
	return NULL;
}



/* 

 */


/*!
* \brief
* Create a data string from the internal values.
* 
* \param dest
*   Destination - a pointer to the result
*
* \param len 
*     Size of the buffer.
*
* \param bin_format
*     Use binary format for the data 
* 
* \returns
*  Number of characters in the data string.
* 
*/
int oemArray::fillDataString(char* dest, size_t len, bool bin_format)
{
	int idx,i;
	int data_length, no_digits, mem_size;
	int divisor;
	int no_chars;
	char *orig_pointer;


	idx = 0;
	data_length = M*N;
	orig_pointer = dest;

	if (data_length>1024) bin_format = true;  /*No one can assess visually more than 1024 values*/
	
	switch(type){
			case oemINT:
				mem_size = data_length * sizeof(int);
				no_digits = (int)(floor(log10l(mem_size)))+1;

				if (bin_format == true){
					if (len<2+no_digits+data_length*sizeof(int)) return 0;  /* Make sure that there is enough space */
					
					dest[0] ='#';
					dest[1] = (char)no_digits + '0';
					divisor = (int)pow((double)10, no_digits-1);

					for (i=0; i< no_digits;i++){
						dest[i+2] = (char)(mem_size / divisor) + '0';
						mem_size = mem_size % divisor;
						divisor = divisor / 10;
					}
					if (scalar == false){
						memcpy(dest+no_digits+2, data.pInt, M*N*sizeof(int));
						dest += no_digits+2+M*N*sizeof(int);
					}else{
						memcpy(dest + no_digits+2, &data.vInt, sizeof(int));    // If a scalar, then only a single value is present
						dest += no_digits+2+sizeof(int);
					}
					return (int)(dest-orig_pointer);   // Return the number of characters written
				}else{
					for (idx = 0; idx < data_length; idx++){
						no_chars = sprintf_s(dest,len,"%d",getIntAt(idx,0));
						if (no_chars == -1) return (int)(dest-orig_pointer);  /* Written as much as humanly possible */

						dest += no_chars;
						len -= no_chars;

						if (idx < data_length-1){
							no_chars = sprintf_s(dest,len,"%s",", ");
							dest += no_chars;
							len -=no_chars;
						}
						if (len<1) return (int)(dest-orig_pointer);
					}
					return (int)(dest-orig_pointer);
				}	
				
								
				break;


			case oemDOUBLE:
				mem_size = data_length * sizeof(double);
				no_digits = (int)(floor(log10l(mem_size)))+1;

				if (bin_format == true){
					if (len<2+no_digits+data_length*sizeof(double)) return 0;  /* Make sure that there is enough space */

					dest[0] ='#';
					dest[1] = (char)no_digits + '0';
					divisor = (int)pow((double)10, no_digits-1);

					for (i=0; i< no_digits;i++){
						dest[i+2] = (char)(mem_size / divisor) + '0';
						mem_size = mem_size % divisor;
						divisor = divisor / 10;
					}
					if (scalar == false){
						memcpy(dest+no_digits+2, data.pDouble, M*N*sizeof(double));
						dest += no_digits+2+M*N*sizeof(double);
					}else{
						memcpy(dest + no_digits+2, &data.vDouble, sizeof(double));    // If a scalar, then only a single value is present
						dest += no_digits+2+sizeof(double);
					}
					
					return (int)(dest-orig_pointer);   // Return the number of characters written
				}else{
					for (idx = 0; idx < data_length; idx++){
						no_chars = sprintf_s(dest,len,"%g",getDoubleAt(idx,0));
						if (no_chars == -1) return (int)(dest-orig_pointer);  /* Written as much as humanly possible */

						dest += no_chars;
						len -= no_chars;

						if (idx < data_length-1){
							no_chars = sprintf_s(dest,len,"%s",", ");
							dest += no_chars;
							len -=no_chars;
						}
						if (len<1) return (int)(dest-orig_pointer);
					}
					return (int)(dest-orig_pointer);
				}	

  			 break;


			case oemCHAR:  /* Char arrays are treated as strings */
				/*Inspect if this is a single character */
				if (scalar)
				{
					data_length = 1;
				}
				else if (M==1 && data.pChar[N-1]== '\0')
				{ /*Handle the special case of a single string */
					data_length = (int)strlen(data.pChar)+1;
				};

				mem_size = data_length;
				no_digits = (int)(floor(log10l(mem_size)))+1;

				if (bin_format == true){
					if (len<(size_t)(2+no_digits+data_length)) return 0;  /* Make sure that there is enough space */

					dest[0] ='#';
					dest[1] = (char)no_digits + '0';
					divisor = (int)pow((double)10, no_digits-1);

					for (i=0; i< no_digits;i++){
						dest[i+2] = (char)(mem_size / divisor) + '0';
						mem_size = mem_size % divisor;
						divisor = divisor / 10;
					}
					if (scalar == false){
						memcpy(dest+no_digits+1, data.pChar, data_length);
						dest += no_digits+1+len;
					}else{
						memcpy(dest + no_digits+1, &data.vChar, sizeof(char));    // If a scalar, then only a single value is present
						dest += no_digits+1+sizeof(char);
					}
					return (int)(dest-orig_pointer);   // Return the number of characters written
				}else{
					if (scalar == false)
					{
						for (idx = 0; idx < M; idx++){
							no_chars = sprintf_s(dest, len,"\"%s\"",getSzStringAt(idx));
							if (no_chars == -1) return (int)(dest-orig_pointer);  /* Written as much as humanly possible */

							dest += no_chars;
							len -= no_chars;

							if (idx < M-1){
								no_chars = sprintf_s(dest,len,"%s",", ");
								dest += no_chars;
								len -=no_chars;
							}
							if (len<1) return (int)(dest-orig_pointer);
						}
					}
					else
					{
						dest[0] = this->data.vChar;
						dest[1] = '\0';
						dest ++;
					}
					return (int)(dest-orig_pointer);
				}	
				break;

			case oemCELL:
				orig_pointer = dest;
				for (idx = 0; idx < data_length; idx++){
					no_chars = data.pArray[idx].fillDataString(dest, len, bin_format);
					len -= no_chars;
					dest += no_chars;
					
					if (idx < data_length-1){
						no_chars = sprintf_s(dest,len,"%s",", ");
						dest += no_chars;
						len -=no_chars;
					}					
				}
				return (int)(dest-orig_pointer);
				break;

			case oemUNKNOWN:
			default:
				return 0;
	}
			
}




/*!
* \brief
* Create a command string from the data in oemArray
* 
* \param dest
*   A buffer where to store the command string
*
* \param len
*   Length of the buffer.
*
* \param with_view
*  If ~=0 the view is appended to the command.
* 
* \returns
*   Number of characters in the command string
* 
*/
int oemArray::fillCommandString(char* dest, size_t len, bool with_view)
{

	int no_chars;
	char *orig_ptr;
	

	orig_ptr = dest;

	if (with_view == true)
		no_chars = sprintf_s(dest,len,"COMMAND:%s:%c ",oem_name, view);
	else
		no_chars = sprintf_s(dest,len,"COMMAND:%s  ",oem_name, view);

	dest += no_chars;
	len -=no_chars;
	
	no_chars = fillDataString(dest,len, false);
	dest += no_chars;
	len -= no_chars;
	no_chars = sprintf_s(dest,len,";",oem_name);
	
	return (int) (dest-orig_ptr);
}



#define OEM_PAR_BUFFER_LEN   1024*1024
static char oem_par_buffer[OEM_PAR_BUFFER_LEN];
static char oem_par_types[][16]={"oemUNKNOWN", "oemDOUBLE", "oemINT", "oemCHAR", "oemCELL"};


/*!
* \brief
* Appends the value of an oemArray to a file
* 
* \param outfile
*    Pointer to a FILE structure as returned by fopen
*
* \param var
*    Pointer to oemArray
* 
* \returns
*   Returns 0 upon success
*/
int append_oem_array_to_file(FILE* outfile, oemArray* var)
{

	int result = 0;      // Success

	fseek(outfile, 0, SEEK_END);
	fprintf_s(outfile, "<oemArray>\n");
	fprintf_s(outfile, "oem_name=%s\n",var->oem_name);
	fprintf_s(outfile, "type=%s\n",oem_par_types[var->type]);
	fprintf_s(outfile, "view=%c\n", var->view);
	fprintf_s(outfile,"M=%d\n", var->M);
	fprintf_s(outfile,"N=%d\n", var->N);
	fprintf_s(outfile, "scalar=%d\n",(int)var->scalar);

	if (var->type==oemCELL){
		int data_length = var->M*var->N;
		int result;
		for (int idx = 0; idx<data_length; idx++){
			result = append_oem_array_to_file(outfile, &var->data.pArray[idx]);
		}
	}else{
		size_t data_str_len;
		char* next;
		size_t no_bytes_to_write;

		size_t no_written_bytes;

		memset(oem_par_buffer,0,OEM_PAR_BUFFER_LEN);

		bool binary = (var->type == oemDOUBLE);  // Double precision variables need binary output
		data_str_len = var->fillDataString(oem_par_buffer, OEM_PAR_BUFFER_LEN-1,binary);
		
		fprintf_s(outfile,"data_length=%d\n", data_str_len);

		next = oem_par_buffer;        /* Start writing from the start of buffer */

		while(data_str_len)
		{
			if (data_str_len>(64*1024))
				no_bytes_to_write = 64*1024;
			else 
				no_bytes_to_write = data_str_len;

			no_written_bytes = fwrite(next, 1, no_bytes_to_write, outfile);
			if (no_written_bytes != no_bytes_to_write)
			{
				printf("Error: %s \n",__FUNCTION__);
				printf("Failed writing to file");
				return -1;
			}
			data_str_len -= no_written_bytes;
			next = next + no_written_bytes;
		}
	}
	
	fprintf_s(outfile,"\n</oemArray>\n", var->oem_name);
	
	return result;
}



/*  
  helping function to separate right_value from left_value in expressions 
  of type:
  
  left_value = right_vale

 */

/*!
* \brief
*  Helping function to separate right_value from left_value in expressions 
*  of type: left_value = right_vale
* 
* \param left_value
*   Destination for the left value portion of the string
*
*\param left_len
*    Length of the buffer left_value
*
*\param right_value
*    Destination for the right-value portion of the string
*
*\param right_len 
*      Size of the string "right_value"
*
* \param src
*      Original string
* 
*/
void split_string(char *left_value, int left_len, char* right_value, int right_len, char* src)
{
	char *p1, *p2;

	memset(left_value, 0, left_len);
	memset(right_value, 0, right_len);
	
	p1 = src;
	p2 = left_value;
	
	while (*p1 == ' ') p1++;   /*Clear space in the start of line */
	
	/* Left token finishes either with space or equal sign */
	while (*p1 != ' ' && *p1 != '=' && *p1!=0 && (p2-left_value)<left_len)  *p2++=*p1++;

	/* Find equal sign */
	while (*p1 != '=' && *p1!=0) p1++;
    p1++;

	while (*p1 == ' ' ) p1++;   /*Clear space in start of right-side token */
	
	/*Right token finishes at white space */
	p2 = right_value;
	while((p2-right_value)<right_len && *p1!=' ' && *p1 !=';' && *p1!=0 && *p1!= 10 && *p1!=13) *p2++=*p1++;

}



/*
 *
 */

/*!
* \brief
* Read an OEM array from a file with OEM arrays.
* 
* \param var
*    Pointer to the destination object
*
* \param infile
*     File to read data from
* 
* \returns
*  0 upon success.
*/
int read_oem_array_from_file(oemArray *var, FILE* infile)
{
	char* next;
	int result;
	int found;
	
#define FOUND_NAME 1
#define FOUND_TYPE 2
#define FOUND_VIEW 4
#define FOUND_M 8
#define FOUND_N 16
#define FOUND_SCALAR 32
#define FOUND_DATA_LENGTH 64

#define FOUND_ALL 127

char attribute_name[128];
char attribute_val[128];


char name[128];
oemTypeID type;
char view;
int M = 0;
int N = 0;
bool scalar;
size_t data_length = 0u;


	result = 0;

	memset(oem_par_buffer,0, OEM_PAR_BUFFER_LEN);

	
	do{
		next = fgets(oem_par_buffer,OEM_PAR_BUFFER_LEN,infile);
		if (next!=oem_par_buffer){
			printf("ERROR: %s\n", __FUNCTION__);
			return -1;
		}
	}while(strncmp(next,"<oemArray>",strlen("<oemArray>")));
	
	found = 0;
	type = oemUNKNOWN;

	do{
		memset(oem_par_buffer,0,OEM_PAR_BUFFER_LEN);
		next = fgets(oem_par_buffer,OEM_PAR_BUFFER_LEN,infile);  
		if (next!=oem_par_buffer) return -1;

		if (strlen(next) > 1){
			split_string(attribute_name,128, attribute_val,128, oem_par_buffer);

			if (!strncmp(attribute_name,"oem_name", strlen("oem_name"))){
				found += FOUND_NAME;
				memset(name,0,128);
				strcpy_s(name, 128, attribute_val);

			}else if(!strncmp(attribute_name,"type", strlen("type"))){
				found += FOUND_TYPE;
				for (int i = 0; i < 5; i++){
					if (!strcmp(attribute_val,oem_par_types[i]))type = (oemTypeID)i;
				}
			}else if(!strncmp(attribute_name,"view", strlen("view"))){
				found += FOUND_VIEW;
				view = attribute_val[0];
			}else if(attribute_name[0]=='M'){
				found += FOUND_M;
				M = atoi(attribute_val);
			}else if(attribute_name[0] == 'N'){
				found += FOUND_N;
				N = atoi(attribute_val);
			}else if(!strncmp(attribute_name, "scalar", strlen("scalar"))){
				found += FOUND_SCALAR;
				scalar =  (0==atoi(attribute_val))?false:true;
			}else if(!strncmp(attribute_name, "data_length", strlen("data_length"))){
				found += FOUND_DATA_LENGTH;
				data_length = atoi(attribute_val);
			}else if(!strncmp(attribute_name,"</oemArray>",strlen("</oemArray>"))){
				found = FOUND_ALL+1;    // Should never happen if data is OK.
			}
		}
	}while(((found < FOUND_ALL) && (type!=oemCELL)) || ((found < FOUND_ALL-FOUND_DATA_LENGTH) && (type==oemCELL)));

	
	var->reformat(type,M,N,name);

	if (type==oemCELL){
		for (int idx = 0; idx < M*N; idx++){
			result = read_oem_array_from_file(&var->data.pArray[idx], infile);
			if (result != 0) return result;
		}
	}

	if (found>FOUND_ALL){
		/* Redefinition of some of the values*/
		return -1;
	}

	

	/* Read the buffer inside*/
	size_t remaining_data_length = data_length;
	size_t no_bytes_to_read;
	size_t no_bytes_read;

	

	/* Ensure that the program does not crash */
	if ( remaining_data_length> OEM_PAR_BUFFER_LEN) remaining_data_length = OEM_PAR_BUFFER_LEN;

	if (type != oemCELL){
		next = oem_par_buffer;
		while(remaining_data_length)
		{ 
			if ((remaining_data_length)>(64*1024))    // Some times fread is limited to 64 K
				no_bytes_to_read= 64*1024;
			else 
				no_bytes_to_read = remaining_data_length;

			no_bytes_read = fread(next, 1, no_bytes_to_read, infile);

			if (no_bytes_read != no_bytes_to_read)
			{

				printf("ERROR: %s :", __FUNCTION__ );
				printf(" Couldn't read from file ");
				return -1;
			}
			remaining_data_length -= no_bytes_read;
			next = next + no_bytes_read;
		}
		var->setFromDataString(oem_par_buffer, data_length);
	}

   /*Look for token for the end of file */
	do{
		next = fgets(oem_par_buffer,OEM_PAR_BUFFER_LEN,infile);
		if (next!=oem_par_buffer){
			printf("ERROR: %s\n", __FUNCTION__);
			return -1;
		}
	}while(strncmp(next,"</oemArray>",strlen("</oemArray>")));


	return result;
}





bool IsNumber(char* str, int strLen)
{
	if (!isdigit(str[0]) && !(str[0] == '.') && !(str[0] == '-'))
	{
		return false;
	}

	for(int n = 0; n < strLen; ++n)
	{
		if (!isdigit(str[n]) && (str[n] != '.') && (str[n] != '-') && (str[n] != 'E') && (str[n] != 'e') && (str[n] != '+' ) )
		{
			return false;
		}
	}
	return true;
}


bool IsInteger(char* str, int strLen)
{
	if (!IsNumber(str, strLen))
	{
		return false;
	}

	for (int n = 0; n < strLen; n++)
	{
		if (!isdigit(str[n]) && (str[n] != '-') && (str[n] != '+'))
		{
			return false;
		}
	}
	return true;
}

bool IsFloat(char* str, int strLen)
{
	return (IsNumber(str, strLen) && !IsInteger(str, strLen));
}



/*!
* \brief
* Fill the values of an oem array from a data string.
*
*   A Data string is a string in the format:
*
* DATA:NAME .., val1, val2, ...
*  It is returned by the scanner.
* 
* \param var
*   Pointer to an oemArray object
*
* \param str
*   Pointer to the data string
*
* \param inStrLen
*    Length of the data string 
* 
* \returns
* true upon success, false if it fails
* 
*/
int fill_oem_array_from_data_str(oemArray *var, char * str, size_t inStrLen)
{
#ifdef _MSC_VER
#pragma warning (disable: 4996; push)    // Begging for safe versions of strcpy, strcmp etc.
#endif

	bool success = true;
	char seperators[10];
	

	char* strCopy = new char[inStrLen+ 1] ;  
	char* name = NULL;                            // Will be allocated inside the function

	// Dependent on that str is terminated with a '\0'
	strcpy(strCopy, str);
	strcpy(seperators, ": ");

	char* token = strtok(str, seperators);

	if(token == NULL) {
		printf("Error, String format is probably wrong \n");
		return false;
	}

	//     The data string must begin with DATA or SDATA	
	if (strncmp(token, "DATA", 4)!= 0 && strncmp(token,"SDATA", 5) )
	{
		printf("Error, data string does not start with DATA or SDATA \n");
		success = false;
		goto data_str_to_oem_array_return;
	}


	token = strtok(NULL, seperators);

	if (token == NULL)
	{
		printf("Error, could not extract name \n");
		success = false;
		goto data_str_to_oem_array_return;

	}

	size_t nextTokenOffset = token - str + strlen(token);
	if (nextTokenOffset >= inStrLen)
	{
		printf("Error, there seems to lack data in the string \n");
		success = false;
		goto data_str_to_oem_array_return;

	}
	
	name = new char[strlen(token) + 1];
	strcpy(name, token);
	char view;     // View which is described by the data string
	if (strCopy[nextTokenOffset] == ':')
	{
		token = strtok(NULL, seperators);
		if (strlen(token) > 1)
		{
			printf("Error - the value for the view is wrong (%s) \n", token);
			success = false;
			goto data_str_to_oem_array_return;
		}
		view = token[0];
	}
	else
	{
		view = '\0';
	}
	
	size_t dataValuesOffset = token - str + strlen(token);

	if (dataValuesOffset >= inStrLen)
	{
		success = false;
		goto data_str_to_oem_array_return;
	}

	// from here on, we are looking for dimensions
	strcpy(seperators, ", ;#");

	int numVals = 0;
	oemTypeID type = oemUNKNOWN;
	size_t maxTokenLen = 0;

	while(token!=NULL)
	{
		token = strtok(NULL, seperators);
		if (token != NULL)
		{
			numVals ++;
			oemTypeID localType;
			size_t tokenLen = strlen(token);

			if (IsNumber(token, (int)tokenLen))
			{
				if (IsInteger(token, (int)tokenLen))
				{
					localType = oemINT;
				}
				else
				{
					localType = oemDOUBLE;
				}
			}
			else 
			{
				localType = oemCHAR;
			}
			// Records will be avoided if possible.
			// We are looking for the most descriptive type
			// int < double < char
			// If one of the tokens is char, then everything is char.
			// otherwise if one token is double, then everything is double,
			// otherwise if one int, then everything is INT. 
			
			if (localType == oemCHAR)
			{
				type = oemCHAR;
			}
			else if (localType == oemDOUBLE && type != oemCHAR)
			{
				type = oemDOUBLE;
			}
			else if (localType == oemINT && type != oemCHAR && type != oemDOUBLE)
			{
				type = oemINT;
			}

			maxTokenLen = (tokenLen > maxTokenLen) ? tokenLen : maxTokenLen;
		}
		
	}

	
	if (type == oemINT || type == oemDOUBLE)
	{
		var->reformat(type, numVals, 1, name);
	}
	else if (type == oemCHAR)
	{
		var->reformat(type, numVals, (int)maxTokenLen+1, name);
	}

	if (dataValuesOffset > inStrLen)
	{
		printf("Error : beginning of data points beyond the length of %s \n", strCopy);
		success = false;
		goto data_str_to_oem_array_return;
	}

	var->setFromDataString(strCopy + dataValuesOffset, inStrLen - dataValuesOffset);
	

data_str_to_oem_array_return:
	if (name != NULL) delete [] name;
	if (strCopy != NULL) delete [] strCopy;

	if (success == false)
	{
		var->reformat(oemCHAR, 1, 1,"invalid_data_string");
		var->setChar('0');
	}

	return ((success)?0:-1);

#ifdef _MSC_VER
#pragma warning (pop)
#endif
}



/// <summary> Compare is two variables of type oemArray are equal.
///
///  Two variables of the type oemArray are equal only when they 
///  are of the same type, dimensions, and their values are equal.
///
/// </summary>
bool cmp_oem_array_eq(const oemArray& a, const oemArray& b)
{
	bool retval = false;

	if ( a.type != b.type )
	{
		return false;
	}

	if ( a.M != b.M )
	{
		return false;
	}

	
	if ( a.N != b.N )
	{
		return false;
	}

	assert(a.scalar == b.scalar);  // Since dimensions are equal, this must be equal too. Otherwise there is a bug

	if (a.type == oemCELL)
	{
		/* CELL arrays are not supported yet. */
		assert(false);
		return false;
	}


	if (a.scalar == true)
	{
		switch(a.type)
		{
		case oemCHAR:
			retval = (a.data.vChar == b.data.vChar);
			break;

		case oemINT:
			retval = (a.data.vInt == b.data.vInt);
			break;

		case oemDOUBLE:
			retval = (a.data.vDouble == b.data.vDouble);
			break;
		}
		
	}
	else
	{
		switch(a.type)
		{
		case oemINT:
			retval = (0 == memcmp(a.data.pChar,b.data.pChar, a.M * a.N * sizeof(int)));
			break;

		case oemDOUBLE:
			retval = (0 == memcmp(a.data.pChar,b.data.pChar, a.M * a.N * sizeof(double)));
			break;

		case oemCHAR:
			retval = (0 == memcmp(a.data.pChar,b.data.pChar, a.M * a.N));
			break;
		}

	}
	return retval;
}
