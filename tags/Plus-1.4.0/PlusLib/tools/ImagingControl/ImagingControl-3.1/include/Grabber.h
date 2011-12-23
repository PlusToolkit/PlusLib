// Grabber.h: interface for the Grabber class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRABBER_H__F72DCE0B_3C5C_44EA_BFAB_ADDE04304CA6__INCLUDED_)
#define AFX_GRABBER_H__F72DCE0B_3C5C_44EA_BFAB_ADDE04304CA6__INCLUDED_

#pragma once

#include <string>
#include <vector>

#include "udshl_defs.h"
#include "smart_ptr.h"

#include "simplectypes.h"

#include "AnalogChannelItem.h"
#include "VideoNormItem.h"
#include "VideoFormatItem.h"
#include "VideoCaptureDeviceItem.h"
#include "Error.h"
#include "IVCDProperty.h"
#include "DeBayerTransform.h"
#include "FrameFilterBase.h"

namespace _DSHOWLIB_NAMESPACE
{
	class Grabber;
	class MemBufferCollection;
	class MemBuffer;
	class GrabberListener;
	class GrabberSinkType;
	class FilterInfoObject;
	class OverlayBitmap;

	class IDispEventDispatcher;

	/** This function must be used to initialize the library.
	 * @param pSerialNumber		the serial number you want to set
	 * @param coinitmode		mode passed to CoInitializeEx.
	 *				Pass -1 when you already called OleInitialize or CoInitialize/Ex or you bind 
	 *					statically to common dialogs like the file open dialog.
	 *				Pass COINIT_APARTMENTTHREADED if you want to load a common dialog dynamically or when
	 *					some OLE object fails after InitLibrary.
	 * @return true on success, false otherwise.
	 */
	_UDSHL_EXP_API bool	InitLibrary( const char* pSerialNumber, DWORD coinitmode = 0 );
	/** This function must be used to initialize the library.
	 * @param ppSerialNumber	the serial numbers you want to set.
	 * @param count				the number of serial numbers in ppSerialNumber
	 * @param coinitmode		mode passed to CoInitializeEx.
	 *				Pass -1 when you already called OleInitialize or CoInitialize/Ex or you bind 
	 *					statically to common dialogs like the file open dialog.
	 *				Pass COINIT_APARTMENTTHREADED if you want to load a common dialog dynamically or when
	 *					some OLE object fails after InitLibrary.
	 * @return true on success, false otherwise.
	 */
	_UDSHL_EXP_API bool	InitLibrary( const char** ppSerialNumber, unsigned int count, DWORD coinitmode = 0 );

	/** This method returns true when all objects returned by the 
	 * the library were destroyed, otherwise false. 
	 * You can use this to determine the best time to unload the library (e.g. for COM objects).
	 */
	_UDSHL_EXP_API bool	CanUnloadLibrary();

	/** This method should be called by the client app, when the library should shut down.
	 * If you don't do this, then the library may seem to leak several objects and bytes, because the dump running
	 * objects call may happen before the library has been told to unload itself.
	 *
	 * After calling ExitLibrary no operation on the library is valid and so its effect is undefined.
	 */
	_UDSHL_EXP_API void	ExitLibrary();

	/// struct that defines a frame end callback
	struct _UDSHL_EXP_API tsFrameEndCallback
	{
		typedef smart_ptr<MemBuffer>	tMemBufferPtr;

		/** Callback function for user defined EndOfFrame Callback
		 * @param ptr pointer set by setCallback()
		 * @param buffer pointer to actual frame buffer
		 * @param frameCount number of current frame (continuous until UINT_MAX)
		 **/
		typedef void (*tFrameEndCallbackFuncPtr)( void* ptr, tMemBufferPtr pBuffer, DWORD frameCount );

		/// default constructor initializes pointers with 0
		tsFrameEndCallback();
		/// constructor to give initial values
		tsFrameEndCallback( tFrameEndCallbackFuncPtr func, void* data );

		tFrameEndCallbackFuncPtr	pFunc;		///< callback function pointer
		void*						pData;		///< pointer to data for callback function
	};

	/// a range type for all the properties in Grabber
	struct tsPropertyRange
	{
		long min;
		long max;
	};

	class GrabberPImpl;

	/** This class provides the interface to the device.
	 *
	 * Most of the methods return a boolean value. If false or 0 is returned by a method an Error 
	 * may have occurred. You can check this with getLastError(), which returns the last occurred error.
	 *
	 * @see Error
	 **/
	class _UDSHL_EXP_API Grabber
	{
	public:
		Grabber();
		virtual ~Grabber();

		/// Video  properties.
		typedef VideoProcAmpProperty					tVideoPropertyEnum;
		/// Camera properties.
		typedef CameraControlProperty					tCameraPropertyEnum;

        /// VideoCaptureDevice (i. e. device)
        typedef VideoCaptureDeviceItem					tVideoCaptureDeviceItem;
        typedef std::vector<tVideoCaptureDeviceItem>	tVidCapDevList;
		/// VideoCaptureDeviceList
		typedef smart_ptr<tVidCapDevList>				tVidCapDevListPtr;

