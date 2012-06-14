#pragma once



class TcpClientImp;
class  WSAIF;

/// Length of filter condition used in CallbackEntry.
static const int TCP_FILTER_LEN = 128;


/// Exception base class use by TcpClient.
class TcpClientException
{
public:
	/// String describing the exception condition.
	CString Message;
	/// Value defining the exception.
	DWORD ReturnValue;
};

/// Exception thrown by TcpClient when a wait times out.
class TcpClientWaitException : public TcpClientException
{
};


/// <summary>
/// CallbackEnabled is an abstract class with no data members or member functions.
/// 
/// Objects of type TcpClient can register callback functions. These
/// callback functions can be either standard C++ functions, or can be members of
/// a class, in which case a pointer to the object (<tt>this</tt>) is needed.
/// 
/// CallbackEnabled is used by CallbackEntry.
/// </summary>
/// <remarks>
/// The class, whose member function is registered as a callback function <b>need not</b>
/// inherit CallbackEnabled.  
/// </remarks>
/// 
/// <see cref="CallbackEntry" />
/// <see cref="TcpClient" />
class CallbackEnabled{ };

/// <summary>
/// The structure CallbackEntry allows the registration of callbacks.
/// 
/// Two cases are supported - the first case of function in an object/class
/// and the second is just ordinary function.
/// 
/// The function itself is responsible to allocate a buffer for the data to
/// be written to. What is needed is: (1) pointer to the function, (2) pointer
/// to the allocated memory area for the data stream, and (3) the size of the
/// allocated memory area.
/// 
/// When a function of a class is registered, then a pointer to the object
/// owning the callback function is also needed.
/// </summary>
/// 
/// <see cref="CallbackEntry" />
/// <see cref="TcpClient" />
typedef struct{
	CallbackEnabled* CallbackObj;
	char *Buffer;                    ///< Passed as first argument to callback function
	size_t BufferSize;               ///< Passed as second argument to the callback function
	
	char Filter[TCP_FILTER_LEN];    ///< Condition to call the callback. Up to 128 values. 
	void (* CallbackFunc)(char* buf, size_t bufferSize); ///< Pointer to a C++ function 
	void (*CallbackMember)(CallbackEnabled* obj, char* buf, size_t bufferSize); ///< Pointer to class member
} CallBackEntry;

/// <summary>
/// The TcpClient class enables the communication
/// to the OEM interface and to the toolbox.
/// 
/// The communication process has the added complexity of
/// that everything goes through a single communication channel.
/// The user can subscribe to certain events in the scanner - for example
/// FREEZE and RUN. When an event occurs, then the scanner sends
/// some information.
/// 
/// The client must be ready to receive the message. Each message
/// starts with a TEXT identifier describing the text message.
/// It is thus possible to filter the incoming text messages.
/// 
/// 
/// A TcpClient object starts a separate thread, which only listens
/// to the port for incoming messages. If a message has come, then
/// it is recorded. The user application can register call-backs.
/// A given CallBack function is executed, if the received message
/// starts with a given pattern of characters - for example SDATA.
/// This pattern is stored in the <tt>filter</tt> field of the CallbackEntry
/// 
/// The callback functionality can be implemented in two-ways.
/// - Register a c++ function. This function must not be member of a given class,
///   because the "this" pointer will not be filled properly.
/// 
///  - Register a class member function. Note that the class which is able to
///   do that should be derived from the CallbackEnabled class.
/// 
/// <b> Example</b>
/// \code
///  TcpClient the_client; 
///  client.setServer(SrvAddr);
///  client.setPort(SrcPort);
///  client.start();
/// \endcode
/// </summary>
class TcpClient
{
public:

