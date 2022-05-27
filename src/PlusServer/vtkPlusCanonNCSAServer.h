/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __VTKPLUSCANONNCSASERVER_H
#define __VTKPLUSCANONNCSASERVER_H

// Local includes
#include "vtkPlusServerExport.h"
#include "vtkPlusDataCollector.h"
#include "vtkIGSIOTransformRepository.h"

// VTK includes
#include <vtkMultiThreader.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STL includes
#include <deque>

// OS includes
#if (_MSC_VER == 1500)
  #include <stdint.h>
#endif

// Canon NCSA includes
#include <NcsaCommLibrary.h>
#include <Logger.h>
#include <cxxopts.hpp>
#include "Canon/ConfigurationCommon.h"

class vtkPlusDataCollector;
class vtkPlusChannel;
class vtkPlusCommandProcessor;
class vtkPlusCommandResponse;
class vtkIGSIORecursiveCriticalSection;

/*!
  \class vtkPlusCanonNCSAServer
  \brief This class provides a network interface for data acquired by Plus to be broadcasted onto the Canon NCSA platform

  \ingroup PlusLibPlusServer
*/
class vtkPlusServerExport vtkPlusCanonNCSAServer : public vtkObject, ncsa::NcsaListener
{
public:
  static vtkPlusCanonNCSAServer* New();
  vtkTypeMacro(vtkPlusCanonNCSAServer, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Configures and starts the server from the provided PlusOpenIGTLinkServer XML element */
  PlusStatus Start(vtkPlusDataCollector* dataCollector, vtkIGSIOTransformRepository* transformRepository, vtkXMLDataElement* serverElement, const std::string& configFilePath, int argc, char** argv);

  /*! Configures and starts the server from the provided device set configuration file */
  PlusStatus Stop();


  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* serverElement, const std::string& aFilename);

  /*! Set data collector instance */
  vtkSetMacro(DataCollector, vtkPlusDataCollector*);
  vtkGetMacroConst(DataCollector, vtkPlusDataCollector*);

  /*! Set transform repository instance */
  vtkSetMacro(TransformRepository, vtkIGSIOTransformRepository*);
  vtkGetMacroConst(TransformRepository, vtkIGSIOTransformRepository*);

  /*! Start server */
  PlusStatus StartService();

  /*! Stop server */
  PlusStatus StopService();

  vtkGetStdStringMacro(ConfigFilename);

  // ncsa::NcsaListener methods
  /**
     * @brief Listen for operator updates.  For simplicity, this is a no-op.
     *
     * @param [in] op Reference to an Operator that has changed.
     */
  void onUpdate(const ncsa::Operator& /*op*/) override {}

  /**
   * @brief Listen for operator deleted updates.  For simplicity, this is a no-op.
   *
   * @param [in] name Reference to the name of the Operator that has been deleted.
   * @param [in] type The type of Operator that has been deleted.
   */
  void onDelete(const std::string& /*name*/, ncsa::Operator::Type /*type*/) override {}

  /**
   * @brief Listen for data source created updates.  For simplicity, this is a no-op.
   *
   * @param [in] name Reference to the name of the DataSrc that was created.
   * @param [in] type The type of the DataSrc that was created.
   */
  void onCreate(const std::string& /*name*/, ncsa::DataSrc::Type /*type*/) override {}

  /**
   * @brief Listen for data source updates.  For simplicity, this is a no-op.
   *
   * @param [in] op Reference to a DataSrc that has changed.
   */
  void onUpdate(const ncsa::DataSrc& /*op*/) override {}

  /**
   * @brief Listen for data source deleted updates.  For simplicity, this is a no-op.
   *
   * @param [in] name Reference to the name of the DataSrc that was deleted.
   * @param [in] type The type of the DataSrc that was deleted.
   */
  void onDelete(const std::string& /*name*/, ncsa::DataSrc::Type /*type*/) override {}

protected:
  vtkPlusCanonNCSAServer();
  virtual ~vtkPlusCanonNCSAServer();

  bool JoinCompositionSpace();

  /*! Thread for sending data to clients */
  static void* DataSenderThread(vtkMultiThreader::ThreadInfo* data);

  /*! Attempt to send any unsent frames to clients, if unsuccessful, accumulate an elapsed time */
  static PlusStatus SendLatestFramesToClients(vtkPlusCanonNCSAServer& self, double& elapsedTimeSinceLastPacketSentSec);

  /*! Tracked frame interface, sends the selected message type and data to all clients */
  virtual PlusStatus SendTrackedFrame(igsioTrackedFrame& trackedFrame);

  vtkSetStdStringMacro(OutputChannelId);
  vtkGetStdStringMacro(OutputChannelId);
  vtkSetStdStringMacro(ConfigFilename);

private:
  vtkPlusCanonNCSAServer(const vtkPlusCanonNCSAServer&);
  void operator=(const vtkPlusCanonNCSAServer&);

  /*! Transform repository instance */
  vtkSmartPointer<vtkIGSIOTransformRepository> TransformRepository;

  /*! Data collector instance */
  vtkSmartPointer<vtkPlusDataCollector> DataCollector;

  /*! Multithreader instance for controlling threads */
  vtkSmartPointer<vtkMultiThreader> Threader;

  /*! Channel ID to request the data from */
  std::string OutputChannelId;

  /*! Channel to use for broadcasting */
  vtkPlusChannel* BroadcastChannel;

  int DataSenderThreadId;

  std::string ConfigFilename;

  // Active flag for threads (request, respond )
  struct ThreadFlags
  {
    bool Request;
    bool Respond;
    ThreadFlags()
      : Request(false)
      , Respond(false)
    {}
  };
  ThreadFlags DataSenderActive;

  ConfigurationCommon Config;

  ncsa::CompositionSpace* CompositionSpace;
  ncsa::DataSrc* DataSource;
};

#endif