		/// input channel for device
		typedef AnalogChannelItem						tInputChannelItem;
		typedef std::vector<tInputChannelItem>			tInChnList;
		/// list of input channels
		typedef smart_ptr<tInChnList>					tInChnListPtr;

        /// VideoNorm for device
        typedef VideoNormItem							tVideoNormItem;
		typedef std::vector<tVideoNormItem>				tVidNrmList;
		/// VideoNormList
		typedef smart_ptr<tVidNrmList>					tVidNrmListPtr;

        /// VideoFormat for device
        typedef VideoFormatItem							tVideoFormatItem;
		typedef std::vector<tVideoFormatItem>			tVidFmtList;
		/// VideoFormatList
		typedef smart_ptr<tVidFmtList>					tVidFmtListPtr;

		/// Memory buffer smart pointer
		typedef smart_ptr<MemBufferCollection>			tMemBufferCollectionPtr;
		/// Memory buffer smart pointer
		typedef smart_ptr<MemBuffer>					tMemBufferPtr;

		/// compressor item
		typedef FilterInfoObject						tCompressorItems;
		typedef std::vector<FilterInfoObject>			tCompressorList;
		/// list of all available compressors
		typedef smart_ptr<tCompressorList>				tCompressorListPtr;

		/// list of frame rates
		typedef std::vector<long>						tFrameRateList;
		typedef smart_ptr<tFrameRateList>				tFrameRateListPtr;

		typedef std::vector<double>						tFPSList;
		typedef smart_ptr<tFPSList>						tFPSListPtr;

		/** returns the last error.
		 * when the last call into the library failed, returns the according error structure, else return eNOERROR.
		 * @return the last occurred error
		 */
		Error				getLastError() const;


		/** get all available video capture devices in a vector.
		 * @return all available video capture devices in a vector. This may be empty.
         *	On an error 0 is returned.
		 **/
		tVidCapDevListPtr	getAvailableVideoCaptureDevices() const;

		/** get all available compressors
		 * @return all available video compressors in a vector. This may be empty.
         *	On an error 0 is returned.
		 */
		tCompressorListPtr	getAvailableVideoCompressors() const;

		/** get all available video formats for the current device and current VideoNorm.
		 * @return all available video formats in a vector for the current VideoNorm. This may be empty.
         *	On an error 0 is returned.
		 **/
		tVidFmtListPtr		getAvailableVideoFormats() const;

		/** get all available video formats for the current device and a given VideoNorm.
		 * @return all available VideoFormats for the given VideoNorm in a vector. This may be empty.
         *	On an error 0 is returned.
		 * @param videonorm videonorm for which the VideoFormats are returned
		 **/
		tVidFmtListPtr		getAvailableVideoFormats( const VideoNormItem& videonorm ) const;

		/** test if the device supports video norms
		 * @return true if the device supports video norms
		 **/
		bool				isVideoNormAvailableWithCurDev() const;

		/** get all supported VideoNorms supported by the device
		 * @return all available VideoNorms in a vector. This may be empty.
         *	On an error 0 is returned.
		 **/
		tVidNrmListPtr		getAvailableVideoNorms() const;

		/** test if the device supports multiple input channels
		 * @return true if the device supports multiple input channels
		 **/
		bool				isInputChannelAvailableWithCurDev() const;

		/** get all supported input channels
		 * @return all available input channels in a vector. This may be empty.
         *	On an error 0 is returned.
		 **/
		tInChnListPtr		getAvailableInputChannels() const;

        /** get current input channel
         * @param channel input channel to set
         * @return true on success, else false
         **/
        tInputChannelItem   getInputChannel() const;

        /** set new input channel
         * @param channel new input channel
         * @return true on success, else false
         **/
        bool                setInputChannel( const tInputChannelItem& );

        /** set new input channel
         * @param channel new input channel
         * @return true on success, else false
         **/
        bool                setInputChannel( DWORD channel );

		/** Opens a video capture device.
		 * @return true on success
		 **/
		bool                openDev( const VideoCaptureDeviceItem& dev );

		/** Opens a video capture device.
		 * @return true on success
		 **/
		bool                openDev( const char* dev );

		/** gets an item for the currently opened video device
		 * @return the current VideoCaptureDeviceItem, on error an invalid item is returned
		 **/
		tVideoCaptureDeviceItem	getDev() const;

		/** Closes the current opened video capture device.
		* @return true on success
		**/
		bool                closeDev();


		/** Test if a device is open
		* @return true, if dev is opened
		**/
		bool                isDevOpen() const;

		/** Test if the device is valid (not unplugged)
		 * @return true, if dev is valid, else false if no device is opened or current device is invalid
		 **/
		bool				isDevValid() const;

		/** get maximum image size
		* @return maximum image size (width)
		**/
		long                getAcqSizeMaxX() const;

		/** get maximum image size
		* @return maximum image size (height)
		**/
		long                getAcqSizeMaxY() const;

		/** set the format of the sink.
		 * this function invalidates the active MemBufferCollection, when you pass a FrameGrabberSink 
		 * with another colorformat than the colorformat of the MemBufferCollection into it.
		 * if you want to hold a reference to the sink you should call setSinkType( const smart_ptr<GrabberSinkType>& pNewSink )
		 * @param newSinkType the new sink type
		 * @return true on success

		 * removed
		 */
//		bool					setSinkType( const GrabberSinkType& newSinkType );

