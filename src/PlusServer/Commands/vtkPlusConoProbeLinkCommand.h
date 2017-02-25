/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
Date: Ag 2015
Authors include:
- Mikael Brudfors [*] brudfors@hggm.es
- Javier Pascau [*][ç] jpascau@hggm.es
[*] Laboratorio de Imagen Medica, Hospital Gregorio Maranon - http://image.hggm.es/
[ç] Departamento de Bioingeniería e Ingeniería Aeroespacial. Universidad Carlos III de Madrid
=========================================================================*/

#ifndef __vtkPlusConoProbeLinkCommand_h
#define __vtkPlusConoProbeLinkCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkPlusOptimetConoProbeMeasurer;

/*!
  \class vtkPlusConoProbeLinkCommand
  \brief This command allows for communicating with the OptimetConoProbe device.
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusConoProbeLinkCommand : public vtkPlusCommand
{
public:
  static vtkPlusConoProbeLinkCommand* New();
  vtkTypeMacro(vtkPlusConoProbeLinkCommand, vtkPlusCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string>& cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const std::string& commandName);

  vtkGetStdStringMacro(ConoProbeDeviceId);
  vtkSetStdStringMacro(ConoProbeDeviceId);

  void SetNameToShow();

  /*!
    Helper function to get pointer to the ConoProbe device
    \param conoProbeDeviceId ConoProbe device ID..
  */
  vtkPlusOptimetConoProbeMeasurer* GetConoProbeDevice(const std::string& conoProbeDeviceID);

protected:
  vtkPlusConoProbeLinkCommand();
  virtual ~vtkPlusConoProbeLinkCommand();

protected:
  std::string ConoProbeDeviceId;

private:
  vtkPlusConoProbeLinkCommand(const vtkPlusConoProbeLinkCommand&);
  void operator=(const vtkPlusConoProbeLinkCommand&);

};

#endif