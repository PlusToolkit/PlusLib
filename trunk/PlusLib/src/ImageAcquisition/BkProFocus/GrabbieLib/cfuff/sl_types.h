/***********************************************************************
 NAME      : sl_types.h
 
 ABSTRACT  : Definitons for the common types, used in the sys_lib
 
 CREATED   : 15 Dec 1998, Svetoslav Nikolov

 MODIFIED  : 15 Dec 1998, framework created. Basic types defined
             22 Jan 1999, added types for compatibility with Field II.
***********************************************************************/

#ifndef __sl_types_H
#define __sl_types_H


/* Definitions */

 #define TRUE   1
 #define FALSE  0

 #define PIm2      6.2831855307179586
#ifndef PI 
 #define PI        3.141592653589793
#endif 
#define EPS        1e-6 
#define DEPS       1e-14
#define PRECISION  1e-60
#define SWAP(a,b)  {tempr=(a);(a)=(b);(b)=tempr;} 

#define HW_MIN(a,b)  ((a)<(b))?(a):(b)
#define HW_MAX(a,b)  ((a)>(b))?(a):(b)
/* Types */

typedef signed int sint;
typedef unsigned long dword;
typedef unsigned short word; 
typedef unsigned char byte;

#ifndef VCPP
 typedef int boolean;
#else
 #define boolean BOOLEAN
#endif

typedef long int32;                      /* long is ALWAYS 32 bits.  */
typedef char int64[8];                   /* This is 64 bit integer   */
typedef double TReal;                    /* for MATLAB compatibility */
typedef int THandle;                     /* for defining handlers    */
typedef int TCount;                      /* Number of things ...     */
typedef dword TSize;                     /* Size of an argument      */
typedef byte* TPtr;                    /* Pointer to address       */
/*-------------------------------------*/
/* Now define a general type variable  */
/*-------------------------------------*/

typedef union{
  char  vString[50];
  long  vLong;
  double vDouble;
}TVar;

typedef struct Point{
   TReal x,y,z;                           /*  Coordinates of a point */
} TPoint;

typedef struct Vector{
  TReal x , y, z;
} TVector;

#endif   /* ifndef __sl_types_H  */
