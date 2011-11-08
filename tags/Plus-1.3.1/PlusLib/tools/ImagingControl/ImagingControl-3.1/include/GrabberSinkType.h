// GrabberSinkType.h: interface for the GrabberSinkType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRABBERSINKTYPE_H__7341AF44_4BE7_42A9_8825_DAE0A0724BA1__INCLUDED_)
#define AFX_GRABBERSINKTYPE_H__7341AF44_4BE7_42A9_8825_DAE0A0724BA1__INCLUDED_

#pragma once

#include "udshl_defs.h"
#include "smart_com.h"

#include "simplectypes.h"

#include "int_interface_pre.h"

namespace _DSHOWLIB_NAMESPACE
{
	class GrabberPImpl;
	class Grabber;

	/** Pure virtual base class for all sinks. The currently available sink objects are
	 * MediaStreamSink and FrameHandlerSink (and the deprecated FrameGrabberSink and AviSink).
	 */
	class _UDSHL_EXP_API GrabberSinkType  
	{
		friend Grabber;
		friend GrabberPImpl;
	public:
		/** dtor */
		virtual ~GrabberSinkType();

		/** must return the sink id*/
		virtual tSinkType	getSinkType() const = 0;

		///< represents the mode in which the sink currently is/ is set
		enum tSinkModes
		{
			eRUN	= 0x1,
			ePAUSE	= 0x2,
			/// you cannot set this mode, this is only for the case that no graph is built
			eUNCONNECTED = -1,
		};
		/** sets the mode of the sink.
		 * So by calling this function you can pause and restart the sink.
		 * @return the mode in which the sink is set after the command.
		 */
		virtual tSinkModes	setSinkMode( tSinkModes mode );
		/** returns the current sink mode */
		virtual tSinkModes	getSinkMode() const;
		/** Returns if the sink is currently attached to a Grabber */
		bool		isAttached() const;
	protected:
		GrabberSinkType();

		virtual bool	attach( GrabberPImpl* );
		virtual void	detach();

		virtual smart_com<icbase::IDShowFilter>			getBaseSinkFilter() const = 0;

		GrabberPImpl*		m_pAttachedParent;
	};
};

#endif // !defined(AFX_GRABBERSINKTYPE_H__7341AF44_4BE7_42A9_8825_DAE0A0724BA1__INCLUDED_)
