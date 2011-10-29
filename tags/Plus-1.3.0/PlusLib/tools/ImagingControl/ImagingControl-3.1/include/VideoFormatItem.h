// VideoFormatItem.h: interface for the VideoFormatItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOFORMATITEM_H__AD23522D_3281_4115_8943_C22F95EE0957__INCLUDED_)
#define AFX_VIDEOFORMATITEM_H__AD23522D_3281_4115_8943_C22F95EE0957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "udshl_defs.h"

#include "int_interface_pre.h"


namespace _DSHOWLIB_NAMESPACE
{
	class Grabber;
	class GrabberPImpl;

	/** Represents a certain Video Format with its size and MEDIASUBTYPE/Colorformat.
	 * All members will deliver valid values only if isValid() returns true
	 **/
	class _UDSHL_EXP_API VideoFormatItem
	{
		friend Grabber;
		friend GrabberPImpl;
    public:
		/** constructs an empty VideoFormatItem
		 * the constructed item is invalid (see isValid())
		 * @see isValid()
		 **/
		VideoFormatItem();
		/** copies a VideoFormatItem
		 * @param op the VideoFormatItem to copy
		 **/
		VideoFormatItem( const VideoFormatItem& op );
		/** dtor */
		virtual ~VideoFormatItem();

		/** assignment operator
		 * @param op2 op2
		 */
		VideoFormatItem& operator=( const VideoFormatItem& op2 );

		/** test if instance is a valid format
		 * @return true if <code>this</code> is valid
		 **/
		bool			isValid() const;

		/** get size of format
		 * @return the size of a frame with this format in a SIZE struct
		 **/
		SIZE			getSize() const;

		/** get the bit count per pixel.
		 */
		DWORD			getBitsPerPixel() const;

		/** get color format
		 * @return the GUID of the color format of this format
		 **/
		GUID			getColorformat() const;
		GUID			getSubtype() const;

		/** get string representing the Format
		 * @return a string representation describing the format
		 * @see operator std::string ()
		 **/
		const char*		c_str() const;
	
		/** Creates a textual representation for this VideoFormatItem
		 * @return The textual representation.
		 */
		std::string		toString() const;
		std::wstring	toStringW() const;

		/** get string representing the dimensions of this format
		 * @return a string representing the size of a frame with this format
		 **/
		std::string		getSizeString() const;

		/** get string representing the color format of this format
		 * @return string representing the color format of this format
		 **/
		std::string		getColorformatString() const;
		std::wstring	getColorformatStringW() const;

		bool			operator<( const VideoFormatItem& op ) const;

		/** test if two formats are equal
		 * @param op format to compare to this
		 * @return true if this and op are equal, else false
		 **/
		bool			operator==( const VideoFormatItem& op ) const;

		/** test if two formats are not equal
		 * @param op format to compare to this
		 * @return false if this and op are equal, else true
		 **/
        bool            operator!=( const VideoFormatItem& op ) const;

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
		static VideoFormatItem createInvalid();


		/** get internal VideoFormat. should not be used
		 * @depreceated this function should only be used in the library, but not outside, because you can't do anything with 
		 *		a VideoFormat outside
		 */
		win32_utils::CVideoFormat		getVideoFormat() const;
	private:
		win32_utils::CVideoFormat*			get()		{ return m_pInternalData; }
		const win32_utils::CVideoFormat*	get() const	{ return m_pInternalData; }

		/** constructs a VideoFormatItem from a VideoFormat
		 * @param op2
		 **/
		VideoFormatItem( const win32_utils::CVideoFormat& op2 );

		win32_utils::CVideoFormat*	m_pInternalData;
		
		void			initVideoFormatStringMember();

		std::string		m_VideoFormatString;
	};

};

#endif // !defined(AFX_VIDEOFORMATITEM_H__AD23522D_3281_4115_8943_C22F95EE0957__INCLUDED_)
