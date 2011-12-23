// MemBufferCollection.h: interface for the MemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMBUFFERCOLLECTION_H__9DAE8989_42EC_4A63_8328_AD5829AD2D11__INCLUDED_)
#define AFX_MEMBUFFERCOLLECTION_H__9DAE8989_42EC_4A63_8328_AD5829AD2D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "udshl_defs.h"
#include "simplectypes.h"
#include "smart_ptr.h"

#include "Error.h"
#include "FrameType.h"

namespace _DSHOWLIB_NAMESPACE
{
	class MemBuffer;
	class MemBufferCollection;

	typedef smart_ptr<MemBufferCollection> tMemBufferCollectionPtr;

	struct MemBufferCollectionPImpl;

	/** this class implements a memory buffer handling for image buffers.
	 * it can collect multiple buffers of the same type, i. e. with the same size and colorformat.
	 * The class provides an init() method to initialize the internal buffers. Any Errors can be
	 * queried by getLastError(). MemBufferCollection can handle user allocated buffers or allocates
	 * memory for you.
	 **/
	class _UDSHL_EXP_API MemBufferCollection
	{
	public:
		virtual ~MemBufferCollection();

		typedef smart_ptr<MemBuffer> tMemBufferPtr;
		typedef smart_ptr<BITMAPINFOHEADER> tBmInfHPtr;

		/** get frame Size
		 * @return size of each Frame in the buffer
		 **/
		const SIZE&					getFrameSize() const;

		/** get colorformat
		 * @return colorformat for that the buffer was initialized.
		 * If the buffer is uninitialized, the return value is undefined.
		 **/
		tColorformatEnum			getColorformat() const;

		/** get bit per pixel
		 * @return number of bits per pixel for the current colorformat
		 **/
		int							getBitsPerPixel() const;

		/** get buffer length
		 * @return length of the buffer in frames
		 **/
		DWORD						getBufferCount() const; // in frames

		/** get a MemBuffer with a certain index
		 * @param pos frame number (starting from 0)
		 * @return the queried MemBuffer
		 **/
		tMemBufferPtr				operator [] ( DWORD pos ) const;

		/** get a MemBuffer with a certain index
		 * @param pos frame number (starting from 0)
		 * @return the queried MemBuffer
		 **/
		tMemBufferPtr				getBuffer( DWORD pos ) const;

		/** get the size of one frame buffer
		 * @return the size of one frame buffer in bytes
		 **/
		DWORD						getBufferSize() const; // in bytes

		/** test whether the buffer is initialized. Deprecated, the collection is always valid.
		 * @return true, if it is initialized
		 **/
		bool						isInit() const;

		/// @return the last occurred error
		Error						getLastError() const;


		/** fills given buffer with a certain pattern.
		 * @param buffno number of buffer to fill with pattern
		 * @return true on success
		 **/
		bool						fillWithPattern( DWORD buffno, tPatternEnum pattern );

		/** fills every buffer with a certain pattern.
		 * @return true on success
		 **/
		bool						fillWithPattern( tPatternEnum pattern );

		/** writes buffers to disk in a bmp-file for each buffer.
		 * <bold>Attention:</bold> existing file are overwritten.
		 * @param filename is a path and filename relative to the current directory. The name must
		 * contain one '*' character which will be replaced by the collection number of the
		 * buffers.
		 * @return true on success, else false
		 **/
		bool						save( const std::string& filename ) const;
		bool						save( const std::wstring& filename ) const;

		/** return pointer to BitmapInfoHeader struct according to the buffers. 
		 * The size of the returned buffer is sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * tBmInfHPtr->biClrUsed
		 * @return smart_ptr< BITMAPINFOHEADER > with BitmapInfoHeader to this collection
		 **/
		tBmInfHPtr					getBitmapInfoHeader() const;


		// since 2.0
		/** get the size of the valid data in the buffer itself.
		 * User allocated buffer may be larger than the actual data which is defined through
		 * (getFrameSize().cx * getFrameSize().cy * getBitsPerPixel()) / 8.
		 */
		DWORD						getBufferDataSize() const; // in bytes

		/** internal method, do not use.
		 */
		GUID						getMediaSubtype() const;

		// since 3.0
		/* Returns the frame type of the collection. */
		const FrameTypeInfo&		getFrameType() const;

		/** Creates a MemBufferCollection for use with the FrameHandlerSink or the Grabber.
		 * @param frame_type	Must contain a fully specified frame type (no partial frame type allowed).
		 * @param count			The number of buffers the collection should contain. Must be > 0.
		 * @return 0 when one of the parameters is not valid, otherwise a MemBufferCollection instance.
		 */
		static tMemBufferCollectionPtr		create( const FrameTypeInfo& frame_type, DWORD count );

		/** Creates a MemBufferCollection for use with the FrameHandlerSink or the Grabber.
		 * @param frame_type	Must contain a fully specified frame type (no partial frame type allowed).
		 * @param count			The number of buffers the collection should contain. Must be > 0.
		 * @param buffers		The user specified image data pointers. These will be used by the MemBuffers for the
		 *							image data. The array must contain <count> pointers, which must not be 0.
		 * @return 0 when one of the parameters is not valid, otherwise a MemBufferCollection instance.
		 */
		static tMemBufferCollectionPtr		create( const FrameTypeInfo& frame_type, DWORD count, BYTE* buffers[] );

		/** Creates a new MemBufferCollection from  a colorformat and image dimensions.
		 * @param colorformat	The colorformat of the MemBuffers. Must be a valid colorformat.
		 * @param dim			The dimensions of the MemBuffers created. Must be > 0
		 * @param count			The number of buffers the collection should contain. Must be > 0.
		 * @return 0 when one of the parameters is not valid, otherwise a MemBufferCollection instance.
		 **/
		static tMemBufferCollectionPtr		create( tColorformatEnum colorformat, SIZE dim, DWORD count );

		/** Creates a new MemBufferCollection from a colorformat and image dimensions.
		 * The MemBuffers in the collection get the pointers you specified as image data pointers.
		 * @param colorformat	The colorformat of the MemBuffers. Must be a valid colorformat.
		 * @param dim			The dimensions of the MemBuffers created. Must be > 0
		 * @param count			The number of buffers the collection should contain. Must be > 0.
		 * @param buffers		The user specified image data pointers. These will be used by the MemBuffers for the
		 *							image data. The array must contain <count> pointers, which must not be 0.
		 * @return 0 when one of the parameters is not valid, otherwise a MemBufferCollection instance.
		 **/
		static tMemBufferCollectionPtr		create( tColorformatEnum colorformat, SIZE dim, DWORD count, BYTE* buffers[] );
	protected:
		MemBufferCollection( const FrameTypeInfo& type, DWORD buffercount, BYTE* buffers[] );
	private:
		/** Copying of MemBufferCollection objects is prohibited. */
		MemBufferCollection( const MemBufferCollection& op );
		void	operator=( const MemBufferCollection& op2 );

		MemBufferCollectionPImpl*	m_pP;
	};
};

#endif // !defined(AFX_MEMBUFFERCOLLECTION_H__9DAE8989_42EC_4A63_8328_AD5829AD2D11__INCLUDED_)
