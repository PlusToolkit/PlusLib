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

class vtkDataCollector;
class vtkTransformRepository; 
class TrackedFrame; 
class vtkMutexLock; 

/*!
  \class vtkPlusOpenIGTLinkServer 
  \brief This class provides a network interface for data acquired by Plus as an OpenIGTLink server.

  After the server is started, it waits for a client sending request message of type
  "igtl::PlusClientInfoMessage" with requested message types and transform names.  

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusOpenIGTLinkServer: public vtkObject
{
public:
  
  static vtkPlusOpenIGTLinkServer *New();
  vtkTypeRevisionMacro( vtkPlusOpenIGTLinkServer, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  /*! Read the configuration file in XML format and set up the devices */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aDataCollectionConfig ); 

  /*! Set server listening port */ 
  vtkSetMacro( ListeningPort, int );
  /*! Get server listening port */ 
  vtkGetMacro( ListeningPort, int );
  
  /*! Set data collector instance */
  virtual void SetDataCollector(vtkDataCollector* dataCollector); 

  /*! Set transform repository instance */
  virtual void SetTransformRepository(vtkTransformRepository* transformRepository); 

  /*! Start server */ 
  PlusStatus Start();
  
  /*! Stop server */ 
  PlusStatus Stop();
    
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

  /*! Mutex instance for safe data access */ 
  vtkSmartPointer<vtkMutexLock> Mutex;
  
  /*! Server listening port */ 
  int  ListeningPort;

  /*! Number of retry attempts for message sending to clients */ 
  int NumberOfRetryAttempts; 

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

  /*! Default transform name used for sending igt image message. 
  Used only if the client didn't set image transform name (can be set from config file)
  */
  PlusTransformName DefaultImageTransformName; 

};


#endif

