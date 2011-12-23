
#ifndef FRAMETYPE_H_INC_
#define FRAMETYPE_H_INC_

#pragma once

#include <string>
#include "udshl_defs.h"
#include "simplectypes.h"
#include "smart_ptr.h"

namespace _DSHOWLIB_NAMESPACE
{
	struct FrameTypeInfo;

	/** Short SIZE helper class, which provides constructors and helpers. (nothing fancy)
	 */
	struct tSIZE : public SIZE {
		tSIZE()							{}
		tSIZE( const SIZE& op2 )		{ cx = op2.cx; cy = op2.cy; }
		tSIZE( long x, long y )	{ cx = x; cy = y; }

		bool	operator==( const SIZE& op2 ) const	{ return cx == op2.cx && cy == op2.cy; }
		bool	operator!=( const SIZE& op2 ) const	{ return cx != op2.cx || cy != op2.cy; }

		tSIZE&	operator+=( const SIZE& op2 )	{ cx += op2.cx; cy += op2.cy; return *this; }
		tSIZE&	operator-=( const SIZE& op2 )	{ cx += op2.cx; cy += op2.cy; return *this; }
	};

	inline tSIZE	operator+( const SIZE& op1, const SIZE& op2 )	{ return tSIZE( op1.cx + op2.cx, op1.cy + op2.cy ); }
	inline tSIZE	operator-( const SIZE& op1, const SIZE& op2 )	{ return tSIZE( op1.cx - op2.cx, op1.cy - op2.cy ); }

	inline unsigned long	area( const SIZE& dim )
	{
		return abs(dim.cx * dim.cy);
	}

	/** An object implementing the IFrame interface permits access to its image data and the
	 * frame type of the image.
	 *
	 * The pointer returned from getPtr may only be valid in the scope of the function in which get a reference
	 * to the IFrame passed in.
	 */
	class IFrame
	{
	public:
		/** Returns the frame type of this frame. */
		virtual const FrameTypeInfo&		getFrameType() const = 0;

		/** Get pointer to the frame data.
		 * @return pointer to the frame buffer. This may be 0.
		 **/
		virtual BYTE*						getPtr() const = 0;

		/** Returns the sample description. */
		virtual const tsMediaSampleDesc&	getSampleDesc() const = 0;
	};

	/** This structure represents a frame type as used in the library to specify the type and dimensions of one frame or
	 * of one data stream.
	 *
	 * This structure may have 3 distinct meanings :
	 *	1) Fully defined type. When subtype != MEDIASUBTYPE_NULL, dim != 0 and buffersize != 0.
	 *	2) Partially defined type. When subtype != MEDIASUBTYPE_NULL, dim == 0 and buffersize == 0.
	 *	3) Generic type. When subtype == MEDIASUBTYPE_NULL.
	 *
	 * These distinct meanings matter in case of type matching. A full defined type only matches another
	 *	full qualified type. A partial type matches any partial and any fully defined type with the same subtype.
	 *	The generic type matches all other frame types.
	 *
	 * buffersize is a special member of the frame type, in that it contains a derived value from the subtype
	 *	and dimension. There are cases when the buffersize of a frame type may be higher then needed, when
	 *	the user needs the larger buffer.
	 */
	struct FrameTypeInfo {
		FrameTypeInfo();
		FrameTypeInfo( tColorformatEnum fmt, unsigned int width = 0, int height = 0, unsigned int buf_size = 0 );
		FrameTypeInfo( tColorformatEnum fmt, const SIZE& dim, unsigned int buf_size = 0 );
		FrameTypeInfo( const GUID& subtype, unsigned int width = 0, int height = 0, unsigned int buf_size = 0 );
		FrameTypeInfo( const GUID& subtype, const SIZE& dim, unsigned int buf_size = 0 );
		FrameTypeInfo( unsigned int buf_size );

		/** Returns the equivalent color format of this frame type (if possible). */
		tColorformatEnum	getColorformat()	const;
		/** Returns the bits per pixel this subtype has. May return 0 when the subtype is unknown. */
		unsigned int		getBitsPerPixel()	const;
		/** Returns if this frame type is a known bottom up type, like RGB32, RGB24 ... */
		bool				isBottomUpFrameType() const;


