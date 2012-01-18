
#ifndef MEDIASTREAMCONTAINER_H_INC_
#define MEDIASTREAMCONTAINER_H_INC_

#pragma once

#include <string>
#include <vector>
#include "udshl_defs.h"
#include "smart_com.h"

#include "int_interface_pre.h"

namespace _DSHOWLIB_NAMESPACE
{
	class MediaStreamSink;
	class MediaStreamContainer;

	typedef smart_ptr<MediaStreamContainer>			tMediaStreamContainerPtr;
	typedef std::vector<tMediaStreamContainerPtr>	tMediaStreamContainerList;
	typedef smart_ptr<tMediaStreamContainerList>	tMediaStreamContainerListPtr;

	_UDSHL_EXP_API extern const GUID MSC_AviContainer;
	_UDSHL_EXP_API extern const GUID MSC_OGMContainer;
	_UDSHL_EXP_API extern const GUID MSC_MatroskaContainer;

	/** This class represents a media stream container, which represents the container destination for
	 * media streams.
	 * Standard container types are "AVI" or "OGM".
	 */
	class _UDSHL_EXP_API MediaStreamContainer
	{
		friend MediaStreamSink;
	public:
		/// dtor
		virtual ~MediaStreamContainer();

		/** Returns a list of available container types.
		 * @return May be 0 when a critical error occurred. May be empty (size() == 0) when no containers were found.
		 *			Otherwise contains the available containers as present in the system.
		 */
		static tMediaStreamContainerListPtr	getAvailableMediaStreamContainers();

		/** Creates the container specified with the id.
		 * @param id The container id of the container.
		 * @return 0 when the container could not be found/created, otherwise a reference to the container.
		 */
		static tMediaStreamContainerPtr		create( const GUID& id );

		/** This creates a copy of the stream container, which is completely independent from the
		 * current object, but copies the internal state and data of the container.
		 * @return
		 */
		tMediaStreamContainerPtr	clone() const;

		/**	Returns a reference to an internal interface.
		* With this function you can fetch an custom interface from the codec.
		* To use this function your compiler must support the __uuidof operator and the interface must
		* be assigned an iid with the __declspec( uuid( "iid" ) ) compiler option. When this option is
		* not available then you should use the other function.
		* \param pItf
		* \return A reference to the interface requested or 0 if the interface is not supported.
		*/
		template<class TItf>
		smart_com<TItf>		getInternalInterface( smart_com<TItf>& pItf ) const
		{
			return getInternalInterface( __uuidof( TItf ), pItf );
		}

		/**	Returns a reference to an internal interface.
		* With this function you can fetch an custom interface from the codec.
		* \par usage
		*
		*	smart_com<ICodecInterface> pItf;
		*	if( pFilterInfoObject->getInternalInterface( pItf ) == 0 )
		*	{
		*		... // interface is not supported, so error handling
		*	}
		*	else
		*	{
		*		...	// use the interface
		*	}
		*
		* \param pItf A smart_com to a interface reference.
		* \param riid An interface ID.
		* \return A reference to the interface requested or 0 if the interface is not supported.
		*/
		template<class TItf>
		smart_com<TItf>		getInternalInterface( REFIID riid, smart_com<TItf>& pItf ) const
		{
			pItf = 0;
			getInternalInterface_( riid, (void**) &pItf.get() );
			return pItf;
		}

		/** The string representation of this container. */
		std::string		getDescription() const;
		std::wstring	getDescriptionW() const;
		/** The string representation of this container. */
		std::string		toString() const;
		std::wstring	toStringW() const;

		bool			operator==( const std::string& ) const;
		bool			operator==( const std::wstring& ) const;

		/** Returns the unique container ID, which you can use to create the container without refering to
		 * the description string.
		 */
		GUID			getMediaStreamContainerID() const;

		/** Returns if this sink does support a codec. If yes then you can create a sink
		 * with a FilterInfoObject which contains the codec to use.
		 * Certain container do support codecs, like AVI others don't like MPEG2.
		 */
		bool			isCustomCodecSupported() const;

		/** Returns the standard file extension used by this container.
		 * e.g. "avi" for AVI files.
		 * @return The file extension.
		 */
		std::string		getPreferredFileExtension() const;
		std::wstring	getPreferredFileExtensionW() const;
	private:
		/// copy ctor
		MediaStreamContainer( const MediaStreamContainer& );

		MediaStreamContainer&	operator=( const MediaStreamContainer& );

		MediaStreamContainer( const smart_com<icbase::IDShowFactoryObjectInfo>& pF );
		MediaStreamContainer( const smart_com<icbase::IDShowFactoryObjectInfo>& pInfo, 
			const smart_com<icbase::IDShowFilter>& pF );

		HRESULT			getInternalInterface_( REFIID riid, void** ppv );

		smart_com<icbase::IDShowFilter>				getFilter() const;

#pragma _DLL_EXPORT_WARNING_DISABLE
		mutable smart_com<icbase::IDShowFilter>				m_pFWFilter;
		mutable smart_com<icbase::IDShowFactoryObjectInfo>	m_pFWFilterInfo;
#pragma _DLL_EXPORT_WARNING_ENABLE
	};
};

#endif // MEDIASTREAMCONTAINER_H_INC_
