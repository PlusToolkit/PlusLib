// VideoCaptureDeviceItem.h: interface for the VideoCaptureDeviceItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOCAPTUREDEVICEITEM_H__7ADEF0BC_B575_4806_8ED2_BE6961FE68DE__INCLUDED_)
#define AFX_VIDEOCAPTUREDEVICEITEM_H__7ADEF0BC_B575_4806_8ED2_BE6961FE68DE__INCLUDED_

#pragma once

#include <string>
#include "udshl_defs.h"

#include "smart_com.h"
#include "int_interface_pre.h"

namespace _DSHOWLIB_NAMESPACE
{
	class Grabber;
	class CSourceFilterType;
	class CVideoCaptureDeviceEnum;

	/** En capsules the data to create a VideoCaptureDevice.
	 */
	class _UDSHL_EXP_API VideoCaptureDeviceItem  
	{
		friend Grabber;
		friend CVideoCaptureDeviceEnum;
	public:
		/** creates an invalid VideoCaptureDeviceItem. */
		VideoCaptureDeviceItem();

		/** copy constructor
		 * @param op2 VideoCaptureDeviceItem to copy info new one
		 **/
		VideoCaptureDeviceItem( const VideoCaptureDeviceItem& op2 );
		/** dtor */
		virtual ~VideoCaptureDeviceItem();

		/** assignment operator.
		 * @return this
		 * @param op2
		 */
		VideoCaptureDeviceItem& operator=( const VideoCaptureDeviceItem& op2 );

		/** compares two VideoCaptureDeviceItems
		 * @param op2 VideoCaptureDeviceItem to compare with this
		 * @return true, if the device ID and the device name are equal, else false
		 **/
		bool				operator==( const VideoCaptureDeviceItem& op2 ) const;

		/** compares this to a string
		 * @param op2 string
		 * @return true/false
		 */
		bool				operator==( const std::string& op2 ) const;
		bool				operator==( const std::wstring& op2 ) const;

		/** test if instance is a valid device
		 * @return true if <code>this</code> is valid
		 **/
		bool				isValid() const;

		/** returns the name of the capture device, as returned from its driver
		 * @return the name of the capture device
		 **/
		const std::string&	getName() const;

		/** Creates a textual representation for this VideoCaptureDeviceItem
		 * @return The textual representation.
		 */
		std::string			toString() const;
		std::wstring		toStringW() const;

		/** returns the serial number
		 * @return true on success, false otherwise
		 */
		bool				getSerialNumber( __int64& val ) const;

		/** returns a unique name for this device. This is only available devices which return
		 * a serial number.
		 * @return When the device has a SerialNumber then a unique name, otherwise
		 *				a empty string is returned (getUniqueName().empty() == true)
		 */
		std::string			getUniqueName() const;
		std::wstring		getUniqueNameW() const;

		/**	returns the unique name of the device in this system.
		 * USB devices keep the same name as long as you do not change the USB port at which this
		 * device resides.
		 * IEEE 1394 devices keep their names, but may have a different name on a different machine.
		 * @return The display name of the device.
		 */
		std::string			getDisplayName() const;
		std::wstring		getDisplayNameW() const;

		/** returns the base name of the device without the identification number appended to the device name.
		 * @return The base name of the device.
		 */
		std::string			getBaseName() const;
		std::wstring		getBaseNameW() const;

		/** returns the name of the capture device, as returned from its driver
		 * @return the name of the capture device
		 **/
		const char*			c_str() const;

		/** returns the string representation (i. e. the device name) of the captue device
		 * @return string representation of the VideoCaptureDevice
		 **/
		operator const char* () const;

		/** returns the result of a lexicographical compare of the name
		 * @return true, if this is before op
		 * @param op the VideoCaptureDeviceItem to compare with
		 **/
		bool				operator<(const VideoCaptureDeviceItem& op) const;

		/** generates an invalid device
		 * @return an invalid device
		 * @see isValid()
		 **/
		static				VideoCaptureDeviceItem createInvalid();

		/** Compares the device's serial number with a given serial number
		 */
		bool				operator==( const __int64& serial ) const;

		/**	Returns a reference to an internal interface.
		 * With this function you can fetch an custom interface from the codec.
		 * To use this function your compiler must support the __uuidof operator and the interface must
		 * be assigned an iid with the __declspec( uuid( "iid" ) ) compiler option. When this option is
		 * not available then you should use the other function.
		 * \param pItf
		 * \return A reference to the interface requested or 0 if the interface is not supported.
		 */
		template<class TItf>
		smart_com<TItf>		getInternalInterface( smart_com<TItf>& pItf ) 
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
		smart_com<TItf>		getInternalInterface( REFIID riid, smart_com<TItf>& pItf ) 
		{
			pItf = 0;
			getInternalInterface_( riid, (void**) &pItf.get() );
			return pItf;
		}
	private:
		/// internal ctor
		VideoCaptureDeviceItem( icbase::IDShowFactoryObjectInfo* pInfo, unsigned int num );

		HRESULT			getInternalInterface_( REFIID riid, void** ppv );

		void							bind() const;
		void							unbind();
		smart_com<CSourceFilterType>	getSourceFilterType() const;

#pragma _DLL_EXPORT_WARNING_DISABLE
		mutable smart_com<CSourceFilterType>	m_pSourceFilter;
		
		/// internal data
		icbase::IDShowFactoryObjectInfo*		m_pInternalData;

		/// user readable name
		std::wstring						m_ReadableName;
		mutable std::string					m_ReadableNameA; // Buffer for c_str
#pragma _DLL_EXPORT_WARNING_ENABLE
	};
};

#endif // !defined(AFX_VIDEOCAPTUREDEVICEITEM_H__7ADEF0BC_B575_4806_8ED2_BE6961FE68DE__INCLUDED_)