		/** The media subtype of this frame. May contain any subtype (even self-defined). */
		GUID			subtype;
		/** The width and height of the frame when applicable. 0 when not defined or should match on all frame dimensions. */
		tSIZE			dim;
		/** Contains the size of the data buffers of this frame type. This may be 0 when no size is enforced. */
		unsigned int	buffersize;

		bool	operator==( const FrameTypeInfo& op2 ) const;
		bool	operator!=( const FrameTypeInfo& op2 ) const;
	};

	/** Simple container around an array of FrameTypeInfo objects. This is used to ease adding frame types to the
	 * array.
	 * Deriving from std::vector<> is here not bad, because only non-virtual methods are added and no members or virtual
	 * methods are used.
	 */
	class FrameTypeInfoArray : public std::vector<FrameTypeInfo>
	{
	public:
		FrameTypeInfoArray()										{}
		explicit FrameTypeInfoArray( tColorformatEnum fmt )			{ addFrameType( fmt ); }
		explicit FrameTypeInfoArray( const FrameTypeInfo& info )	{ addFrameType( info ); }

		/** Sets the accepted type to a native color format. */
		void	addFrameType( tColorformatEnum fmt, unsigned int width = 0, int height = 0 );
		void	addFrameType( const GUID& subtype, unsigned int width = 0, int height = 0 );
		void	addFrameType( const FrameTypeInfo& info );
		void	addFrameType( const FrameTypeInfo* pInfoList, unsigned int count = 1 );

		/** These functions create a FrameTypeInfoArray which contains the specified standard types. */
		static FrameTypeInfoArray	createRGBArray();
		static FrameTypeInfoArray	createStandardTypesArray();
		static FrameTypeInfoArray	createOptimalTypesArray();
	};

	/** Returns the number of allocated bits per pixel. This means that for eRGB555, 16 bits is returned.
	 * @return Number of bits per pixel for the given format if known. Otherwise 0.
	 **/
	_UDSHL_EXP_API int				getBitsPerPixel( tColorformatEnum colorformat );
	_UDSHL_EXP_API DWORD			getBitsPerPixel( const GUID& subtype );

	/** Returns the according subtype for the specified colorformat. */
	_UDSHL_EXP_API GUID				getSubtypeFromColorformat( tColorformatEnum colorformat );
	/** Returns the according colorformat for the passed in subtype. */
	_UDSHL_EXP_API tColorformatEnum	getColorformatFromSubtype( const GUID& subtype );

	/** Returns the required byte count for the specified colorformat and dimension. */
	_UDSHL_EXP_API DWORD			calcBufferSize( const SIZE& dim, tColorformatEnum colorformat );

	/** Returns the required byte count for the specified frame type. */
	_UDSHL_EXP_API DWORD						calcRequiredBuffersize( const FrameTypeInfo& type );
	/** Creates a BITMAPINFOHEADER for the specified frame type, if possible. */
	_UDSHL_EXP_API smart_ptr<BITMAPINFOHEADER>	createBitmapInfoHeader( const FrameTypeInfo& type );