		/** returns the current sink type.
		 * will never fail
		 * @return the current sink type
		 */
		const GrabberSinkType&	getSinkType() const;

		/** returns the current sink type.
		 * will never fail
		 * @return the current sink type
		 */
		GrabberSinkType&		getSinkType();

		/** get range of certain property (e. g. hue, saturation, gamma, focus, ... ).
		 * @param type property to query
		 * @return range of setting (min,max)
		 **/
		tsPropertyRange     getPropertyRange( tCameraPropertyEnum type ) const;

		/** get range of certain property (e. g. hue, saturation, gamma, focus, ... ).
		 * @param type property to query
		 * @return range of setting (min,max)
		 **/
		tsPropertyRange     getPropertyRange( tVideoPropertyEnum type ) const;

        /** get the default value of a certain property
         * @param type property to query
         * @return default value for property
         **/
        long                getPropertyDefault( tCameraPropertyEnum type ) const;

        /** get the default value of a certain property
         * @param type property to query
         * @return default value for property
         **/
        long                getPropertyDefault( tVideoPropertyEnum type ) const;

		/** set properties automation state (e. g. hue, saturation, gamma, focus, ... ).
		 * @param type property to set
		 * @param autom true to set, false to unset automation
		 * @return true on success
		 **/
		bool                setProperty( tCameraPropertyEnum type, bool autom );

		/** set properties automation state (e. g. hue, saturation, gamma, focus, ... ).
		 * @param type property to set
		 * @param autom true to set, false to unset automation
		 * @return true on success
		 **/
		bool                setProperty( tVideoPropertyEnum type, bool autom );

		/** set properties (e. g. hue, saturation, gamma, focus, ... ).
		 * @param type property to set
		 * @return true on success
		 **/
		bool                setProperty( tCameraPropertyEnum type, long val );

		/** set properties (e. g. hue, saturation, gamma, focus, ... ).
		 * @param type property to set
		 * @param val value to set
		 * @return true on success
		 **/
		bool                setProperty( tVideoPropertyEnum type, long val );

		/** get properties (e. g. hue, saturation, gamma, focus, ... ).
         * @param type property to return
		 * @param val value to set
    	 * @return value of given property
		 **/
		long                getProperty( tCameraPropertyEnum type ) const;

		/** get properties (e. g. hue, saturation, gamma, focus, ... ).
         * @param type property to return
    	 * @return value of given property
		 **/
		long                getProperty( tVideoPropertyEnum type ) const;

		/** query automation state of properties (e. g. hue, saturation, gamma, focus, ... ).
         * @param type property to query
    	 * @return true, if automation is enabled for given property, else false
		 **/
		bool                isPropertyAutomationEnabled( tCameraPropertyEnum type ) const;

		/** query automation state of properties (e. g. hue, saturation, gamma, focus, ... ).
         * @param type property to query
    	 * @return true, if automation is enabled for given property, else false
		 **/
		bool                isPropertyAutomationEnabled( tVideoPropertyEnum type ) const;

        /** query availability of certain property
         * @param type property to query
         * @return true on availability of type, else false
         **/
        bool                isPropertyAvailableWithCurDev( tCameraPropertyEnum type ) const;

        /** query availability of certain property
         * @param type property to query
         * @return true on availability of type, else false
         **/
        bool                isPropertyAvailableWithCurDev( tVideoPropertyEnum type ) const;

        /** query availability of automation for certain property
         * @param type property to query
         * @return true on availability of automation for type, else false
         **/
        bool                isPropertyAutomationAvailableWithCurDev( tCameraPropertyEnum type ) const;

        /** query availability of automation for certain property
         * @param type property to query
         * @return true on availability of automation for type, else false
         **/
        bool                isPropertyAutomationAvailableWithCurDev( tVideoPropertyEnum type ) const;

		/** creates a new MemBufferCollection
		 * @param size size of the frames
		 * @param colorformat colorformat to use for the frames
		 * @param count the buffer count in the collection
		 * @return true on success
		 **/
		tMemBufferCollectionPtr		newMemBufferCollection( SIZE size, tColorformatEnum colorformat, DWORD count = 1 ) const;

		/** creates a new MemBufferCollection. The parameters are taken from
		 * the current Grabber (an error occurs if current Grabber is
		 * invalid).
		 * @param grabber a Grabber reference to get the size and colorformat
		 * @param count the buffer count in the collection
		 * @return true on success
		 **/
		tMemBufferCollectionPtr		newMemBufferCollection( DWORD count = 1 ) const;

		/** creates a new MemBufferCollection
		 * @param size size of the frames
		 * @param colorformat colorformat to use for the frames
		 * @param buffersize size of one buffer
		 * @param buffers Array with buffer pointers to user allocated memory of size <code>buffersize</code>
		 * @param count dimension of <code>buffers</code>, i. e. the length of the collection
		 **/
		tMemBufferCollectionPtr		newMemBufferCollection( SIZE size, tColorformatEnum colorformat, DWORD buffersize, BYTE* buffers[], DWORD count ) const;

