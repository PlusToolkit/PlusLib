/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSOPENIGTLINKSERVER_H
#define __VTKPLUSOPENIGTLINKSERVER_H

#include "vtkPlusServerExport.h"

#include "vtkObject.h"
#include "vtkMultiThreader.h"
#include "PlusIgtlClientInfo.h" 

#include "igtlMessageBase.h"
#include "igtlServerSocket.h"

class TrackedFrame; 
class vtkDataCollector;
class vtkPlusOpenIGTLinkServer;
class vtkPlusChannel;
class vtkPlusCommandProcessor;
class vtkPlusCommandResponse;
class vtkRecursiveCriticalSection; 
class vtkTransformRepository; 

struct ClientData
{
  ClientData()
  : DataReceiverActive(std::make_pair(false,false))
  , ClientSocket(NULL)
  , DataReceiverThreadId(-1)
  , ClientId(-1)
  , Server(NULL)
  {
  }
  /*! Unique client identifier. First valid value is 1. */
  int ClientId;

  /*! IGTL client socket instance */ 
  igtl::ClientSocket::Pointer ClientSocket; 

  // Active flag for thread (first: request, second: respond )
  std::pair<bool,bool> DataReceiverActive;
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
public:
  static vtkPlusOpenIGTLinkServer *New();
  vtkTypeMacro( vtkPlusOpenIGTLinkServer, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  

  /*! Configures and starts the server from the provided PlusOpenIGTLinkServer XML element */
  PlusStatus Start(vtkDataCollector* dataCollector, vtkTransformRepository* transformRepository, vtkXMLDataElement* serverElement, const std::string& configFilePath);

  /*! Configures and starts the server from the provided device set configuration file */
  PlusStatus Stop();


  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* serverElement, const char* aFilename ); 

  /*! Set server listening port */ 
  vtkSetMacro( ListeningPort, int );
  /*! Get server listening port */ 
  vtkGetMacro( ListeningPort, int );
  
  vtkGetStringMacro(OutputChannelId);

  vtkSetMacro(MissingInputGracePeriodSec, double);
  vtkGetMacro(MissingInputGracePeriodSec, double);

  vtkSetMacro(MaxTimeSpentWithProcessingMs, double);
  vtkGetMacro(MaxTimeSpentWithProcessingMs, double);

  vtkSetMacro(SendValidTransformsOnly, bool); 
  vtkGetMacro(SendValidTransformsOnly, bool); 

  /*! Set data collector instance */
  virtual void SetDataCollector(vtkDataCollector* dataCollector); 
  virtual vtkDataCollector* GetDataCollector();

  /*! Set transform repository instance */
  virtual void SetTransformRepository(vtkTransformRepository* transformRepository); 
  virtual vtkTransformRepository* GetTransformRepository();

  /*! Get number of connected clients */ 
  virtual int GetNumberOfConnectedClients(); 

  /*! Start server */ 
  PlusStatus StartOpenIGTLinkService();
  
  /*! Stop server */ 
  PlusStatus StopOpenIGTLinkService();

  vtkGetStringMacro(ConfigFilename);
    
  /*! 
    Execute all commands in the queue from the current thread (useful if commands should be executed from the main thread) 
    \return Number of executed commands
  */
  int ProcessPendingCommands();  

protected:
  vtkPlusOpenIGTLinkServer();
  virtual ~vtkPlusOpenIGTLinkServer();

  /*! Thread for client connection handling */ 
  static void* ConnectionReceiverThread( vtkMultiThreader::ThreadInfo* data );
  
  /*! Thread for sending data to clients */ 
  static void* DataSenderThread( vtkMultiThreader::ThreadInfo* data );
  
  /*! Thread for receiveing control data from clients */ 
  static void* DataReceiverThread( vtkMultiThreader::ThreadInfo* data );

  /*! Tracked frame interface, sends the selected message type and data to all clients */ 
  virtual PlusStatus SendTrackedFrame( TrackedFrame& trackedFrame ); 
  
  /*! Converts a command response to an OpenIGTLink message that can be sent to the client */
  igtl::MessageBase::Pointer CreateIgtlMessageFromCommandResponse(vtkPlusCommandResponse* response);

  /*! Send status message to clients to keep alive the connection */ 
  virtual void KeepAlive(); 

  /*! Stops clinet's data receiving thread, closes the socket, and removes the client from the client list */
  void DisconnectClient(int clientId);

  /*! Set IGTL CRC check flag (0: disabled, 1: enabled) */ 
  vtkSetMacro(IgtlMessageCrcCheckEnabled, bool); 
  /*! Get IGTL CRC check flag (0: disabled, 1: enabled) */ 
  vtkGetMacro(IgtlMessageCrcCheckEnabled, bool); 

  vtkSetMacro(MaxNumberOfIgtlMessagesToSend, int); 
  vtkGetMacro(MaxNumberOfIgtlMessagesToSend, int); 

  /*! 
    Execute a remotely invocated string command
    \param resultString String containing the reply to the command (human readable)
    \return Status code (igtl::StatusMessage::STATUS_OK, STATUS_UNKNOWN_INSTRUCTION, ... see igtl_status.h)
  */ 
  int ExecuteCommand(const char* commandString, std::string& resultString); 

  vtkSetStringMacro(OutputChannelId);

  vtkSetStringMacro(ConfigFilename);

  bool HasGracePeriodExpired();

private:
  
  vtkPlusOpenIGTLinkServer( const vtkPlusOpenIGTLinkServer& );
  void operator=( const vtkPlusOpenIGTLinkServer& );

  /*! IGTL server socket */ 
  igtl::ServerSocket::Pointer ServerSocket;
  
  /*! Transform repository instance */ 
  vtkSmartPointer<vtkTransformRepository> TransformRepository;
  
  /*! Data collector instance */ 
  vtkSmartPointer<vtkDataCollector> DataCollector;

  /*! Multithreader instance for controlling threads */ 
  vtkSmartPointer<vtkMultiThreader> Threader;
 
  /*! Server listening port */ 
  int  ListeningPort;

  /*! Number of retry attempts for message sending to clients */ 
  int NumberOfRetryAttempts; 

  /*! Delay between retry attempts */ 
  int DelayBetweenRetryAttemptsSec; 

  /*! Maximum number of IGTL messages to send in one period */ 
  int MaxNumberOfIgtlMessagesToSend; 

  // Active flag for threads (first: request, second: respond )
  std::pair<bool,bool> ConnectionActive;
  std::pair<bool,bool> DataSenderActive;

  // Thread IDs 
  int  ConnectionReceiverThreadId;
  int  DataSenderThreadId;

  /*! List of connected clients */ 
  std::list<ClientData> IgtlClients;

  /*! Mutex instance for accessing client data list */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> IgtlClientsMutex;
  
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

  /*! Flag for IGTL CRC check */ 
  bool IgtlMessageCrcCheckEnabled; 

  /*! Factory to generate commands that are invoked remotely */ 
  vtkSmartPointer<vtkPlusCommandProcessor> PlusCommandProcessor;

  /*! Channel ID to request the data from */
  char* OutputChannelId;

  /*! Channel to use for broadcasting */
  vtkPlusChannel* BroadcastChannel;

  char* ConfigFilename;

  vtkPlusLogger::LogLevelType GracePeriodLogLevel;
  double MissingInputGracePeriodSec;
  double BroadcastStartTime;

  /*! Counter to generate unique client IDs. Access to the counter is not protected, therefore all clients should be created from the same thread. */
  static int ClientIdCounter;
};


#endif