	/** Creates a string representation of the FrameTypeInfo object passed. */
	_UDSHL_EXP_API std::wstring		convertToStringW( const FrameTypeInfo& type );
	_UDSHL_EXP_API std::string		convertToStringA( const FrameTypeInfo& type );
	inline std::string				convertToString( const FrameTypeInfo& type )
	{
		return convertToStringA( type );
	}
	/** Creates a string representation of the media subtype passed.
	 * There are 3 types of strings the function can create.
	 *	1) The according name of the media subtype. (e.g. RGB32, RGB8 ...)
	 *	2) The FourCC of the media subtype if no string could be found, and the subtype denotes
	 *		a FourCC media subtype (e.g. UYVY, YUY2, ... )
	 *	3) The string representation of the guid itself (e.g. "847F57E1-6843-4CE7-9E32-D9979B0B1A74")
	 */
	_UDSHL_EXP_API std::wstring		convertSubtypeToStringW( const GUID& type );
	/** Parses the string and tries to extract a subtype from it. May return GUID_NULL when
	 * no subtype could be created from the string.
	 */
	_UDSHL_EXP_API GUID				convertStringToSubtype( const std::wstring& type );
	/** Parses the string and tries to extract a FrameTypeInfo from it. May return a empty FrameTypeInfo
	 * when the string could not be parsed.
	 * 
	 * The layout must be "<subtype> (<dim.cx>x<dim.cy>) <buffersize>". Whereby the dimension and the
	 *	buffersize may be optional.
	 */
	_UDSHL_EXP_API FrameTypeInfo	convertToFrameTypeInfo( const std::wstring& type );

	/** Returns if this frame type is a known bottom up type, like RGB32, RGB24 ... */
	_UDSHL_EXP_API bool				isBottomUpFrameType( tColorformatEnum colorformat );
	_UDSHL_EXP_API bool				isBottomUpFrameType( const GUID& subtype );

	/** Creates a FourCC subtype from a FourCC string representation.
	 * e.g. "Y800" -> "30303859-0000-0010-8000-00AA00389b71"
	 */
	_UDSHL_EXP_API GUID				createFCCMediaSubType( const wchar_t* pFCC );
	_UDSHL_EXP_API GUID				createFCCMediaSubType( const char* pFCC );

	/** A partially defined type is one, which only has its subtype set and all other members to 0.
	 * (buffersize may contain a byte count which is taken into account if it is larger then the needed size).
	 * @param type
	 * @return true/false
	 */
	bool	isPartiallyDefined( const FrameTypeInfo& type );
	
	/** Returns if the in frame type is compatible with the out frame type.
	 * This function takes partially defined types into account, so that a partial defined frame type is compatible
	 * with a complete frame type when the not defined part is initialized with the placeholder (0 or GUID_NULL).
	 *
	 * The order of the parameters is necessary, so that larger output buffer sizes can be allowed.
	 * @param inFt	The input frame type.
	 * @param outFt	The output frame type.
	 * @return true when both types are compatible.
	 */
	bool	isPartiallyEqual( const FrameTypeInfo& inFt, const FrameTypeInfo& outFt );
	

	//////////////////////////////////////////////////////////////////////////
	// inline function implementations follow
	//////////////////////////////////////////////////////////////////////////
	

	inline FrameTypeInfo::FrameTypeInfo()	: subtype( MEDIASUBTYPE_NULL ), dim( 0, 0 ), buffersize( 0 ) {}
	inline FrameTypeInfo::FrameTypeInfo( tColorformatEnum fmt, unsigned int width, int height, unsigned int buf_size )
		: subtype( getSubtypeFromColorformat( fmt ) ), dim( width, height ), buffersize( buf_size )	{}
	inline FrameTypeInfo::FrameTypeInfo( tColorformatEnum fmt, const SIZE& dim, unsigned int buf_size )
		: subtype( getSubtypeFromColorformat( fmt ) ), dim( dim ), buffersize( buf_size )	{}
	inline FrameTypeInfo::FrameTypeInfo( const GUID& subtype, unsigned int width, int height, unsigned int buf_size )
		: subtype( subtype ), dim( width, height ), buffersize( buf_size )	{}
	inline FrameTypeInfo::FrameTypeInfo( const GUID& subtype, const SIZE& dim, unsigned int buf_size )
		: subtype( subtype ), dim( dim ), buffersize( buf_size )	{}
	inline FrameTypeInfo::FrameTypeInfo( unsigned int buf_size )
		: subtype( MEDIASUBTYPE_NULL ), dim( 0, 0 ), buffersize( buf_size )	{}

	inline tColorformatEnum	FrameTypeInfo::getColorformat()	const
	{ return getColorformatFromSubtype( subtype ); }
	inline unsigned int		FrameTypeInfo::getBitsPerPixel()	const
	{ return _DSHOWLIB_NAMESPACE::getBitsPerPixel( subtype ); }
	inline bool				FrameTypeInfo::isBottomUpFrameType() const
	{ return _DSHOWLIB_NAMESPACE::isBottomUpFrameType( subtype ); }


