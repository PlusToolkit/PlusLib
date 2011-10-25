/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _BINSEARCH_H
#define _BINSEARCH_H

template <class T, class Compare> bool bs_find( const T &item, T *data, int dlen )
{
	T *lower = data;
	T *upper = data + dlen - 1;
	while ( true ) {
		if ( upper < lower )
			return false;

		T *mid = lower + ((upper-lower)>>1);
		long cmp = Compare::compare( item, *mid );
		if ( cmp < 0 )
			upper = mid - 1;
		else if ( cmp > 0 )
			lower = mid + 1;
		else
			return true;
	}
}

template <class T, class Compare> void bs_insert( const T &item, T *data, int &dlen )
{
	T *lower = data;
	T *upper = data + dlen - 1;
	while ( true ) {
		if ( upper < lower )
			goto insert;

		T *mid = lower + ((upper-lower)>>1);
		long cmp = Compare::compare( item, *mid );
		if ( cmp < 0 )
			upper = mid - 1;
		else if ( cmp > 0 )
			lower = mid + 1;
		else
			return;
	}
insert:
	int pos = lower - data;
	int newLen = dlen + 1;

	/* Shift over data at insert spot if needed. */
	if ( dlen > 0 && pos < dlen )
		memmove(data+pos+1, data+pos, sizeof(T)*(dlen-pos));

	/* Save the new length. */
	dlen = newLen;
	data[pos] = item;
}

#endif
