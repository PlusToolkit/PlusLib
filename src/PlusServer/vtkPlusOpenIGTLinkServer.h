/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __VTKPLUSOPENIGTLINKSERVER_H
#define __VTKPLUSOPENIGTLINKSERVER_H

// Local includes
#include "vtkPlusServerExport.h"
#include "PlusIgtlClientInfo.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusIgtlMessageFactory.h"
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

// IGTL includes
#include <igtlMessageBase.h>
#include <igtlServerSocket.h>

//class igsioTrackedFrame; 
class vtkPlusDataCollector;
class vtkPlusOpenIGTLinkServer;
class vtkPlusChannel;
class vtkPlusCommandProcessor;
class vtkPlusCommandResponse;
class vtkIGSIORecursiveCriticalSection;
//class vtkIGSIOTransformRepository;

struct ClientData
{
  ClientData()
    : ClientId(-1)
    , ClientSocket(NULL)
    , DataReceiverActive(std::make_pair(false, false))
    , DataReceiverThreadId(-1)
    , Server(NULL)
  {
  }

  /// Unique client identifier. First valid value is 1.
  int ClientId;

  /// IGTL client socket instance
  igtl::ClientSocket::Pointer ClientSocket;

  /// Client specific timeouts
  uint32_t ClientSocketSendTimeout;
  uint32_t ClientSocketReceiveTimeout;

  /// Active flag for thread (first: request, second: respond )
  std::pair<bool, bool> DataReceiverActive;
  int DataReceiverThreadId;

  PlusIgtlClientInfo ClientInfo;

  vtkPlusOpenIGTLinkServer* Server;
};

/*!
  \class vtkPlusOpenIGTLinkServer
  \brief This class provides a network interface for data acquired by Plus as an OpenIGTLink server.

  As soon as a client connects to the server, the server start streaming those image and tracking information
  that are defined in the server's default client information (DefaultClientInfo element in the device set configuration file).

  A connected client any time can change what information the server sends to it by sending a CLIENTINFO message. The CLIENTINFO
  message is encoded the same way as an OpenIGTLink STRING message, the only difference is that the message type is
  CLIENTINFO (implemented in igtl::PlusClientInfoMessage). The contents of the message is an XML string, describing the
  requested image and tracking information in the same format as in the DefaultClientInfo element in the device set
  configuration file.

  \ingroup PlusLibPlusServer
*/
class vtkPlusServerExport vtkPlusOpenIGTLinkServer: public vtkObject
{
  typedef std::map<int, std::vector<igtl::MessageBase::Pointer> > ClientIdToMessageListMap;

public:
  static vtkPlusOpenIGTLinkServer* New();
  vtkTypeMacro(vtkPlusOpenIGTLinkServer, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Configures and starts the server from the provided PlusOpenIGTLinkServer XML element */
  PlusStatus Start(vtkPlusDataCollector* dataCollector, vtkIGSIOTransformRepository* transformRepository, vtkXMLDataElement* serverElement, const std::string& configFilePath);

  /*! Configures and starts the server from the provided device set configuration file */
  PlusStatus Stop();


  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* serverElement, const std::string& aFilename);

  /*! Set server listening port */
  vtkSetMacro(ListeningPort, int);
  /*! Get server listening port */
  vtkGetMacroConst(ListeningPort, int);

  vtkGetStdStringMacro(OutputChannelId);

  vtkSetMacro(MissingInputGracePeriodSec, double);
  vtkGetMacroConst(MissingInputGracePeriodSec, double);

  vtkSetMacro(MaxTimeSpentWithProcessingMs, double);
  vtkGetMacroConst(MaxTimeSpentWithProcessingMs, double);

  vtkSetMacro(SendValidTransformsOnly, bool);
  vtkGetMacroConst(SendValidTransformsOnly, bool);

  vtkSetMacro(DefaultClientSendTimeoutSec, float);
  vtkGetMacroConst(DefaultClientSendTimeoutSec, float);