		/** creates a new MemBufferCollection. The parameters are taken from
		 * the current Grabber (an error occurs if current Grabber is
		 * invalid).
		 * @param buffersize size of one buffer
		 * @param buffers Array with buffer pointers to user allocated memory
		 * of size <code>buffersize</code>
		 * @param count dimension of <code>buffers</code>, i. e. the length of
		 * the collection
		 **/
		tMemBufferCollectionPtr		newMemBufferCollection( DWORD buffersize, BYTE* buffers[], DWORD count ) const ;


		/** set active memory buffer for grabbing.
         * This method invalidates all the buffers set with pushBackUserMemBuffer().
 		 * @param pBuffer buffer to use for grabbing or 0 to reset internal setting
		 * @return true on success
		 **/
		bool                setActiveMemBufferCollection( tMemBufferCollectionPtr pBuffer );

		/** get active memory buffer for grabbing
		 * @return Buffer used for grabbing
		 **/
		tMemBufferCollectionPtr		getActiveMemBufferCollection() const;

		/** get active MemBuffer. A pointer to a buffer containing the last acquired image is returned
		 * @return pointer to actual buffer or 0 if no buffer is active
		 **/
		tMemBufferPtr				getActiveMemBuffer() const;

        /** get currently necessary size of a UserMemBuffer
         * @return the size of a frame in bytes according to the current colorformat and region of interest
         **/
        DWORD						getUserMemBufferSize() const; // in bytes

		/** Start live mode.
		 * This operation may take relatively long and can fail due to many different reasons. So check the error
		 * value after the call of this method.
		 * @param show_videowindow	true to enable the live video window, false to grab only.
		 * @return true on success
		 **/
		bool                startLive( bool show_videowindow = true );

		/** stop live mode
		 * @return true on success
		 **/
		bool				stopLive();

		/** test of live mode
		 * @return true if live mode is on
		 **/
		bool				isLive() const;

		/** snaps some images in the currently active buffer at the currently active position.
		 * This function will only work, if live mode is active.
         * @param count the number of frames to acquire (default: 1)
		 * @param timeout if 0xFFFFFFFF then the function may block indefinitely, otherwise the function waits for timeout
		 *			milliseconds. When the timeout is reached, the system may snap the remaining frames if the system does commence.
		 *			this parameter is only intended to reduce hangups when no samples are delivered by the device
   		 * @return true on success otherwise false
 		 **/
		bool                snapImages( DWORD count = 1, DWORD timeout = 0xFFFFFFFF );

		/** get current videonorm
		 * @return current videonorm
		 **/
		VideoNormItem		getVideoNorm() const;

		/** set current video norm
		 * @param videonorm videonorm to set as current
		 * @return true on success, else false
		 **/
		bool				setVideoNorm( const VideoNormItem& videonorm );

		/** get current videoformat (for acquisition)
		 * @return current videoformat
		 **/
		VideoFormatItem		getVideoFormat() const;

		/** set current videoformat (for acquisition)
		 * @param videoformat videoformat to set
		 * @return true on success, else false
		 **/
		bool				setVideoFormat( const VideoFormatItem& videoformat );

		/** set Window for live grabbing
		 * @param hwnd window handle for the window to use
		 * @return true on success
		 **/
		bool                setHWND( HWND hwnd );


		/** get current HWND for live grabbing
		 * @return null if no handle is set, else the handle
		 **/
		HWND                getHWND() const;

		/** set callback function pointer
		 * @param callback structure with callback information (default is reset)
		 * @return true on success, else false
		 **/
		bool				setCallback( const tsFrameEndCallback& callback = tsFrameEndCallback() );

		/** get callback function pointer
		 * @return callback function pointer or 0 on error
		 **/
		tsFrameEndCallback getCallback() const;

		/** get current frame count
		 * @return content of internal frame counter
		 **/
		DWORD				getFrameCount() const;

		/** flip image horizontal.
		 * @param flip true flip, false do not flip
		 * @return true on success
		 **/
		bool				setFlipH( bool flip = true );

		/** get current horizontal flip state
		 * @return current flip state or false if any error occurs
		 * @see setFlip()
		 **/
		bool				getFlipH() const;

		/** returns if flip horizontal is available.
		 * @return current flip state or false if any error occurs
		 * @see setFlip()
		 **/
		bool				isFlipHAvailable() const;

		/** returns true if the current video capture device has an external transport control
		 */
		bool				hasExternalTransport() const;

		/** this function sets the Mode of the ExternalTranport Module if present
		 * for an overview of all available modes see IAMExtTransport::put_Mode
		 */
		bool				setExternalTransportMode( long );
		/** retrieve the current mode of the External Transport Device */
		long				getExternalTransportMode() const;

		/** returns the frame rates available for this videoformat.
		 * the rates describe the time from the start of one frame to the next in milliseconds 
		 */
		tFrameRateListPtr	getAvailableFrameRates( const VideoFormatItem& op ) const;
		/** returns the frame rates available for the current videoformat.
		 * the rates describe the time from the start of one frame to the next in milliseconds 
		 */
		tFrameRateListPtr	getAvailableFrameRates() const;
		/** retrieve the actual frame rate achieved by the device.
		 * the rates describe the time from the start of one frame to the next in milliseconds<br>
		 * only available in live mode
		 */
		long				getCurrentActualFrameRate() const;
		/** sets the frame rate to achieve by the device.
		 * the rates describe the time from the start of one frame to the next in milliseconds<br>
		 * only available outside live mode
		 */
		bool				setFrameRate( long rate );
		/** gets the frame rate which is currently set.
		 * the rates describe the time from the start of one frame to the next in milliseconds<br>
		 */
		long				getFrameRate() const;

