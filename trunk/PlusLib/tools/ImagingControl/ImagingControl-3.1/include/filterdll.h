
#ifndef _FILTERDLL_H_INC_
#define _FILTERDLL_H_INC_

#include "libbasedefs.h"

/** The factory interface and the according helper functions are defined here.
 *
 *	To export filters from your dll, use the macros defined below:
 *
 *	FILTERDLL_BEGIN_EXPORT()
 *		FILTERDLL_REGISTER_FILTER(CMyFilter1)
 *		FILTERDLL_REGISTER_FILTER(CMyFilter2)
 *		FILTERDLL_REGISTER_FILTER(CMyFilter2)
 *	FILTERDLL_END_EXPORT()
 *
 *	This registration has to be in any of the cpp files of your dll project.
 *	The classes have to be visible for the compiler at this point.
 */

#define FILTER_INTERFACE_VERSION 1

namespace _DSHOWLIB_NAMESPACE
{
	class IFrameFilter;
	struct FilterInfo;

	/**
	 *	Abstract base class for a filter factory. An implementation of this
	 *	interface has to be created by <link>getFilterFactory</link>.
	 *	
	 *	IFilterFactory has methods for enumeration, creation and removal of
	 *	Frame-/Update-Transform-filters
	 */
	class IFilterFactory
	{
	public:
		virtual bool enumFilters( unsigned int index, FilterInfo* pFilterInfo ) = 0;

		virtual IFrameFilter*		createFilter( const wchar_t* name ) = 0;
		
		virtual int	getVersion() = 0;
	};
}

#define FILTERDLL_BEGIN_EXPORT() \
	extern "C" __declspec(dllexport) _DSHOWLIB_NAMESPACE::IFilterFactory*	getFilterFactory() \
	{ \
		static bool init = false; \
		static _DSHOWLIB_NAMESPACE::CFilterFactory fac; \
		if( !init ) \
		{

#define FILTERDLL_REGISTER_FILTER(CFilter) \
			fac.registerFilter( (CFilter*) 0 );

#define FILTERDLL_END_EXPORT() \
			init = true; \
		} \
		return &fac; \
	}

#endif