
#ifndef CODEC_H_INC_
#define CODEC_H_INC_

#pragma once

#include <string>
#include <vector>
#include "udshl_defs.h"

#include "smart_ptr.h"
#include "smart_com.h"

#include "int_interface_pre.h"

namespace _DSHOWLIB_NAMESPACE
{
	class AviSink;
	class MediaStreamSink;

	class Codec;

	typedef smart_ptr<Codec>		tCodecPtr;
	typedef std::vector<tCodecPtr>	tCodecList;
	typedef smart_ptr<tCodecList>	tCodecListPtr;

	/** Represents a Codec filter, which can be used in conjunction with the MediaStreamSink. */
	class _UDSHL_EXP_API Codec  
	{
		friend AviSink;
		friend MediaStreamSink;
	public:
		/** Returns a tCodecListPtr containing the video compressors that were found on your system. 
		 */
		static tCodecListPtr	getAvailableCodecs();

		///< dtor
		virtual ~Codec();

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
		 *	if( pCodec->getInternalInterface( pItf ) == 0 )
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

		/** Returns the readable name of this Filter */
		std::string			getName()	const;

		/** Creates a textual representation for this Codec
		 * @return The textual representation.
		 */
		std::string			toString() const;
		std::wstring		toStringW() const;

		/** test if two formats are equal
		 * @param op format to compare to this
		 * @return true if this and op are equal, else false
		 **/
		bool			operator==( const Codec& op ) const;

		/** test if two formats are not equal
		 * @param op format to compare to this
		 * @return false if this and op are equal, else true
		 **/
        bool            operator!=( const Codec& op ) const;

		/** test if this is equal to the string passed in op
		 * @param op item to compare to this
		 * @return true if this and op are equal, else false
		 **/
        bool            operator==( const std::string& op ) const;
		bool            operator==( const std::wstring& op ) const;

		/** returns true if this CODEC has an Dialog. */
		bool			hasDialog() const;
		
		/** calls the Dialog and saves the Data retrieved after the operation in the local buffer.*/
		bool			callDialog( HWND hParent = 0 );

		/** returns true if the codec contained is an Video for Windows CODEC */
		bool			isVfwCodec() const;

		/**	Returns if you can get/set the codec data.
		 * @return true/false
		 */
		bool			hasSettings() const;

		/** Returns a string containing the settings of the codec. 
		 * The string may be empty, when no data can be saved/restored by the codec.
		 */
		std::string		getSettings() const;
		std::wstring	getSettingsW() const;

		/** This method is used to set previously retrieved codec data.
		 * @return true on success, false on failure
		 */
		bool			setSettings( const std::string& data );
		bool			setSettings( const std::wstring& data );

	private:
		HRESULT			getInternalInterface_( REFIID riid, void** ppv );

		/// internal ctor
		Codec( const smart_com<icbase::IDShowFactoryObjectInfo>& op2 );
		Codec( const smart_com<icbase::IDShowFilter>& op2 );

		bool			setCodecData( BYTE* pData, DWORD data_size, bool bPure );
		bool			getCodecData( BYTE* pData, DWORD& size, bool bPure ) const;

		/** returns the pointer to the current compressor filter, only needed in build filter graph in grabber.cpp */
		smart_com<icbase::IDShowFilter>				getFilter() const;

#pragma _DLL_EXPORT_WARNING_DISABLE
		/// the user friendly readable name
		std::string							m_ReadableName;

		/// holds the information to create a filter
		mutable smart_com<icbase::IDShowFactoryObjectInfo>	m_pFilterInfo;
		mutable smart_com<icbase::IDShowFilter>				m_pFilter;		///< Instance of the compressor filter
#pragma _DLL_EXPORT_WARNING_ENABLE
	};
};

#endif // CODEC_H_INC_
