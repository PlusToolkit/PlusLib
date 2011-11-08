
#ifndef _FILTERLOADER_H_INC_
#define _FILTERLOADER_H_INC_

#include "FrameFilter.h"

namespace _DSHOWLIB_NAMESPACE
{
	class IFrameFilter_param;

	/**
	 *	The FilterLoader methods allows you to load frame filters from external DLLs.
	 *
	 *	The FilterLoader loads the frame filters from the ".ftf" filter DLLs in the following directories :
			1) The path where this DLL resides. (tis_udshl_*.dll)
			2) The Application Path or if you called <link>FilterLoader::setLoadPath<link> in the specified
				path.
	 *
	 *	The loaded frame filter factories can be queried by calling getAvailableFrameFilters.
	 *	You can then create a specific filter by calling createFilter with a FilterInfo or
	 *	by specifying the according name.
	 *
	 *	As all methods of this class are static, you can not create instances of this class.
	 */
	class _UDSHL_EXP_API FilterLoader
	{
	public:
		/**
		 *	Internal use only.
		 */
		static void						setLoadPath( const std::wstring& path );

		/**
		 *	Retrieves a list with the names of the available frame filters.
		 *	To instantiate a filter from this list, call
		 *	<link>createFilter</link>.
		 *	@return A list with FilterInfo objects which describe the available filters.
		 */
		static std::vector<FilterInfo>	getAvailableFrameFilters( tFilterClass filterClass = eFC_ALL );

		/** Creates an instance of a specified frame filter.
		 * @return 0 when no according filter could be created, otherwise a reference to the newly created filter.
		 */
		static smart_com<IFrameFilter>	createFilter( const FilterInfo& fi );
		/** Creates an instance of a specified frame filter.
		 * When module == std::string(), then the loader creates the first filter with the specified name
		 *	ignoring the module.
		 * The look up of the filter name is case-sensitive.
		 * @return 0 when no according filter could be created, otherwise a reference to the newly created filter.
		 */
		static smart_com<IFrameFilter>	createFilter( const std::string& filter_name, const std::string& module = std::string() );
		static smart_com<IFrameFilter>	createFilter( const std::wstring& filter_name, const std::wstring& module = std::wstring() );
	private:
		/**
		 *	Do not create any instances of this class.
		 */
		FilterLoader() {}
	};
}

#endif