	/// <summary>	Constructor of TcpClient. </summary>
	/// <param name="wsaIf">	 	[in] Interface to WinSock. Facilitates dependency injection. </param>
	/// <param name="bufferSize">	Size of the internal buffers. Must be larger than the maximum size
	/// 							of the data in a single transaction. </param>
	/// <param name="port">		 	TCP/IP port number. The port for the OEM interface is 7915 by default.
	/// 							The port for the toolbox is 5001 by default. </param>
	/// <param name="host">		 	The address of the server. It can be either a name such as "astra.bkmed.dk"
	/// 							or an IP idress such as "10.200.3.9"
	/// 							
	/// 							The constructor allocates memory for the communication buffers and initializes
	/// 							variables. </param>
	///
	/// <remarks>
	/// After a TcpClient object is created, use start() to connect to the server.
	/// </remarks>
	/// <see cref="Start()" />
	/// <see cref="Stop()" />
	TcpClient(WSAIF* wsaIf, size_t bufferSize, unsigned short port, LPCTSTR host);
	/// <summary>
	/// The destructor ~TcpClient stops the reader thread, if running
	/// and releases the allocated memory. Waits up to 5 seconds
	/// for the reader thread to terminate.
	/// </summary>
	virtual ~TcpClient();

public:
	/// <summary>	Connect to a server. </summary>
	/// <returns>	True if connection is successful, and false otherwise. </returns>
	/// <remarks>
	/// This function just connects to a server. It does not
	/// control the behaviour of a reader() thread.
	/// </remarks>
	/// <see cref="Start()" />
	/// <see cref="Stop()" />
	virtual bool Connect();

	/// <summary>	Closes the socket to the server. </summary>
	virtual void Disconnect();

	/// <summary>	Connects to server and starts a new reader thread. </summary>
	/// <returns>	True if connection is established and a thread is running. </returns>
	/// <remarks>
	/// If a reader thread is running, then the thread is stopped.
	/// If there is a connection, then the application disconnects first.
	/// </remarks>
	/// <see cref="Stop()" />
	/// <see cref="Connect()" />
	/// <see cref="Disconnect()" />
	virtual bool Start();

	/// <summary>	stop() stops the reader thread and disconnects from the server. </summary>
	/// <returns>   True upon success.</returns>
	/// <see cref="Start()" />
	/// <see cref="Connect()" />
	/// <see cref="Disconnect()" />
	virtual bool Stop();

	/// <summary>	Register a callback function. </summary>
	/// <param name="callbackFunc">	[in] Pointer to the callback function. </param>
	/// <param name="buf">	   	    Pointer to a buffer, where the callback function will receive data. </param>
	/// <param name="buf_size">	    Size of buf. </param>
	/// <param name="filter">  	    Start of the message which will trigger the callback function.
	/// 							The callback function will be called
	/// 							every time, when a message starting with a given header (==filter) is received
	/// 							by the receiver() member function of the class; </param>
	/// <see cref="Reader()" />
	/// <see cref="RegisterCallbackObj()" />
	virtual void RegisterCallbackFunc(void(*callbackFunc)(char* buffer, size_t bufferSize), char* buffer, size_t bufferSize, char* filter);

	/// <summary>	Register a member function of an object as a callback function. </summary>
	/// <param name="obj">			 	[in] Pointer to the object whose member function becomes a callback. </param>
	/// <param name="callbackMember">	[in] Pointer to the member function of the class. </param>
	/// <param name="buf">		 	    Buffer to which the received data stream is copied. </param>
	/// <param name="buf_size">      	Size of the buffer buf. </param>
	/// <param name="filter">    	 	Text. If the incoming stream starts with this text, then the function will be called. </param>
	/// <see cref="Reader()" />
	/// <see cref="RegisterCallbackFunc()" />
	//virtual void RegisterCallbackObj(CallbackEnabled* obj, void (CallbackEnabled::*callbackMember)(char* buffer, size_t bufferSize), char *buffer, size_t bufferSize, char* filter);
	virtual void RegisterCallbackObj(CallbackEnabled* obj, void (*callbackMember)(CallbackEnabled*obj, char* buffer, size_t bufferSize), char *buffer, size_t bufferSize, char* filter);
	 
