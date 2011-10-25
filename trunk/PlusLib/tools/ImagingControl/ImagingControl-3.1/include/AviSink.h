// AviSink.h: interface for the AviSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISINK_H__75215A63_EB7B_46E2_AAFA_174B8EF4D77D__INCLUDED_)
#define AFX_AVISINK_H__75215A63_EB7B_46E2_AAFA_174B8EF4D77D__INCLUDED_

#pragma once

#include <string>
#include "udshl_defs.h"

#include "smart_ptr.h"
#include "FilterInfoObject.h"
#include "MediaStreamSink.h"

namespace _DSHOWLIB_NAMESPACE
{
	/** Using this Sink in a new Application is deprecated.
	 *
	 * The AviSink writes the image stream that comes from the currently active video capture device, to an avi file.
	 * If a codec is specified in the constructor, the image stream is compressed using this codec.
	 */
	class _UDSHL_EXP_API AviSink : public MediaStreamSink  
	{
	public:
		/** Create sink from filename and GUID.
		 * @param codec_type possible GUIDs are codec types or MEDIASUBTYPEs.
		 *			This may be GUID_NULL when the VideoFormat from the source should be used as avi stream type.
		 */
		AviSink( const std::string& filename, const GUID& codec_type, tSinkModes AutoStart = eRUN );
		AviSink( const std::wstring& filename, const GUID& codec_type, tSinkModes AutoStart = eRUN );
		/** create sink from filter object and optional a codec_type
		 * @param finfo the codec filter you want to use
		 */
		AviSink( const std::string& filename, const FilterInfoObject& finfo, tSinkModes AutoStart = eRUN );
		AviSink(  const std::wstring& filename, const FilterInfoObject& finfo, tSinkModes AutoStart = eRUN );

		/** dtor */
		virtual ~AviSink();

		/** Returns eAviSink
		 * @return eAviSink
		 */
		tSinkType						getSinkType() const;

		/** Returns the codec type as specified in the ctor. */
		GUID							getCodecType() const;
		/** The codec as specified in the ctor, may be 0.
		 * @return maybe 0
		 */
		smart_ptr<FilterInfoObject>		getFilterInfo() const;
	protected:
#pragma _DLL_EXPORT_WARNING_DISABLE
		smart_ptr<FilterInfoObject>		m_pFilterInfoBuffer;
#pragma _DLL_EXPORT_WARNING_ENABLE
	};

};

#endif // !defined(AFX_AVISINK_H__75215A63_EB7B_46E2_AAFA_174B8EF4D77D__INCLUDED_)