  vtkSetMacro(DefaultClientReceiveTimeoutSec, float);
  vtkGetMacroConst(DefaultClientReceiveTimeoutSec, float);

  /*! Set data collector instance */
  vtkSetMacro(DataCollector, vtkPlusDataCollector*);
  vtkGetMacroConst(DataCollector, vtkPlusDataCollector*);

  /*! Set transform repository instance */
  vtkSetMacro(TransformRepository, vtkIGSIOTransformRepository*);
  vtkGetMacroConst(TransformRepository, vtkIGSIOTransformRepository*);

  /*! Get number of connected clients */
  virtual unsigned int GetNumberOfConnectedClients() const;

  /*! Retrieve a COPY of client info for a given clientId
    Locks access to the client info for the duration of the function
    */
  virtual PlusStatus GetClientInfo(unsigned int clientId, PlusIgtlClientInfo& outClientInfo) const;

  /*! Start server */
  PlusStatus StartOpenIGTLinkService();

  /*! Stop server */
  PlusStatus StopOpenIGTLinkService();

  vtkGetStdStringMacro(ConfigFilename);

  vtkGetMacro(IGTLProtocolVersion, int);
  vtkGetMacro(IGTLHeaderVersion, int);

  /*!
    Execute all commands in the queue from the current thread (useful if commands should be executed from the main thread)
    \return Number of executed commands
  */
  int ProcessPendingCommands();

protected:
  vtkPlusOpenIGTLinkServer();
  virtual ~vtkPlusOpenIGTLinkServer();

  /*! Add a response to the queue for sending to the client */
  PlusStatus QueueMessageResponseForClient(int clientId, igtl::MessageBase::Pointer message);

  /*! Thread for client connection handling */
  static void* ConnectionReceiverThread(vtkMultiThreader::ThreadInfo* data);

  /*! Thread for sending data to clients */
  static void* DataSenderThread(vtkMultiThreader::ThreadInfo* data);

  /*! Attempt to send any unsent frames to clients, if unsuccessful, accumulate an elapsed time */
  static PlusStatus SendLatestFramesToClients(vtkPlusOpenIGTLinkServer& self, double& elapsedTimeSinceLastPacketSentSec);

  /*! Process the message replies queue and send messages */
  static PlusStatus SendMessageResponses(vtkPlusOpenIGTLinkServer& self);

  /*! Process the command replies queue and send messages */
  static PlusStatus SendCommandResponses(vtkPlusOpenIGTLinkServer& self);

  /*! Thread for receiving control data from clients */
  static void* DataReceiverThread(vtkMultiThreader::ThreadInfo* data);

  /*! Tracked frame interface, sends the selected message type and data to all clients */
  virtual PlusStatus SendTrackedFrame(igsioTrackedFrame& trackedFrame);

  /*! Converts a command response to an OpenIGTLink message that can be sent to the client */
  igtl::MessageBase::Pointer CreateIgtlMessageFromCommandResponse(vtkPlusCommandResponse* response);

  /*! Send status message to clients to keep alive the connection */
  virtual void KeepAlive();

  /*! Stops client's data receiving thread, closes the socket, and removes the client from the client list */
  void DisconnectClient(int clientId);

  /*! Set IGTL CRC check flag (0: disabled, 1: enabled) */
  vtkSetMacro(IgtlMessageCrcCheckEnabled, bool);
  /*! Get IGTL CRC check flag (0: disabled, 1: enabled) */
  vtkGetMacroConst(IgtlMessageCrcCheckEnabled, bool);

  vtkSetMacro(LogWarningOnNoDataAvailable, bool);
  vtkGetMacroConst(LogWarningOnNoDataAvailable, bool);

  vtkSetMacro(MaxNumberOfIgtlMessagesToSend, int);
  vtkGetMacroConst(MaxNumberOfIgtlMessagesToSend, int);

  vtkSetMacro(NumberOfRetryAttempts, int);
  vtkGetMacroConst(NumberOfRetryAttempts, int);

