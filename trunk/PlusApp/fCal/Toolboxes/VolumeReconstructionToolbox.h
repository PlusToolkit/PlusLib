/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef VOLUMERECONSTRUCTIONTOOLBOX_H
#define VOLUMERECONSTRUCTIONTOOLBOX_H

#include "ui_VolumeReconstructionToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkVolumeReconstructor;
class vtkImageData;

//-----------------------------------------------------------------------------

/*! \class VolumeReconstructionToolbox 
 * \brief Volume reconstruction toolbox view class
 * \ingroup PlusAppFCal
 */
class VolumeReconstructionToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  VolumeReconstructionToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~VolumeReconstructionToolbox();

  /*! Initialize toolbox (load session data) - overridden method */
  void Initialize();

  /*!
  * Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
  */
  void RefreshContent();

  /*!
  * Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

  /*!
  * Add image file name to the list (usually when one is saved in Capturing toolbox)
  * \param aImageFileName Path and filename of the image
  */
  void AddImageFileName(QString aImageFileName);

protected:
  /*!
  * Reconstructs volume from input file
  * \return Success flag
  */
  PlusStatus ReconstructVolumeFromInputImage();

  /*!
  * Saves volume to file
  * \param aOutput Output file
  * \return Success flag
  */
  PlusStatus SaveVolumeToFile(QString aOutput);

  /*! Display reconstructed volume in canvas */
  void DisplayReconstructedVolume();

  /*!
  * Populate image combobox from the image file name list and the unsaved data in Capturing toolbox if present
  */
  void PopulateImageComboBox();

protected slots:
  /*! Slot handling open volume reconstruction config button click */
  void OpenVolumeReconstructionConfig();

  /*! Called when input image selection has been changed */
  void InputImageChanged(int aItemIndex);

  /*! Slot handling open input image button click */
  void OpenInputImage();

  /*! Slot handling open reconstruct button click  */
  void Reconstruct();

  /*! Slot handling open save button click */
  void Save();

  /*! Recompute the surface that is shown from the reconstructed volume when slider is moved */
  void RecomputeContourFromReconstructedVolume(int aValue);

protected:
  /*! Volume reconstructor instance */
  vtkVolumeReconstructor*  m_VolumeReconstructor;

  /*! Reconstructed volume */
  vtkImageData*            m_ReconstructedVolume;

  /*! Flag indicating whether a volume reconstruction config file has been loaded successfully */
  bool                    m_VolumeReconstructionConfigFileLoaded;

  /*! Contouring threshold */
  double                  m_ContouringThreshold;

  /*! String list containing the file names of the loaded images and the images that have been saved by Capturing toolbox */
  QStringList             m_ImageFileNames;

  /*! String to hold the last location of data saved */
  QString                 m_LastSaveLocation;

protected:
  Ui::VolumeReconstructionToolbox ui;
};

#endif