	/// <summary>	Blocking read operation. </summary>
	/// <param name="dest">  	[out] Buffer where the received data stream is to be copied. </param>
	/// <param name="len">   	Length of the buffer dest. </param>
	/// <param name="filter">	[in] Filter for the message expected by the read operation.
	/// 						If the filter is empty or NULL, then the first received
	/// 						data stream is returned to the caller. </param>
	/// <returns>
	/// The number of bytes received.
	/// </returns>
	/// <remarks>
	/// The read() function sets the m_read_pending flag and sleeps
	/// until the reader() thread wakes it. m_on_read_event is used
	/// for synchronization.
	/// This function might sleep forever, if no data is received.
	/// </remarks>
	/// <see cref="Reader()" />
	/// <see cref="StripInput()" />
	virtual size_t Read(char* dest, size_t len, char* filter = NULL);

	/// <summary>	Blocking write operation. </summary>
	/// <param name="src">	The data stream to be sent. </param>
	/// <param name="len">	Number of bytes to send. </param>
	/// <returns>	The number of bytes successfully sent. </returns>
	/// <remarks>
	/// Runs in the same thread as the caller
	/// </remarks>
	/// <see cref="SetOutput()" />
	/// <see cref="Read()" />
	virtual size_t Write(LPCTSTR src, size_t len);

	/// <summary>
	/// Sets a new name of a server. If connected to some server, the client
	/// will disconnect from the old server and connect to the new one.
	/// </summary>
	/// <param name="servername">	[in] server_name can be either a name such as "www.google.com" or an internet
	/// 							address such as "10.200.3.9". </param>
	virtual void SetServer(char* serverName);
	/// <summary>	Gets the server IP-address. </summary>
	/// <returns>	The IP-address in a form such as "10.200.3.9". </returns>
	virtual const char* GetServer() const;

	/// <summary>
	/// Change the port of the connection. If connected, disconnect() and connect()
	/// using the new port.
	/// </summary>
	/// <param name="port">	Port number. Default for OEM interface is 7915 and for toolbox 5001. </param>
	virtual void SetPort(unsigned short port);
	/// <summary>	Gets the port number for the connection. </summary>
	/// <returns>	The port number. </returns>
	virtual unsigned short GetPort() const;


private:
	/// Private implementation.
	TcpClientImp* impl;
};


/**
  \page PageUseTcpClient  How to use Tcp Client
  Here is a brief example of how to send queries to and receive replies from the OEM interface

  \code
	int SendReceiveOemCommands()
	{
		WSAIF wsaif;  // WSAIF is Microsofts ' way to handle socket communications
		TcpClient oemClient(&wsaif,         // Pointer to the interface
		                     2*1024*1024,   // Size of internal buffers
							 7915,          // Port of OEM interface
							 "10.200.28.45" // IP Address
							 );

		bool connected = oemClient.Connect(); // Establish connection

		if (!connected) return -1;   // Quit if there is no connection
		

		oemClient.Start();    // Start the client. A separate reading thread runs
		                      // in the background.

		static char query[ 2048 ];       // Place-holder for the queries
		static char reply[2*1024*1024];  // Place-holder for the replies

		memset(query,0, sizeof(query));   
		memset(reply,0,sizeof(reply));


		bool quit = false;      // We will run a loop until quit becomes true

		while(!quit)
		{
			cout << "query >> ";
			cin >> query;

			quit = !_strnicmp(query,"quit", 4);

			if (!quit)
			{
				memset(reply, 0, sizeof(reply));
				oemClient.Write(query, strlen(query));
				oemClient.Read(reply,sizeof(reply));
				cout << "reply  ] " << reply << endl;
			}
			else
			{
				cout << endl << "Quitting. May take up to 2 seconds" << endl;
			}
		}

		oemClient.Stop();    // It is necessary to stop the client
		return 0;
	}
  \endcode

 */