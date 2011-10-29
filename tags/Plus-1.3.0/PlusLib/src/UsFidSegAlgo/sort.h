/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _SORT_H
#define _SORT_H

#include <string.h>

template<class T, class LessThan> void doSort(T *tmpStor, T *data, long len)
{
	if ( len <= 1 )
		return;

	long mid = len / 2;

	doSort<T, LessThan>( tmpStor, data, mid );
	doSort<T, LessThan>( tmpStor + mid, data + mid, len - mid );
	
	/* Merge the data. */
	T *endLower = data + mid, *lower = data;
	T *endUpper = data + len, *upper = data + mid;
	T *dest = tmpStor;
	while ( true ) {
		if ( lower == endLower ) {
			/* Possibly upper left. */
			if ( upper != endUpper )
				memcpy( dest, upper, (endUpper - upper) * sizeof(T) );
			break;
		}
		else if ( upper == endUpper ) {
			/* Only lower left. */
			if ( lower != endLower )
				memcpy( dest, lower, (endLower - lower) * sizeof(T) );
			break;
		}
		else {
			/* Both upper and lower left. */
			if ( LessThan::lessThan( *upper, *lower ) )
				memcpy( dest++, upper++, sizeof(T) );
			else
				memcpy( dest++, lower++, sizeof(T) );
		}
	}

	/* Copy back from the tmpStor array. */
	memcpy( data, tmpStor, sizeof( T ) * len );
}

template<class T, class LessThan> void sort(T *data, long len)
{
	/* Allocate the tmp space needed by merge sort, sort and free. */

	// Thomas Kuiran Chen - retouched for ANSI-C++
	// ANSI-C++ does not allow an initialization of static array
	// data using a variable.  A walk-around is to use the dynamic
	// allocation of the array.
	//T tmpStor[len];
	T *tmpStor = new T[len];
	doSort<T, LessThan>( tmpStor, data, len );

	// Thomas Kuiran Chen - make sure to clean up the memory.
	delete [] tmpStor;
	tmpStor = NULL;
}

#endif /* _SORT_H */