	inline bool	FrameTypeInfo::operator==( const FrameTypeInfo& op2 ) const
	{
		return subtype == op2.subtype && dim == op2.dim && buffersize == op2.buffersize;
	}
	inline bool	FrameTypeInfo::operator!=( const FrameTypeInfo& op2 ) const
	{
		return subtype != op2.subtype || dim != op2.dim || buffersize != op2.buffersize;
	}

	inline bool	isPartiallyEqual( const FrameTypeInfo& inFt, const FrameTypeInfo& outFt )
	{
		// MEDIASUBTYPE_NULL matches always
		if( inFt.subtype == MEDIASUBTYPE_NULL || outFt.subtype == MEDIASUBTYPE_NULL )	return true;

		// the subtypes must always match, when neither is equal to MEDIASUBTYPE_NULL
		if( inFt.subtype != outFt.subtype )		return false;

		// either one of the buffer sizes is 0 (then both sizes always match) or the
		// output buffersize must always be equal to or larger than the input size (only because of 
		// this the order of the function arguments must be ensured)
		if( inFt.buffersize != 0 && outFt.buffersize != 0 && inFt.buffersize > outFt.buffersize )
			return false;

		// either one of the dimensions is 0 (this means all elements of dim are 0) or
		// the frame types cx and cy must be equal
		if( inFt.dim != tSIZE( 0, 0 ) && outFt.dim != tSIZE( 0, 0 ) && inFt.dim != outFt.dim )
			return false;

		return true;
	}

	inline bool	isPartiallyDefined( const FrameTypeInfo& type )
	{
		if( type.subtype == GUID_NULL )					return false;

		if( type.dim.cx == 0 && type.dim.cy == 0 )		return true;

		return false;
	}


	inline void	FrameTypeInfoArray::addFrameType( tColorformatEnum fmt, unsigned int width, int height )
	{
		FrameTypeInfo tmp( fmt, width, height );
		addFrameType( tmp );
	}
	inline void	FrameTypeInfoArray::addFrameType( const GUID& subtype, unsigned int width, int height )
	{
		FrameTypeInfo tmp( subtype, width, height );
		addFrameType( tmp );
	}
	inline void	FrameTypeInfoArray::addFrameType( const FrameTypeInfo& info )
	{
		push_back( info );
	}

	inline void	FrameTypeInfoArray::addFrameType( const FrameTypeInfo* pInfoList, unsigned int count )
	{
		insert( end(), pInfoList, pInfoList + count );
	}

	inline FrameTypeInfoArray	FrameTypeInfoArray::createRGBArray()
	{
		FrameTypeInfoArray rval;
		rval.addFrameType( eRGB32 );
		rval.addFrameType( eRGB24 );
		rval.addFrameType( eRGB565 );
		rval.addFrameType( eRGB555 );
		rval.addFrameType( eRGB8 );
		return rval;
	}
	inline FrameTypeInfoArray	FrameTypeInfoArray::createStandardTypesArray()
	{
		FrameTypeInfoArray rval;
		rval.addFrameType( eRGB32 );
		rval.addFrameType( eRGB24 );
		rval.addFrameType( eRGB565 );
		rval.addFrameType( eRGB555 );
		rval.addFrameType( eRGB8 );
		rval.addFrameType( eY800 );
		return rval;
	}
	inline FrameTypeInfoArray	FrameTypeInfoArray::createOptimalTypesArray()
	{
		FrameTypeInfoArray rval;
		rval.addFrameType( eRGB32 );		// when a transformation is needed, then this is the fastest type.
		rval.addFrameType( eRGB24 );		// sometimes native, so maybe no conversion needed.
		rval.addFrameType( eY800 );			// better then eRGB8, because it does not need to be flipped.
		return rval;
	}


};

#endif // FRAMETYPE_H_INC_
