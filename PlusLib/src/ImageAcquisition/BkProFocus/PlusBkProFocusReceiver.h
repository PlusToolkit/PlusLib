// BK library includes
#include "IAcquisitionDataReceiver.h"
#include "libbmode.h"

#include "vtkBkProFocusVideoSource.h"

/*!
\class PlusBkProFocusReceiver 
\brief Class for receiving images through the BK ProFocus SDK (Grabbie)
\ingroup PlusLibImageAcquisition
*/
class PlusBkProFocusReceiver : public IAcquisitionDataReceiver
{
public:

  PlusBkProFocusReceiver();
  virtual ~PlusBkProFocusReceiver();

  /*! Set the video source that will be notified when a new frame is received */
  virtual void SetPlusVideoSource(vtkBkProFocusVideoSource *videoSource);

  /*!
    Set what kind of images are sent to the video source. The default is RF mode,
    when the raw RF data is recorded. Applications (e.g., fCal) may still be able
    to show a B-mode image for monitoring the acquisition by performing the
    brightness conversion and scan conversion internally.
    If B mode is selected then the receiver performs brightness conversion using
    the algorithm implemented in the BK ProFocus SDK, however no scan conversion
    is performed. Therefore, B mode acquisition is intended for experimental or
    testing use only.
  */
  virtual void SetImagingMode(vtkBkProFocusVideoSource::ImagingModeType imagingMode);

  /*! Callback functions called by the BK ProFocus SDK when starting the acquisition */
  virtual bool Prepare(int samples, int lines, int pitch);

  /*! Callback functions called by the BK ProFocus SDK when a frame is acquired */
  virtual bool DataAvailable(int lines, int pitch, void const* frameData);

  /*! Callback functions called by the BK ProFocus SDK when stopping the acquisition */
  virtual bool Cleanup();  

protected:
  vtkBkProFocusVideoSource* CallbackVideoSource;

  vtkBkProFocusVideoSource::ImagingModeType ImagingMode;

  /*! Parameters used by the BK ProFocus SDK RF to B mode converter */
  TBModeParams m_BModeConvertParams;

  /*! Buffer storing the RF lines that can be converted to brightness lines */
  unsigned char* m_Frame;
  
  /*! Buffer storing the brightness converted lines */
  unsigned char* m_BModeFrame;

  /*!
    Buffer storing the full RF lines, without decimation
    TODO: this has to be cleaned up, because it's essentially the same as the m_Frame (without decimation).
     Probably it's enough to save the RF data with the specified decimation.
  */
  unsigned char* m_RfFrame;

  /*! Paramter to reduce the amount of processed and stored data. The receiver uses only 1 out of m_Decimation samples in a line.*/
  int m_Decimation;

  /*! Number of RF samples in one RF line. TODO: clarify what does one sample mean (an IQ pair is 1 sample; or an IQ pair are 2 samples). */
  int m_NumberOfRfSamples;
};
