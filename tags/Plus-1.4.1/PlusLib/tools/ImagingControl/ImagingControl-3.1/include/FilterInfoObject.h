// FilterInfoObject.h: interface for the FilterInfoObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERINFOOBJECT_H__4ACDB17A_C8F5_4932_B676_CC930A39EE6D__INCLUDED_)
#define AFX_FILTERINFOOBJECT_H__4ACDB17A_C8F5_4932_B676_CC930A39EE6D__INCLUDED_

#pragma once

#include <string>
#include "udshl_defs.h"

#include "smart_com.h"
#include "int_interface_pre.h"

namespace _DSHOWLIB_NAMESPACE
{
	class Grabber;
	class AviSink;
	class MediaStreamSink;

	/** This class is deprecated and can only be used in conjunction with the AviSink.
	 * The Codec and MediaStreamSink classes supersede this class. 
	 *
	 * A FilterInfoObject contains the name of a video codec used for avi file compression.
	 * deprecated, use the Codec object.
	 */
	class _UDSHL_EXP_API FilterInfoObject  
	{
		friend AviSink;
		friend Grabber;
		friend MediaStreamSink;
	public:
		/// creates an invalid object
		FilterInfoObject();
		/// copy ctor
		FilterInfoObject( const FilterInfoObject& );
		/// dtor
		virtual ~FilterInfoObject();

		FilterInfoObject&	operator=( const FilterInfoObject& );

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
			getInternalInterface_( __uuidof( TItf ), (void**) &pItf.get() );
			return pItf;
		}

		/** returns true if this object is valid */
		bool				isValid()	const;
		/** returns the readable name of this Filter */
		std::string			getName()	const;
		/** returns the name as char */
		const char*			c_str()		const;

		/** Creates a textual representation for this FilterInfoObject
		 * @return The textual representation.
		 */
		std::string		toString() const;
		std::wstring	toStringW() const;


		/** test if this lies alphabetical before op.
		 * @param op format to compare to this
		 * @return true if this < op, else false
		 **/
		bool			operator<( const FilterInfoObject& op ) const;

		/** test if two formats are equal
		 * @param op format to compare to this
		 * @return true if this and op are equal, else false
		 **/
		bool			operator==( const FilterInfoObject& op ) const;

		/** test if two formats are not equal
		 * @param op format to compare to this
		 * @return false if this and op are equal, else true
		 **/
        bool            operator!=( const FilterInfoObject& op ) const;

		/** test if this is equal to the string passed in op
		 * @param op item to compare to this
		 * @return true if this and op are equal, else false
		 **/
        bool            operator==( const std::string& op ) const;
		bool            operator==( const std::wstring& op ) const;

		/** get string representing the Format
		 * @return a string representation describing the format
		 * @see getFormatString()
		 **/
		operator const char* () const;

		/** generates an invalid item
		 * @return an invalid item
		 * @see isValid()
		 **/
		static FilterInfoObject createInvalid();

		/** returns true if this CODEC has an Dialog. */
		bool			hasDialog() const;
		
		/** calls the Dialog and saves the Data retrieved after the operation in the local buffer.*/
		bool			callDialog( HWND hParent = 0 );

		/**	Retrieves the current data set in the codec.
		 * @param pData	The data buffer to copy the data into. May be 0 when you only want to retrieve the size of the buffer.
		 * @param size	IN	The size of the buffer.
		 *				OUT	The size of the data.
		 * @return true on success, otherwise false.
		 */
		bool			getCodecData( BYTE* pData, DWORD& size ) const;
		bool			setCodecData( BYTE* pData, DWORD data_size );

		/** returns true if the codec contained is an Video for Windows CODEC */
		bool			isVfwCodec() const;

		/**	Returns if you can get/set the codec data.
		 * @return true/false
		 */
		bool			isCodecDataAvailable() const;

		bool			hasCodecData() const;

		std::string		getCodecData() const;
		std::wstring	getCodecDataW() const;

		bool			setCodecData( const std::string& data );
		bool			setCodecData( const std::wstring& data );
	private:
		HRESULT			getInternalInterface_( REFIID riid, void** ppv );

		static	void	createCodecList( std::vector<FilterInfoObject>& listToFill );

		/// internal ctor
		FilterInfoObject( const smart_com<icbase::IDShowFactoryObjectInfo>& op2 );
		FilterInfoObject( const smart_com<icbase::IDShowFilter>& op2 );

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

#endif // !defined(AFX_FILTERINFOOBJECT_H__4ACDB17A_C8F5_4932_B676_CC930A39EE6D__INCLUDED_)
