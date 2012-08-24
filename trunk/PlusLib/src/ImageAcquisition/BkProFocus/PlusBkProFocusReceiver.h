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
    Other imaging modes are not yet supported.
  */
  virtual void SetImagingMode(vtkBkProFocusVideoSource::ImagingModeType imagingMode);

  /*!
    Set decimation value to reduce the amount of processed data.
    Only every N-th RF sample is acquired.
    Regardelss of the decimation value all the lines are acquired.
  */
  virtual void SetDecimation(int decimation);

  /*! Callback functions called by the BK ProFocus SDK when starting the acquisition */
  virtual bool Prepare(int samples, int lines, int pitch);

  /*! Callback functions called by the BK ProFocus SDK when a frame is acquired */
  virtual bool DataAvailable(int lines, int pitch, void const* frameData);

  /*! Callback functions called by the BK ProFocus SDK when stopping the acquisition */
  virtual bool Cleanup();  

protected:
  
  /*! Video source that is notified about each new frame */
  vtkBkProFocusVideoSource* m_CallbackVideoSource;

  /*! For future use. Now only RF mode is supported. */
  vtkBkProFocusVideoSource::ImagingModeType m_ImagingMode;

  /*! Buffer storing the RF lines that can be converted to brightness lines, after decimation */
  unsigned char* m_Frame;
  
  /*! Paramter to reduce the amount of processed and stored data. The receiver uses only 1 out of m_Decimation samples in a line.*/
  int m_Decimation;

  /*!
    Number of samples (I or Q value) in one RF line in m_Frame.
    One sample is a 16-bit integer.
    The number of RF "sample pairs" = number of RF "samples" / 2.
    The number does not include the RF line header.
  */
  int m_NumberOfRfSamplesPerLine;

  /*! Number RF lines that can be stored in m_Frame */
  int m_MaxNumberOfLines;
};