  vtkSetMacro(DelayBetweenRetryAttemptsSec, double);
  vtkGetMacroConst(DelayBetweenRetryAttemptsSec, double);

  vtkSetMacro(KeepAliveIntervalSec, double);
  vtkGetMacroConst(KeepAliveIntervalSec, double);

  vtkSetStdStringMacro(OutputChannelId);
  vtkSetStdStringMacro(ConfigFilename);

  bool HasGracePeriodExpired();

private:
  vtkPlusOpenIGTLinkServer(const vtkPlusOpenIGTLinkServer&);
  void operator=(const vtkPlusOpenIGTLinkServer&);

  /*! IGTL server socket */
  igtl::ServerSocket::Pointer ServerSocket;

  /*! Transform repository instance */
  vtkSmartPointer<vtkIGSIOTransformRepository> TransformRepository;

  /*! Data collector instance */
  vtkSmartPointer<vtkPlusDataCollector> DataCollector;

  /*! Multithreader instance for controlling threads */
  vtkSmartPointer<vtkMultiThreader> Threader;

  /*! The version of the IGTL protocol that this server is using */
  int IGTLProtocolVersion;
  int IGTLHeaderVersion;

  /*! Server listening port */
  int ListeningPort;

  /*! Number of retry attempts for message sending to clients */
  int NumberOfRetryAttempts;

  /*! Delay between retry attempts */
  double DelayBetweenRetryAttemptsSec;

  /*! Maximum number of IGTL messages to send in one period */
  int MaxNumberOfIgtlMessagesToSend;

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
  ThreadFlags ConnectionActive;
  ThreadFlags DataSenderActive;

  // Thread IDs
  int ConnectionReceiverThreadId;
  int DataSenderThreadId;

  /*! List of connected clients */
  std::list<ClientData> IgtlClients;

  /*! igtl Factory for message sending */
  vtkSmartPointer<vtkPlusIgtlMessageFactory> IgtlMessageFactory;

  /*! Mutex instance for accessing client data list */
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection> IgtlClientsMutex;

  /*! Last sent tracked frame timestamp */
  double LastSentTrackedFrameTimestamp;

  /*! Maximum time spent with processing (getting tracked frames, sending messages) per second (in milliseconds) */
  int MaxTimeSpentWithProcessingMs;

  /*! Time needed to process one frame in the latest recording round (in milliseconds) */
  int LastProcessingTimePerFrameMs;

  /*! Whether or not the server should send invalid transforms through the IGT Link */
  bool SendValidTransformsOnly;

  /*!
  Default IGT client info used for sending data to clients.
  Used only if the client didn't set IGT message types and transform/image/string names.
  The default client info can be set in the devices set config file in the DefaultClientInfo element.
  */
  PlusIgtlClientInfo DefaultClientInfo;
  float DefaultClientSendTimeoutSec;
  float DefaultClientReceiveTimeoutSec;

  /*! Flag for IGTL CRC check */
  bool IgtlMessageCrcCheckEnabled;

  /*! Factory to generate commands that are invoked remotely */
  vtkSmartPointer<vtkPlusCommandProcessor> PlusCommandProcessor;

  /*! List of messages to be sent as replies per client*/
  ClientIdToMessageListMap MessageResponseQueue;

  /*! Mutex to protect access to the message response list */
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection> MessageResponseQueueMutex;

  /*! Channel ID to request the data from */
  std::string OutputChannelId;

  /*! Channel to use for broadcasting */
  vtkPlusChannel* BroadcastChannel;

  bool LogWarningOnNoDataAvailable;

  double KeepAliveIntervalSec;

  std::string ConfigFilename;

  vtkIGSIOLogger::LogLevelType GracePeriodLogLevel;
  double MissingInputGracePeriodSec;
  double BroadcastStartTime;

  /*! Counter to generate unique client IDs. Access to the counter is not protected, therefore all clients should be created from the same thread. */
  static int ClientIdCounter;

  static const float CLIENT_SOCKET_TIMEOUT_SEC;
};

#endif