		/** returns if the current device has and may use an external trigger.
		 * <strong>this may return false information for devices, that implement internal trigger support</strong>
		 */
		bool				hasExternalTrigger() const;
		/** sets the External Trigger on/off.
		 * only available outside live mode
		 * @param m when true the external trigger is enabled, else disabled.
		 */
		bool				setExternalTrigger( bool m );
		/** returns the current setting for the External Trigger.
		 * @return true if it is enabled, else false
		 */
		bool				getExternalTrigger() const;

		// new in 1.4
		///////

		/** flip image vertical.
		 * @param flip true flip, false do not flip
		 * @return true on success
		 **/
		bool				setFlipV( bool flip = true );

		/** get current vertical flip state
		 * @return current flip state or false if any error occurs
		 * @see setFlip()
		 **/
		bool				getFlipV() const;

		/** returns if flip vertical is available.
		 * @return current flip state or false if any error occurs
		 * @see setFlip()
		 **/
		bool				isFlipVAvailable() const;

		/** sets the position of the video in the live window.
		 * the settings are invalidated by a call to setVideoFormat when getDefaultWindowPosition() == true, otherwise
		 * the settings will not be invalidated by any call to the grabber object.
		 * calling this when the graph is running directly influences the live display.
		 * @param x0 the offset left from the window origin
		 * @param y0 the offset down from the window origin
		 * @return true on success otherwise getLastError() returns the error value.
		 */
		bool				setWindowPosition( long x0, long y0  );
		/** sets the  size of the video in the live window.
		 * the settings are invalidated by a call to setVideoFormat when getDefaultWindowPosition() == true, otherwise
		 * the settings will not be invalidated by any call to the grabber object.
		 * calling this when the graph is running directly influences the live display.
		 * @param width the width of the video display in the window. Must be >= 0, otherwise an error is returned.
		 * @param height the height of the video display in the window. Must be >= 0, otherwise an error is returned.
		 * @return true on success otherwise getLastError() returns the error value.
		 */
		bool				setWindowSize( long width, long height );
		/** resets the live window position and size to the defaults (x0 = 0, y0 = 0, w = getAcqSizeMaxX(), h = getAcqSizeMaxY() )
		 */
		bool				setWindowPosition();
		/** fills the parameters with the current values. */
		bool				getWindowPosition( long& x0, long& y0, long& w, long& h ) const;

		/** sets if the window position is reset by a call to VideoFormat.
		 * @param b if b == true then a change of the VideoFormat will change the dimensions */
		bool				setDefaultWindowPosition( bool b );
		/** returns if DefaultWindowPosition is set. */
		bool				getDefaultWindowPosition() const;

		/**	attaches a new listener object.
		 * @param pListener pointer to the object derived from GrabberListener used as message sink.
		 *			the object pointed to by pListener must live at least as long as the object is registered.
		 * @param reg the callbacks for which the object will be registered. This can be any combination of the flags 
		 *			defined by GrabberListener::tListenerType.
		 *			-1 is interpreted as GrabberListener::eALL.
		 * @return true on success, otherwise false
		 */
		bool				addListener( GrabberListener* pListener, DWORD reg = -1 );

		/** removes a listener object for all of the registration values you passed in.
		 * When an entry (pListener,reg) does not exist, the method does nothing for that entry.
		 * When a CB is currently running for one entry removeListener returns false and removing for that entry 
		 * is postponed until the CB returns.
		 *  (You can check if the listener was removed by calling isListenerRegistered).
		 * If no CB is running on the listener object, the entry is removed and the method returns true.
		 * @param pListener pointer to the object derived from GrabberListener which was previously registered.
		 * @param reg the methods may be called by the Grabber object. The flags are defined by GrabberListener::tListenerType.
		 *			-1 is interpreted as GrabberListener::eALL.
		 * @return True when all entries were immediately removed from the list.
		 *			False when one or more entries could not be immediately removed
		 */
		bool				removeListener( GrabberListener* pListener, DWORD reg = -1 );

		/** returns if the listener object is registered for any of the passed registration values.
		 * @param pListener the listener to check
		 * @param reg the type for the listener is tested to be registered.
		 * @return true if the listener is registered under any of the types passed in by reg, else false
		 */
		bool				isListenerRegistered( GrabberListener* pListener, DWORD reg = -1 );

		/** returns an overlay surfaces which is rendered into the stream */
		smart_ptr<OverlayBitmap>	getOverlay() const;

		/** set the format of the sink.
		 * this function invalidates the active MemBufferCollection, when you pass a FrameGrabberSink 
		 * with another colorformat than the colorformat of the MemBufferCollection into it.
		 * @param pNewSink the new sink type
		 * @return true on success
		 */
		bool				setSinkType( const smart_ptr<GrabberSinkType>& pNewSink );

