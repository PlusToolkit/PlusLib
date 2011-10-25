// MemBuffer.h: interface for the MemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMBUFFER_H__E562743E_3382_4481_8532_C2A2E936FF10__INCLUDED_)
#define AFX_MEMBUFFER_H__E562743E_3382_4481_8532_C2A2E936FF10__INCLUDED_

#pragma once

#include <string>
#include "udshl_defs.h"
#include "smart_ptr.h"

#include "simplectypes.h"
#include "FrameType.h"

namespace _DSHOWLIB_NAMESPACE
{
	struct MemBufferPImpl;
	struct MemBufferCollectionPImpl;
	class MemBufferCollection;
	class Error;

	/** This class saves a pointer to a MemBuffer and some necessary information for this buffer.
	 * The information saved alongside the buffer is the dimension of the frame, the color format and the
	 * size of the buffer. If this buffer is initialized without user mode data pointer, the memory
	 * is automatically allocated on initialization and freed on destruction.
	 *
	 * Remember that Windows images are usually bottom up.
	 **/
	class _UDSHL_EXP_API MemBuffer : public IFrame
	{
		friend MemBufferCollection;
		friend MemBufferCollectionPImpl;
	public:
		typedef smart_ptr< BITMAPINFOHEADER > tBmInfHPtr;

	public:
		/** creates a new MemBuffer by copying an existing one
		 * removed due to inconsistencies in the meaning of this operation
		 * @param op MemBuffer to copy
		 **/
//		MemBuffer( const MemBuffer& op );
		virtual ~MemBuffer();

		/** get internal color format
		 * @return internal color format
		 **/
		tColorformatEnum	getColorformat() const;

		/** get internal frame size
		 * @return internal frame size
		 **/
		const SIZE&			getSize() const;

		/** get size of internal buffer
		 * @return size of internal buffer
		 **/
		DWORD				getBufferSize() const;

		/** get bits per pixel
		 * @return number of bits per pixel for the current color format
		 **/
		int					getBitsPerPixel() const;

		/** get pointer to internal buffer
		 * @return pointer to internal buffer
		 **/
		BYTE*				getPtr() const;

		/** get position in collection
		 * @return position in collection
		 **/
		DWORD				getCollectionPos() const;


		/** get collection of buffer
		 * @return the collection of the buffer
		 **/
		const MemBufferCollection& getCollection() const;

		/** writes buffer to disk in a bmp-file.
		 * <bold>Attention:</bold> existing file are overwritten.
		 * @param filename is a path and filename relative to the current directory.
		 * @return Error that contains the error if on occurred
		 **/
		Error					save( const std::string& filename ) const;
		Error					save( const std::wstring& filename ) const;
		Error					save( const wchar_t* pFilename ) const;

		/** return pointer to BitmapInfoHeader struct according to the buffer
		 * The size of the returned buffer is sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * tBmInfHPtr->biClrUsed
		 * @return smart_ptr< BITMAPINFOHEADER > with BitmapInforHeader to this buffer
		 **/
		tBmInfHPtr				getBitmapInfoHeader() const;

		/** increases the lock count by one */
		void					lock();
		/** decreases the lock count by one */
		void					unlock();
		/** forces a complete unlock of the buffer */
		void					forceUnlock();
		/** returns if the buffer is locked */
		bool					isLocked() const;

		/** returns the WasLockedFlag.
		 * the WasLockedFlag is set by the grabber, when it cannot copy to this buffer, because the buffer is locked.
		 */
		bool					getWasLockedFlag() const;
		/** sets the WasLockedFlag */
		void					setWasLockedFlag( bool f );

		// since 1.4
		/** sets the sample description. (e.g the reference times for the sample)
		 * used internally when a buffer is copied.
		 */
		void						setSampleDesc( const tsMediaSampleDesc& desc );
		/** returns the sample description. */
		const tsMediaSampleDesc&	getSampleDesc() const;

