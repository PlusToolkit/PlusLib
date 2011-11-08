// VideoNormItem.h: interface for the VideoNormItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEONORMITEM_H__978E5F24_E299_4CA7_BFD2_F5700DC0444D__INCLUDED_)
#define AFX_VIDEONORMITEM_H__978E5F24_E299_4CA7_BFD2_F5700DC0444D__INCLUDED_

#pragma once

#include <string>
#include "udshl_defs.h"

namespace _DSHOWLIB_NAMESPACE
{
	class Grabber;

	/** Class representing a video norm.
	 * All members will deliver valid values when isValid() returns true
	 **/
	class _UDSHL_EXP_API VideoNormItem
	{
		friend Grabber;
    private:
		VideoNormItem( AnalogVideoStandard t );
	public:
        /// constructs an invalid VideoNormItem();
		VideoNormItem();
        /// copies an VideoNormItem();
		VideoNormItem( const VideoNormItem& op );
		virtual ~VideoNormItem();

		/** get the saved video norm
		 * @return saved video norm
		 **/
		AnalogVideoStandard	getVideoNorm()	const { return m_VideoNorm; }


		/** get a string representation of the saved video norm
		 * @return a pointer to the string representation of the saved video norm
		 **/
						operator const char* () const;


		/** get a string representation of the saved video norm
		 * @return a pointer to the string representation of the saved video norm
		 **/
		const char*		c_str() const;

		/** Creates a textual representation for this VideoNormItem
		 * @return The textual representation.
		 */
		std::string		toString() const;
		std::wstring	toStringW() const;

		/** assignment operator
		 * @return *this
		 */
		VideoNormItem&	operator=( const VideoNormItem& op2 );

		/** returns the result of a lexicographical compare of the names
		 * @return true, if this is before op
		 * @param op the VideoNormItem to compare with
		 **/
		bool operator < (const VideoNormItem& op) const;

        /** test if <code>this</code> is valid
         * @return true if <code>this</code> is valid
         **/
        bool            isValid() const;

		/** test if two norms are equal
		 * @param op norm to compare to this
		 * @return true if this and op are equal, else false
		 **/
		bool			operator==( const VideoNormItem& op ) const;

		/** test if two norms are not equal
		 * @param op norm to compare to this
		 * @return false if this and op are equal, else true
		 **/
		bool			operator!=( const VideoNormItem& op ) const;

		/** test if two items are equal
		 * @param op item to compare to this
		 * @return true if this and op are equal, else false
		 **/
		bool			operator==( const std::string& op ) const;
		bool			operator==( const std::wstring& op ) const;

		/** generates an invalid item
		 * @return an invalid item
		 * @see isValid()
		 **/
		static VideoNormItem createInvalid();
	private:
		void			initVideoNormString();

		AnalogVideoStandard	m_VideoNorm;
		char*				m_pC;
	};
};

#endif // !defined(AFX_VIDEONORMITEM_H__978E5F24_E299_4CA7_BFD2_F5700DC0444D__INCLUDED_)