		/** returns the current reference time in 100 nanoseconds (or 1/10000 milliseconds).
		 * the value depends on the ReferenceClock used in the graph (but is mostly equivalent to QueryPerformanceCounter).
		 * the method fails when no reference clock is available.
		 * @param Current gets the value of the current reference time.
		 * @return true on success, otherwise false and an error value is set.
		 */
		bool				getCurReferenceTime( REFERENCE_TIME& Current ) const;
		/** returns the reference time used when the graph was started in 100 nanoseconds (or 1/10000 milliseconds).
		 * the value depends on the ReferenceClock used in the graph (but is mostly equivalent to QueryPerformanceCounter).
		 * the method fails when no reference clock is available.
		 * @param GraphStart gets the value of the reference start time of the graph.
		 * @return true on success, otherwise false and an error value is set.
		 */
		bool				getGraphStartReferenceTime( REFERENCE_TIME& GraphStart ) const;

		///////////////////////////////////////////////
		// new in version 2.0
		///////////////////////////

		smart_com<IVCDPropertyItems>	getAvailableVCDProperties() const;

		/** Returns if a device supports querying it, if an video stream is available for the device.
		 * @return 
		 */
		bool				isSignalDetectedAvailable() const;
		/** returns if a signal was detected by the device. This flag is only thought for devices 
		 * which get a signal from an external device, e.g. a converter which gets its video
		 * from a analog camera.
		 * @return true the driver of the device said that a signal was detected. false when 
		 * either the driver said no signal was detected or when an error occurred 
		 * (e.g. option not available).
		 */
		bool				getSignalDetected() const;

		/** Returns if the device supports a list of frame rates, which the user can set.
		 * @return	true/false
		 */
		bool				isFrameRateListAvailable() const;
		bool				isFrameRateListAvailable( const VideoFormatItem& op ) const;

		/** Returns the frame rates available for this videoformat.
		 * the rates describe the time from the start of one frame to the next in milliseconds 
		 */
		tFPSListPtr			getAvailableFPS( const VideoFormatItem& op ) const;
		/** Returns the frame rates available for the current videoformat.
		 * the rates describe the time from the start of one frame to the next in milliseconds 
		 */
		tFPSListPtr			getAvailableFPS() const;
		/** Retrieve the actual frame rate achieved by the device.
		 * the rates describe the time from the start of one frame to the next in milliseconds<br>
		 * only available in live mode
		 */
		double				getCurrentActualFPS() const;

		/** Returns the current maximal fps the device can achieve on the bus.
		 * This may be different from the maximal fps in the fps list, due to several devices which run 
		 * on the same bus and have to share the bandwidth.
		 * If you set a higher frame rate than the one returned by this function, the call to startLive may
		 * fail because the device cannot be started at the rate. This leads to the error "Failed to connect
		 * the pins."
		 * @return The FPS. This may be 0 when the device does not support this property or cannot supply it
		 *			in the current mode of operation.
		 *			(some devices need to be started to retrieve this setting (this is a bit odd in some drivers ;-) )
		 */
		double				getCurrentMaxAvailableFPS() const;

		/** sets the frame rate to achieve by the device.
		 * only available outside live mode
		 */
		bool				setFPS( double fps );
		/** gets the frame rate which is currently set.
		 */
		double				getFPS() const;

		/** Returns if the device supports retrieving the current dropped frames counter.
		 * @return true/false
		 */
		bool				isCountOfFramesDroppedAvailable() const;
		/**	Returns the number of frames dropped by the VideoCaptureDevice.
		 * @return The number of frames dropped by the VideoCaptureDevice.
		 *			May be == 0 when the device does not export this property.
		 */
		long				getCountOfFramesDropped() const;

		/** Returns the number of frames the VideoCaptureDevice send, which were not dropped by the device.
		 * This doesn't mean these frames were delivered, because some frames can be dropped along the way.
		 * @return The number of frames not dropped by the VideoCaptureDevice.
		 *			May be == 0 when the device does not export this property.
		 */
		long				getCountOfFramesNotDropped() const;

		/** Sets the pause mode.
		 * In live mode the video is immediately paused. Outside live mode the mode is saved and will be set
		 * when a startLive is called.
		 * @return true on success, otherwise false.
		 */
		bool				setPauseLive( bool on );
		/** Returns if the pause mode is enabled.
		 * @return true when the pause mode is enabled.
		 */
		bool				getPauseLive() const;

		/** Shows the property page for this device.
		 * \param title		The title to show.
		 * \param hParent	The parent window of this property page. If this is 0, then the active window is used
		 *						because the page needs to be modal!!
		 * \return true on success, otherwise false.
		 */
		bool				showVCDPropertyPage( HWND hParent = 0, const std::string& title = "" );
		bool				showVCDPropertyPage( HWND hParent, const std::wstring& title );

		/** Shows a device settings page to choose a device and several other options for this device.
		 * \param hParent	The parent of this modal page. If this is 0, then the active window is used
		 *						because the page needs to be modal!!
		 * \param excludeDevices	A vector containing names of the devices that are not displayed in the
		 *					device selection combo box. This is useful to hide devices that are already opened by
		 *					the calling application.
		 * \return true when the user clicked OK to exit the page, false if he clicked CANCEL or an error occurred.
		 */
		bool				showDevicePage( HWND hParent = 0 );
		bool				showDevicePage( HWND hParent, const std::vector<std::string>& excludeDevices );
		bool				showDevicePage( HWND hParent, const std::vector<std::wstring>& excludeDevices );