		// since 3.0
		virtual const FrameTypeInfo&		getFrameType() const;
	private:
		/** creates a new MemBuffer with user allocated memory an certain data about the frame
		* properties.
		* @param parent the collection of the buffer
		* @param buffer the pointer to the user allocated memory
		* @param collectionpos position of MemBuffer in MemBufferCollection
		**/
		MemBuffer( const MemBufferCollection& parent, BYTE* buffer,  DWORD collectionpos, const FrameTypeInfo& type );

		/** creates a new MemBuffer and allocate memory
		* @param parent the collection of the buffer
		* @param buffersize size of new allocated memory
		* @param collectionpos position of MemBuffer in MemBufferCollection
		**/
		MemBuffer( const MemBufferCollection& parent, DWORD buffersize, DWORD collectionpos, const FrameTypeInfo& type );
	private:
		MemBufferPImpl* m_pP;
	};

	/** This method saves the IFrame data into a bitmap file.
	 * The format of the bitmap file (BITMAPINFO) is specified by the IFrame. When the IFrame does have an
	 * unknown FrameType, then eINVALID_PARAM_VAL is returned. The IFrame data is not converted to another type,
	 * this means that the bitmap may contain a odd looking image (e.g. UYVY is treated as RGB565).
	 * @param buf		The IFrame to save.
	 * @param filename	The filename of the target file. This file must not exist.
	 * @return When the frame could not be saved, then eINVALID_PARAM_VAL is returned. Otherwise eNOERROR
	 */
	Error	_UDSHL_EXP_API saveToFileBMP( const IFrame& buf, const std::string& filename );
	Error	_UDSHL_EXP_API saveToFileBMP( const IFrame& buf, const std::wstring& filename );
	Error	_UDSHL_EXP_API saveToFileBMP( const IFrame& buf, const wchar_t* pFilename );
	/** This method saves the IFrame data into a bitmap file.
	 * The format of the bitmap file (BITMAPINFO) is specified by the file_fmt parameter.
	 * When you specify another color format as the IFrame FrameType, then the data is converted on the fly to the
	 * desired file format.
	 * @param buf		The IFrame to save.
	 * @param filename	The filename of the target file. This file must not exist.
	 * @param file_fmt	The color format of the target file.
	 * @param bTreatAs	If false, then the IFrame data should be converted to the file_fmt, otherwise
	 *						it should be treated as this frame type (this may lead to odd looking images or even
	 *						bitmaps which do have odd width/height).
	 * @return When the conversion failed or the frame could not be saved, then eINVALID_PARAM_VAL is returned. 
	 *				Otherwise eNOERROR.
	 */
	Error	_UDSHL_EXP_API saveToFileBMP( const IFrame& buf, const std::string& filename, tColorformatEnum file_fmt, bool bTreatAs = false );
	Error	_UDSHL_EXP_API saveToFileBMP( const IFrame& buf, const std::wstring& filename, tColorformatEnum file_fmt, bool bTreatAs = false );
	Error	_UDSHL_EXP_API saveToFileBMP( const IFrame& buf, const wchar_t* pFilename, tColorformatEnum file_fmt, bool bTreatAs = false );
	/** This method saves the IFrame data into a JPEG file.
	 * The IFrame data is internally converted into a format which the JPEG library then converts to a JPEG file.
	 * When the input IFrame data cannot be converted, then the method fails.
	 * @param buf		The IFrame to save.
	 * @param filename	The filename of the target file. This file must not exist.
	 * @param quality	The quality of the created JPEG file. Must be in the range [0,100], with 100 being the highest.
	 * @return When the conversion failed, then eINVALID_PARAM_VAL is returned. Otherwise eNOERROR.
	 */
	Error	_UDSHL_EXP_API saveToFileJPEG( const IFrame& buf, const std::string& filename, int quality = 75 );
	Error	_UDSHL_EXP_API saveToFileJPEG( const IFrame& buf, const std::wstring& filename, int quality = 75 );
	Error	_UDSHL_EXP_API saveToFileJPEG( const IFrame& buf, const wchar_t* pFilename, int quality = 75 );
};
#endif // !defined(AFX_MEMBUFFER_H__E562743E_3382_4481_8532_C2A2E936FF10__INCLUDED_)
