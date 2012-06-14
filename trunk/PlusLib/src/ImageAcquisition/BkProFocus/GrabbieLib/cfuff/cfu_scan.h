#ifndef __cfu_scan_h
#define __cfu_scan_h

/**********************************************************************
  NAME      : cfu_scan.h
  ABSTRACT  : Header file, defining the format of storing ultrasound data 
              in a file. 
  VERSION   : 1.0, 15 Dec 1999, Svetoslav Nikolov
**********************************************************************/


#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define FIELD_NAME_LENGTH 30
#define FIELD_VAL_LENGTH  20

#define FORMAT_VERSION "1.0"     /* Version of Format                 */

/*
 *  In the beginning of each file, there is a header, with fixed length
 *  This header has a fixed size, and is read when opening the file. It
 *  contains a number of entries. Here follows the definition of a single
 *  entry.
 */
 
typedef struct{
  char name[FIELD_NAME_LENGTH];  /* Name of the field                 */
  char val[FIELD_VAL_LENGTH];    /* Value of the field                */
} THeaderEntry;


/*
 * This is the actual header of the file. 
 */
 
typedef struct{
  THeaderEntry who;                    /* Who made the file - CFU   */ 
  THeaderEntry version;                /* Version of the foramt     */
  THeaderEntry no_volumes;
  THeaderEntry no_planes;
  THeaderEntry no_planes_per_volume;
  THeaderEntry no_lines_per_plane;
  THeaderEntry no_samples_per_line;
  THeaderEntry no_bits_per_sample;
}TScanHeader;


/*
 * When the user opens a file, he/she must get a pointer to a file
 * structure back. Here follows the definition of such a structure
 *
 */
//#ifndef __cff_mex_h
#ifdef _MSC_VER
typedef unsigned __int32 uint;
#else
typedef uint32_t uint;
#endif

//#endif

/*
 *  The functions that are provided work with files in a manner
 *  similar to the functions from <stdio.h>, and return pointers
 *  to structures that describe the scan-file
 */
typedef struct{
  FILE* fl;       
  uint no_volumes;
  uint no_planes;
  uint no_planes_per_volume;
  uint no_lines_per_plane;
  uint no_samples_per_line;
  uint no_bits_per_sample;
}TScanFile;

#ifdef __cplusplus
	extern"C"{
#endif

/*
 *   EXPORTED FUNCTIONS
 */
                            
TScanFile* scanFileCreate(char* file_name,
                         uint no_planes_per_volume,
                         uint no_lines_per_plane,
                         uint no_samples_per_line,
                         uint no_bits_per_sample );



TScanFile* scanFileOpen(char* file_name);
void scanFileClose(TScanFile* f);
void scanFilePrintInfo(TScanFile* f);
int scanFilePlaneAppend(TScanFile* f, const void* data);
int scanFilePlaneRead(const TScanFile* f, uint plane_no, void* data);

#ifdef __cplusplus
	};
#endif



#endif