		bool				openDev( const std::string& devstring );
		bool				openDev( const std::wstring& devstring );
		bool				openDev( const __int64& serial );
		bool				setVideoFormat( const std::string& videoformatstring );
		bool				setVideoFormat( const std::wstring& videoformatstring );
		bool				setVideoNorm( const std::string& videonormstring );
		bool				setVideoNorm( const std::wstring& videonormstring );
        bool                setInputChannel( const std::string& inputchannel );
		bool                setInputChannel( const std::wstring& inputchannel );

		bool				openDevByUniqueName( const std::string& unique_name );
		bool				openDevByUniqueName( const std::wstring& unique_name );
		bool				openDevByDisplayName( const std::string& display_name );
		bool				openDevByDisplayName( const std::wstring& display_name );
		
		/** Saves the grabber settings to a xml string. Following settings are saved :
		 *	- Opened device (and serial)
		 *	- Video norm
		 *	- Video Format
		 *	- Input Channel
		 *	- FPS
		 *	- FlipH
		 *	- FlipV
		 *	- VCR compatibility
		 *	- VCD Properties
		 */
		std::string		saveDeviceState() const;
		std::wstring	saveDeviceStateW() const;

		/** Restores a saved grabber state */
		bool			loadDeviceState( const std::string& xmlStr, bool bOpenDev = true );
		bool			loadDeviceState( const std::wstring& xmlStr, bool bOpenDev = true );

		/** Saves the grabber state to a file */
		bool			saveDeviceStateToFile( const std::string& filename ) const;
		bool			saveDeviceStateToFile( const std::wstring& filename ) const;

		/** Loads the grabber state from a file */
		bool			loadDeviceStateFromFile( const std::string& filename, bool bOpenDev = true );
		bool			loadDeviceStateFromFile( const std::wstring& filename, bool bOpenDev = true );


		///////////////////////////////////////////////
		// new in version 3.0
		///////////////////////////

		/** sets the position of the OverlayBitmap in the graph.
		 * You can set the position before calling startLive, when the graph is running you cannot change the position.
		 * The Graph position is saved as long as the grabber object instance lives and is not invalidated by closing
		 * the current device.
		 * @see tPathPosition for the available positions the overlay bitmap can take in the graph.
		 * @param OVBPathPositions An or'ed value which contains all path positions which should be enabled.
		 * @return true on success, otherwise false and getLastError returns an error description.
		 */
		bool			setOverlayBitmapPathPosition( DWORD OVBPathPositions );
		/** returns the current position of the OverlayBitmap in the graph.
		 * @see tPathPosition for the available positions the overlay bitmap can take in the graph.
		 * @return true on success, otherwise false and getLastError returns an error description.
		 */
		DWORD	getOverlayBitmapPathPosition() const;

		/** Returns a pointer to the current sink.
		 * @return 0 when no sink is set.
		 */
		smart_ptr<GrabberSinkType>	getSinkTypePtr() const;

		/** This methods allow fine grained control over what is saved to the string/file.
		 * @param bDevice				If the device should be saved
		 * @param bGrabberDeviceSetup	If the following device settings should be saved:
		 *		<ul>
		 *			<li>Video norm</li>
		 *			<li>Video format</li>
		 *			<li>Input Channel</li>
		 *			<li>FPS</li>
		 *			<li>FlipH/V</li>
		 *			<li>VCRCompatibility</li>
		 *		</ul>
		 * @param bVCDProperties		If the VCDProperties should be saved
		 * @return the string.
		 */
		std::string		saveDeviceState( bool bDevice, bool bGrabberDeviceSetup = true, bool bVCDProperties = true ) const;
		std::wstring	saveDeviceStateW( bool bDevice, bool bGrabberDeviceSetup = true, bool bVCDProperties = true ) const;
		bool			saveDeviceStateToFile( const std::string& filename, bool bDevice, bool bGrabberDeviceSetup = true, bool bVCDProperties = true ) const;
		bool			saveDeviceStateToFile( const std::wstring& filename,bool bDevice, bool bGrabberDeviceSetup = true, bool bVCDProperties = true ) const;

		/** Sets one device frame filter for the grabber. You can only set filters when the graph is stopped.
		 * These new frame filters will be placed right behind the VideoCaptureDevice.
		 *
		 * Any previous installed frame filters are removed.
		 * If you pass in 0, no new frame filter will be installed.
		 * @param pFrameFilter The new frame filter you want to use. This must not be deleted
		 *				while it is set in the grabber.
		 * @return true on success, false otherwise.
		 */
		bool					setDeviceFrameFilters( IFrameFilter* pFrameFilter );
		/** Sets one or more device frame filters for the grabber. You can only set filters when the graph is stopped.
		 * These new frame filters will be placed right behind the VideoCaptureDevice.
		 *
		 * Any previous installed frame filters are removed.
		 * If you pass an empty list, no new frame filter will be installed.
		 * @param lst The new frame filters you want to use. These must not be deleted
		 *				while they are set in the grabber.
		 * @return true on success, false otherwise.
		 */
		bool					setDeviceFrameFilters( const tFrameFilterList& lst );
		/** Returns a list of frame filters set by setDeviceFrameFilters.
		 * @return Either the list of currently set frame filters, or an empty list when no frame filters are set.
		 */
		const tFrameFilterList	getDeviceFrameFilters() const;

