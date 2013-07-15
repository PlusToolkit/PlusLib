/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSOPENIGTLINKSERVER_H
#define __VTKPLUSOPENIGTLINKSERVER_H

#include "vtkObject.h"
#include "vtkMultiThreader.h"
#include "PlusIgtlClientInfo.h" 

#include "igtlServerSocket.h"

class TrackedFrame; 
class vtkDataCollector;
class vtkPlusChannel;
class vtkPlusCommandProcessor;
class vtkRecursiveCriticalSection; 
class vtkTransformRepository; 

/*!
  \class vtkPlusOpenIGTLinkServer 
  \brief This class provides a network interface for data acquired by Plus as an OpenIGTLink server.

  After the server is started, it waits for a client sending request message of type
  "igtl::PlusClientInfoMessage" with requested message types and transform names.  

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusOpenIGTLinkServer: public vtkObject
{
  typedef std::map< int, std::vector<std::string> > PreviousCommandIdMap;
  typedef PreviousCommandIdMap::iterator PreviousCommandIdMapIterator;

  typedef std::map< int, double> LastCommandTimestampMap;
  typedef LastCommandTimestampMap::iterator LastCommandTimestampMapIterator;

public:
  static vtkPlusOpenIGTLinkServer *New();
  vtkTypeRevisionMacro( vtkPlusOpenIGTLinkServer, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig, const char* aFilename ); 

  /*! Set server listening port */ 
  vtkSetMacro( ListeningPort, int );
  /*! Get server listening port */ 
  vtkGetMacro( ListeningPort, int );
  
  vtkGetStringMacro(OutputChannelId);

  /*! Set data collector instance */
  virtual void SetDataCollector(vtkDataCollector* dataCollector); 
  virtual vtkDataCollector* GetDataCollector();

  /*! Set transform repository instance */
  virtual void SetTransformRepository(vtkTransformRepository* transformRepository); 
  virtual vtkTransformRepository* GetTransformRepository();

  /*! Get number of connected clients */ 
  virtual int GetNumberOfConnectedClients(); 

  /*! Start server */ 
  PlusStatus Start();
  
  /*! Stop server */ 
  PlusStatus Stop();

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
  
  /*! Send status message to clients to keep alive the connection */ 
  virtual PlusStatus KeepAlive(); 

  /*! Set IGTL CRC check flag (0: disabled, 1: enabled) */ 
  vtkSetMacro(IgtlMessageCrcCheckEnabled, int); 
  /*! Get IGTL CRC check flag (0: disabled, 1: enabled) */ 
  vtkGetMacro(IgtlMessageCrcCheckEnabled, int); 

  /*! 
    Execute a remotely invocated string command
    \param resultString String containing the reply to the command (human readable)
    \return Status code (igtl::StatusMessage::STATUS_OK, STATUS_UNKNOWN_INSTRUCTION, ... see igtl_status.h)
  */ 
  int ExecuteCommand(const char* commandString, std::string& resultString); 

  vtkSetStringMacro(OutputChannelId);

  vtkSetStringMacro(ConfigFilename);

private:
	
  /*! Get client socket corresponding to a client ID. Used by the command processor, which identifies clients by ID. */
  igtl::ClientSocket::Pointer GetClientSocket(int clientId);

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

  /*! Mutex instance for safe data access */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> Mutex;
  
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
  std::pair<bool,bool> DataReceiverActive;

  // Thread IDs 
  int  ConnectionReceiverThreadId;
  int  DataSenderThreadId;
  int  DataReceiverThreadId;

  /*! List of connected clients */ 
  std::list<PlusIgtlClientInfo> IgtlClients;
  
  /*! Last sent tracked frame timestamp */ 
  double LastSentTrackedFrameTimestamp; 

  /*! Maximum time spent with processing (getting tracked frames, sending messages) per second (in milliseconds) */
  int MaxTimeSpentWithProcessingMs; 

  /*! Time needed to process one frame in the latest recording round (in milliseconds) */
  int LastProcessingTimePerFrameMs;

  /*! Whether or not the server should send invalid transforms through the IGT Link */
  bool SendValidTransformsOnly;

  /*! 
  Default IGT message types used for sending data to clients. 
  Used only if the client didn't set IGT message types (can be set from config file)
  */
  std::vector<std::string> DefaultIgtlMessageTypes; 

  /*! 
  Default transform names used for sending igt transform and position messages. 
  Used only if the client didn't set transform names (can be set from config file)
  */ 
  std::vector<PlusTransformName> DefaultTransformNames;
  
  /*! Default transform names used for sending igt image message. 
  Used only if the client didn't set image transform name (can be set from config file)
  */
  std::vector<PlusIgtlClientInfo::ImageStream> DefaultImageStreams; 

  /*! Flag for IGTL CRC check (0: disabled, 1: enabled) */ 
  int IgtlMessageCrcCheckEnabled; 

  /*! Factory to generate commands that are invoked remotely */ 
  vtkSmartPointer<vtkPlusCommandProcessor> PlusCommandProcessor;

  /*! Channel ID to request the data from */
  char* OutputChannelId;

  /*! Channel to use for broadcasting */
  vtkPlusChannel* BroadcastChannel;

  char* ConfigFilename;

  /* Record the previous IDs received for all clients */
  PreviousCommandIdMap PreviousCommands;

  /* Record the last received command timestamp */
  LastCommandTimestampMap LastCommandTimestamp;
};


#endif