		/** Returns the actual dimension of the data which is passed to the video window.
		 * This may be different from the VideoFormat, when you use DeviceFrameFilters/DisplayFrameFilters and
		 * it may be different from the FrameHandlerSink dimensions.
		 *
		 * You can only retrieve this information, when the graph is already built.
		 * @param dim	Will be filled with the dimension of the video data stream, which arrives at the VideoRenderer.
		 * @return true when dim could be filled with the actual dimensions, otherwise false.
		 */
		bool			getVideoDataDimension( SIZE& dim ) const;

		/** Tries to create the graph and then disconnects the VideoCaptureDevice.
		 * You can use this to save time when building the graph.
		 *
		 * No operation which may change the graph layout is permitted after prepareLive is called.
		 *
		 *	So following operations are not permitted :
		 *		<ul>
		 *			<li>setVideoFormat</li>
		 *			<li>setFrameRate/setFPS</li>
		 *			<li>setVideoNorm</li>
		 *			<li>setSinkType</li>
		 *			<li>setOverlayBitmapPathPosition</li>
		 *			<li>setTrigger</li>
		 *			<li>setFlipH/V</li>
		 *			<li>...</li>
		 *		</ul>
		 *
		 *	The methods stopLive and closeDev end this mode.
		 *
		 * @param	bUseVideoRenderer	If a VideoRenderer should be used. @see Grabber::startLive.
		 * @return	true on success, otherwise false.
		 */
		bool			prepareLive( bool bUseVideoRenderer );
		/** Returns if the graph is prepared, but not yet started.
		 * @return true/false.
		 */
		bool			isLivePrepared() const;
		/** Suspends a running graph.
		 * A running graph so gets prepared.
		 *
		 * When the operation fails, the graph is set to the stopped state.
		 * @return true on success, otherwise false.
		 */
		bool			suspendLive();

		/** Sets one display frame filter for the grabber. You can only set filters when the graph is stopped.
		 * These new frame filters will be placed right in front of the VideoRenderer.
		 *
		 * Any previous installed frame filters are removed.
		 * If you pass in 0, no new frame filter will be installed.
		 * @param pFrameFilter The new frame filter you want to use. This must not be deleted
		 *				while it is set in the grabber.
		 * @return true on success, false otherwise.
		 */
		bool					setDisplayFrameFilters( IFrameFilter* pCB );
		/** Sets one or more device frame filters for the grabber. You can only set filters when the graph is stopped.
		 * These new frame filters will be placed right in front of the VideoRenderer.
		 *
		 * Any previous installed frame filters are removed.
		 * If you pass an empty list, no new frame filter will be installed.
		 * @param lst The new frame filters you want to use. These must not be deleted
		 *				while they are set in the grabber.
		 * @return true on success, false otherwise.
		 */
		bool					setDisplayFrameFilters( const tFrameFilterList& lst );
		/** Returns a list of frame filters set by setDeviceFrameFilters.
		 * @return Either the list of currently set frame filters, or an empty list when no frame filters are set.
		 */
		const tFrameFilterList	getDisplayFrameFilters() const;

		/** Returns the control interface for the DeBayerTransform filter, which may be used in
		 * the grabber to implicitly debayer a stream.
		 * @return should never be 0,
		 */
		smart_ptr<DeBayerTransform>	getDeBayerTransform() const;

		/** This method fetches the according OVB from the internal list of usable OVB objects.
		 * @param PathPositionToFetch The position of the OVB to fetch.
		 * @return 0 if an invalid index was passed, otherwise the according object.
		 */
		smart_ptr<OverlayBitmap>	getOverlay( tPathPosition PathPositionToFetch ) const;


		/** Alters the behavior of the setProperty/setPropertyAutomation/... etc. functions.
		 * Do not use when you create a new Application.
		 */
		void	setOldPropertyBehavior( bool bBehavior );
		bool	getOldPropertyBehavior() const;

		/** Retrieves the list of interfaces, retrieved from the graph.
		 * This should be used with caution, as not to hold things, you shouldn't keep open.
		 *
		 *	Beware, that this method does not set the last error var.
		 * \param itf_guid	The IID of the interfaces to retrieve.
		 * \param vec		List, where the interfaces will be added to.
		 * \return false, when no graph is build, etc.
		 */
		typedef std::vector<smart_com<IUnknown> > tGraphItfList;
		bool	getGraphInterfaceList( const GUID& itf_guid, tGraphItfList& vec );

		/** Internal interface. Do not use.
		 */
		//IDispEventDispatcher&	getEventDispatcher();
	private:
		GrabberPImpl*		m_pP;
	};
}

#endif // !defined(AFX_GRABBER_H__F72DCE0B_3C5C_44EA_BFAB_ADDE04304CA6__INCLUDED_)
