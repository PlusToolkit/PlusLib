#include <assert.h>
#include <sstream>
#include <tchar.h>
#include "TCPClient.h"
#include "WSAIF.h"
#include <list>
#include <process.h>
//#include "CasaLogInterface.h"


/// Time that a client will wait for a message before timing out (unit: ms).
static const int CLIENT_READ_TIMEOUT = 12000;

using namespace std;

/// The Windows' _beginthread function needs a static function.
/// The pointer "this" is passed to the function, so that it knows
/// which object is running in the separate thread.
static unsigned int __stdcall StatThreadFunc(void* data);

//////////////////////////////////////////////////////////////////////////
//
//
// TcpClientImp Declaration
//
//
//////////////////////////////////////////////////////////////////////////

class  TcpClientImp
{
public:
	TcpClientImp(WSAIF* wsaIf, size_t bufferSize, unsigned short port, LPCTSTR host);
	~TcpClientImp();

	bool Connect();           ///< connect to a server
	void Disconnect();        ///< close the connection

	bool Start();            ///< Start a server
	bool Stop();

	void RegisterCallbackFunc(void(*callbackFunc)(char* buffer, size_t bufferSize), char* buffer, size_t bufferSize, char* filter);
	//void RegisterCallbackObj(CallbackEnabled* obj, void (CallbackEnabled::*callbackMember)(char* buffer, size_t bufferSize), char *buffer, size_t bufferSize, char* filter);
	void RegisterCallbackObj(CallbackEnabled* obj, void (*callbackMember)(CallbackEnabled*obj, char* buffer, size_t bufferSize), char *buffer, size_t bufferSize, char* filter);

	size_t Read(char* dest, size_t len, char* filter = NULL);
	size_t Write(LPCTSTR src, size_t len);

	void SetServer(char* serverName);
	const char* GetServer() const;

	void SetPort(unsigned short port);
	unsigned short GetPort() const;

private:
	// Special tokens in the communication protocol.
	enum
	{
		SOH = 1,  ///> Start of header.
		EOT = 4,  ///> End of header.
		ESC = 27, ///> Escape.
	};

	/// <summary>	Reader thread. </summary>
	/// <param name="data">	[in] A pointer to void which is set to the value of <b>this</b>
	/// 					of the object that has started the thread. </param>
	/// <returns>
	/// Nothing important. Return value required by Windows API.
	/// </returns>
	/// <remarks>
	/// The StatThreadFunc() runs until the this->stopThread flag is set to
	/// true. Then the function exits. If the connection has dropped because of
	/// some reason, StatThreadFunc() will attempt to reconnect.
	/// When connected, the this->read() function is called.
	/// </remarks>
	/// <see cref="Reader()" />
	/// <see cref="Start()" />
	/// <see cref="Stop()" />
	static unsigned int __stdcall StatThreadFunc(void* data);

	/// <summary>
	/// Continuously reads input from the TCP/IP port.
	/// 
	/// The read operation generates a time-out every 2
	/// seconds. If the result of the recv() function
	/// returns an error, then it is assumed that connection has been
	/// closed. Data is stored in this->readBuffer.
	/// The data stream starts with 1 and finishes with 4.
	/// 1 and 4 are not encountered in the data stream. "ESCAPED" characters
	/// are used to meet this condition.
	/// 
	/// The received data stream is stripped of ESCape characters.
	/// If a read operation is pending, then the Event for reading is triggered.
	/// The read() operation can set a filter for the messages.
	/// 
	/// If read() is not triggered, then the list of callback functions is
	/// traversed. If a callback has been registered and the start of the
	/// message is equal to the filter of the callback function, then
	/// the callback function is called.
	/// 
	/// The function exits:
	///  - When the this->stopThread is set
	///  - When recv() generates an error (connection dropped). Then disconnect() is
	///    called.
	/// </summary>
	/// <remarks>
	/// It may take up to 2 seconds from the moment this->stopThread is set to the moment
	/// reader() returns.
	/// </remarks>
	/// <see cref="RegisterCallbackFunc()" />
	/// <see cref="RegisterCallbackObj()" />
	/// <see cref="StartThreadFunc()" />
	void reader();

	/// <summary>	Strip the special characters. </summary>
	/// <param name="dest">		[out] Place where to store the destination data. </param>
	/// <param name="destSize">	Size of the destination buffer. </param>
	/// <returns>
	/// The number of bytes (characters) that have been written to dest.
	/// </returns>
	/// <remarks>
	/// Strip the special characters. It reads from this->readBuffer
	/// and writes to dest.
	/// 
	/// The first character in a stream is always (1)
	/// Indicator for end-of-stream is (4)
	/// Another special symbol is (27) (ESC character).
	/// 
	/// The encoding for ESCAPEd characters is simple - (27)(bitwise_not_ symbol)
	/// </remarks>
	/// <see cref="Reader()" />
	/// <see cref="SetOutput()" />
	size_t stripInput(char* dest, size_t destSize);

	/// <summary>	Copy the data from src to this->writeBuffer. </summary>
	/// <param name="src">	  	Buffer containing a message to be encoded. </param>
	/// <param name="src_len">	The number of bytes/characters in src. </param>
	/// <returns> The number of bytes in this->writeBuffer </returns>
	/// <remarks>
	/// The function does not just copy data - it encodes according to the OEM protocol.
	/// Detect in the process the special symbols such as char(1), char(4), char(27)
	/// and replaced them with ESC NOT(char). In other words set in the output stream
	/// the value 27 (ASCII(ESC)) and the bitwise-not value of the special character.
	/// The function also places char(1) at the start of the stream and char(4)
	/// at the end of the data stream.
	/// </remarks>
	/// <see cref="StripInput()" />
	/// <see cref="Write()" />
	size_t setOutput(const char* src, size_t src_len);

private:
	/// <summary>	Convert an error code produced by a wait error to a message string. </summary>
	/// <param name="errorCode">	The error code. </param>
	/// <returns>	The error message string. </returns>
  std::string FormatWaitError(DWORD errorCode);

	/// <summary>	Convert a system error code to a message string. </summary>
	/// <param name="errorCode">	The error code. </param>
	/// <returns>	The error message string. </returns>
	std::string FormatSystemError(DWORD errorCode) const;

private:
	WSAIF* wsaIf;
	std::list <CallBackEntry> callbackList;
	char serverIpName[128]; ///< Server name, e.g. "srv.bkmed.dk" or address e.g. "192.38.72.1"
	
	size_t bufferSize;     ///< Size of the internal buffer for communication
	char* readBuffer;      ///< readBuffer is a buffer used for the TCP/IP communications.

	int numReceivedBytes;    ///< Number of received bytes in the buffer
	                              
	/// Points to the same place to which the argument DEST of read() points 
	/// The reader thread copies the data from readBufferfer to readClientBuffer
	char* readClientBuffer;

	size_t readClientLen;     ///< Same as the LEN argument of the read() function
                            

	size_t numTransmitBytes;    ///< Number of transmitted bytes

	char* writeBuffer;     ///< writeBuffer is a buffer used for the TCP/IP communications
	 
	bool isThreadRunning;  ///< Indicates if a separate thread is running
	bool stopThread;      ///< Stop the thread (flag)
	
	bool isReadPending;    ///< A reading operation is pending
	bool isConnected;       ///< The client is connected to a server

	char readFilter[TCP_FILTER_LEN];    ///< Filter the type of message the reader expects. Zero-terminated string

	struct sockaddr_in  serverAddress;  ///< The address of the server

	HANDLE readerThread;                    ///< Handle to the thread running the reader function
	HANDLE onReadEvent;           ///< The command "read" sets an internal flag and waits for this event

	unsigned short port;              ///< Port to work with
	WSADATA wsaData;
	SOCKET  tcpSocket;
	struct hostent* hostEnt;          ///< Contains things like the address of host
};


//////////////////////////////////////////////////////////////////////////
//
//
//		TCPClient implementation
//
//
//////////////////////////////////////////////////////////////////////////

TcpClient::TcpClient(WSAIF* wsaIf, size_t bufferSize /* = 16 * 1024 * 1024 */, unsigned short port /* = TCP_CLIENT_DEF_PORT */, LPCTSTR host /* = TCP_CLIENT_DEF_HOST */)
:	impl(new TcpClientImp(wsaIf, bufferSize, port, host))
{

}
TcpClient::~TcpClient()
{
	if(this->impl)
	{
		delete this->impl;
		this->impl = NULL;
	}

}


bool TcpClient::Connect()
{
	return this->impl->Connect();
}
void TcpClient::Disconnect()
{
	this->impl->Disconnect();
}
bool TcpClient::Start()
{
	return this->impl->Start();
}
bool TcpClient::Stop()
{
	return this->impl->Stop();
}

void TcpClient::RegisterCallbackFunc(void(*callbackFunc)(char* buffer, size_t bufferSize), char* buffer, size_t bufferSize, char* filter)
{
	this->impl->RegisterCallbackFunc(callbackFunc, buffer, bufferSize, filter);
}

void TcpClient::RegisterCallbackObj(CallbackEnabled* obj, void (*callbackMember)(CallbackEnabled*obj, char* buffer, size_t bufferSize), char *buffer, size_t bufferSize, char* filter)
{
	this->impl->RegisterCallbackObj(obj, callbackMember, buffer, bufferSize, filter);
}
size_t TcpClient::Read(char* dest, size_t len, char* filter /* = NULL */)
{
	return this->impl->Read(dest, len, filter);
}
size_t TcpClient::Write(LPCTSTR src, size_t len)
{
	return this->impl->Write(src, len);
}
void TcpClient::SetServer(char* serverName)
{
	this->impl->SetServer(serverName);
}
const char* TcpClient::GetServer() const
{
	return this->impl->GetServer();
}
void TcpClient::SetPort(unsigned short port)
{
	this->impl->SetPort(port);
}
unsigned short TcpClient::GetPort() const
{
	return this->impl->GetPort();
}


//////////////////////////////////////////////////////////////////////////
//
//
//		TcpClientImp implementation
//
//
//////////////////////////////////////////////////////////////////////////

TcpClientImp::TcpClientImp(WSAIF* wsaIf, size_t bufferSize, unsigned short port, LPCTSTR host)
:	wsaIf(wsaIf)
{
	assert(wsaIf);

	int retval;

	this->callbackList;    

	/* this->readBuffer and this->writeBuffer are used for the send and recv commands */
	this->bufferSize = bufferSize;
	this->readBuffer = new char[bufferSize];
	this->writeBuffer = new char[bufferSize];

	this->isReadPending = false;  /* No read operation is pending */
	this->isConnected = false;     /* The client is connected to a server  */

	memset(this->readFilter, 0, TCP_FILTER_LEN);    /* Filter the type of message the reader expects. Zero-terminated string */

	this->isThreadRunning = false;  /* Indicates if a separate thread is running */
	this->stopThread=true;         /* Stop the thread */

	memset(this->serverIpName, 0, 128);
	memcpy(this->serverIpName, host, strlen(host));
	this->port = port;


	//	CRITICAL_SECTION m_app_buf_in_use;

	this->onReadEvent = CreateEvent(NULL, false, false, "on_read_event");

	if((retval = this->wsaIf->WSAStartup(0x202, &this->wsaData)) != 0)
	{
		this->wsaIf->WSACleanup();
	}
}


TcpClientImp::~TcpClientImp()
{
	if(this->readBuffer != NULL)
		delete[] this->readBuffer;
	if(this->writeBuffer != NULL)
		delete[] this->writeBuffer;

	if(this->isThreadRunning)
	{
		this->stopThread = true;
		WaitForSingleObject(this->readerThread, 5000);  // Wait up to 5 seconds for the thread
	}

	if(this->isReadPending)
	{
		SetEvent(this->onReadEvent);
	}

	CloseHandle(this->onReadEvent);

	if(this->isConnected)
		this->wsaIf->closesocket(this->tcpSocket);
	
	this->wsaIf->WSACleanup();
}


bool TcpClientImp::Connect()
{
	if(this->isConnected)
		return true;
	//TRACE("Connecting to %s\n", this->serverIpName);

	if(isalpha(static_cast<unsigned char>(this->serverIpName[0])))
		this->hostEnt = this->wsaIf->gethostbyname(this->serverIpName);
	else
	{
		unsigned int addr = inet_addr(this->serverIpName);
		this->hostEnt = this->wsaIf->gethostbyaddr((char *)&addr, 4, AF_INET);
	}
	if(!this->hostEnt)
	{
		//TRACE("gethostbyname() or gethostbyaddr() failed. Host tried: %s\n", this->serverIpName);
		int error = this->wsaIf->WSAGetLastError();
		std::string errorMessage = FormatSystemError(error);
		//TRACE("Error: %d: %s\n", error, errorMessage);
	}

	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_port = this->wsaIf->htons(this->port);

    if(this->hostEnt != NULL)
	    this->serverAddress.sin_addr =  *((struct in_addr*)this->hostEnt->h_addr); 

	memset(&(this->serverAddress.sin_zero), 0, 8);

	this->tcpSocket = this->wsaIf->socket(AF_INET, SOCK_STREAM, 0); 

	if(this->tcpSocket == INVALID_SOCKET)
	{
		//TRACE("socket() failed.\n");
		int error = this->wsaIf->WSAGetLastError();
		std::string errorMessage = FormatSystemError(error);
		//TRACE("Error: %d: %s\n", error, errorMessage);

		this->wsaIf->WSACleanup();
		return false;
	}

	int on = 1;
	int status = this->wsaIf->setsockopt(this->tcpSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));

	if(status != 0)
	{
		//TRACE("setsockopt() failed.\n");
		int error = this->wsaIf->WSAGetLastError();
		std::string errorMessage = FormatSystemError(error);
		//TRACE("Error: %d: %s\n", error, errorMessage);

		this->wsaIf->WSACleanup();
		return false;
	}

	if(this->wsaIf->connect(this->tcpSocket, (struct sockaddr*)& this->serverAddress, sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		//TRACE("connect() failed to server: %s\n", this->serverIpName);
		int error = this->wsaIf->WSAGetLastError();
		std::string errorMessage = FormatSystemError(error);
		//TRACE("Error: %d: %s\n", error, errorMessage);
		 
		return (this->isConnected);
	}

	this->isConnected = true;
	return this->isConnected;
}


void TcpClientImp::Disconnect()
{
	if (this->isConnected == false)
		return;                   // Nothing to do

	this->wsaIf->closesocket(this->tcpSocket);
	this->isConnected = false;
}


bool TcpClientImp::Start()
{
	if(this->isThreadRunning)
		Stop();
	if(this->isConnected == true)
		Disconnect();

	if(Connect() == false)
	{ 
		this->isConnected = false;
		this->stopThread = true;
		return false;
	}

	this->stopThread = false;
	this->readerThread = (HANDLE)_beginthreadex(NULL, 0, TcpClientImp::StatThreadFunc, this, 0, NULL);
	this->isThreadRunning = true;
	return (this->isConnected && this->isThreadRunning);
}


bool TcpClientImp::Stop()
{
	/* 
	  Stop the thread first.
	 */

	if(this->isThreadRunning == true)
	{
		this->stopThread = true;
		WaitForSingleObject(this->readerThread, INFINITE); 
	}

	Disconnect();
	return 0;
}


void TcpClientImp::RegisterCallbackFunc(void (*callbackFunc)(char* buffer, size_t bufferSize), char* buffer, size_t bufferSize, char* filter)
{
	CallBackEntry aCallBack;
	aCallBack.CallbackFunc = callbackFunc;

	aCallBack.CallbackMember = NULL;     /* callback_member and ptr_callback_obj must be 0  */
	aCallBack.CallbackObj = NULL;    /* because we are registering an ordinary function */

	aCallBack.Buffer = buffer;
	aCallBack.BufferSize = bufferSize;

	memset(aCallBack.Filter, 0, 128);
	strncpy_s(aCallBack.Filter, 128, filter, strlen(filter));

	this->callbackList.push_back(aCallBack);
}


//void TcpClient::RegisterCallbackObj(CallbackEnabled* obj, void (*callbackMember)(CallbackEnabled*obj, char* buffer, size_t bufferSize), char *buffer, size_t bufferSize, char* filter)

void TcpClientImp::RegisterCallbackObj( CallbackEnabled* obj , void (* callbackMember)(CallbackEnabled*obj, char* buffer, size_t bufferSize), char* buffer, size_t bufferSize, char* filter)
{
	CallBackEntry aCallBack;
	aCallBack.CallbackFunc = NULL;

	aCallBack.CallbackMember = callbackMember;     /* callback_member and ptr_callback_obj must be 0  */
	aCallBack.CallbackObj = obj;    /* because we are registering an ordinary function */

	aCallBack.Buffer = buffer;
	aCallBack.BufferSize = bufferSize;

	memset(aCallBack.Filter, 0, 128);
	strncpy_s(aCallBack.Filter, 128, filter, strlen(filter));

	this->callbackList.push_back(aCallBack);
}


size_t TcpClientImp::Read(char* dest, size_t len, char* filter)
{

	memset(this->readFilter, 0, TCP_FILTER_LEN);
	if(filter != NULL)
	{
		strncpy_s(this->readFilter, TCP_FILTER_LEN - 1, filter, strlen(filter));
	}

	this->readClientBuffer = dest;
	this->readClientLen = len;

	//TRACE("read()\n");

	this->isReadPending = true;
	DWORD res = WaitForSingleObject(this->onReadEvent, CLIENT_READ_TIMEOUT);    /* Read is a blocking operation */
	if(res != WAIT_OBJECT_0)
	{
		std::string csMessage = FormatWaitError(res);
//		LogEngineError1(_T("WaitForSingleObject failed: %s\n"), csMessage);
		TcpClientWaitException e;
		e.Message = csMessage;
		e.ReturnValue = res;
		throw e;
	}
	this->isReadPending = false;

	// Paranoia is good!
	assert(dest[this->readClientLen] == '\0');
	assert(this->readClientLen < len);
	if(this->readClientLen >= len)
		this->readClientLen = len - 1;
	dest[this->readClientLen] = '\0';

	return this->readClientLen; 
}


size_t TcpClientImp::Write(LPCTSTR src, size_t len)
{
	int numLastSent;
	bool hasSentAll;

	size_t totalNumSent;

	hasSentAll = false;
	totalNumSent = 0;
	numLastSent = 0;

	if(!this->isConnected)
	{
//		LogEngineError2(_T("Write to socket aborted, as it is not connected. Server: %s Port: %d\n"), this->serverIpName, this->port);
		return 0;
	}

	//TRACE("Write %s\n", src);

	setOutput(src, len);

	while(!hasSentAll)
	{
		numLastSent = this->wsaIf->send(this->tcpSocket, this->writeBuffer + totalNumSent, (int)(this->numTransmitBytes - totalNumSent) , 0);
		if(numLastSent == 0)
		{
//			LogEngineError2(_T("Write to socket failed, no bytes sent. Server: %s Port: %d\n"), this->serverIpName, this->port);
			return totalNumSent;
		}
		if(numLastSent == SOCKET_ERROR)
		{
			DWORD error = this->wsaIf->WSAGetLastError();
			std::string errorMessage = FormatSystemError(error);
//			LogEngineError3(_T("Write to socket failed: %s. Server: %s Port: %d\n"), errorMessage, this->serverIpName, this->port);
			return totalNumSent;
		}

		if(numLastSent > 0)
		{
			totalNumSent += numLastSent;
		}
		hasSentAll = (totalNumSent == this->numTransmitBytes);
	}
	return totalNumSent;
}


void TcpClientImp::SetServer(char* serverName)
{
	bool toConnectAgain = this->isConnected;
	if(this->isConnected)
		Disconnect();

	strncpy_s(this->serverIpName, 128, serverName, 127);

	if(toConnectAgain) 
		Connect();
}


const char* TcpClientImp::GetServer() const
{
	return this->serverIpName;
}


void TcpClientImp::SetPort(unsigned short port)
{
	bool toConnectAgain = this->isConnected;
	if(this->isConnected)
		Disconnect();

	this->port = port;;

	if(toConnectAgain) 
		Connect();
}


unsigned short TcpClientImp::GetPort() const
{
	return this->port;
}


unsigned int __stdcall TcpClientImp::StatThreadFunc(void* data)
{
	TcpClientImp* thisPtr = (TcpClientImp*) data;

	thisPtr->isThreadRunning = true;

	while(!thisPtr->stopThread)
	{
		if(!thisPtr->isConnected)
		{
			//TRACE("Attempting to connect to %s on port %d\n", pThis->serverIpName, pThis->port);

			thisPtr->Connect(); 
			if(!thisPtr->isConnected)
			{
				SleepEx(1000, 1);   /* Try after one second again*/
			}
		}
		else
		{
			thisPtr->reader();
		}
	}
	_endthreadex(1);
	return 1;
}


void TcpClientImp::reader()
{
	struct timeval readTimeout;
	readTimeout.tv_sec = 2;   // Set 2 second read timeout
	readTimeout.tv_usec = 0;

	this->numReceivedBytes = 0;
	int numReceivedBytes = 0;
	bool hasReceivedAll = false;

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(this->tcpSocket, &fd);
	memset(this->readBuffer, 0, this->bufferSize);

	//if(this->numReceivedBytes != 0)
		//TRACE("!!! this->numReceivedBytes = %d !!!\n", this->numReceivedBytes);
    
	/* Wait 2 seconds for incoming input. Release then.*/
	while(hasReceivedAll == false && this->isConnected == true && this->stopThread == false)
	{
		readTimeout.tv_sec = 2;
		readTimeout.tv_usec = 0;
		FD_ZERO(&fd);
		FD_SET(this->tcpSocket, &fd);

		int isReady = this->wsaIf->select((int)(this->tcpSocket + 1) /*ignored by winsock*/, &fd,  NULL, NULL,  &readTimeout);
		if(isReady < 0)
		{
			//TRACE("select() returned %d.\n", isReady);
			int error = this->wsaIf->WSAGetLastError();
			std::string errorMessage = FormatSystemError(error);
			//TRACE("Error: %d: %s\n", error, errorMessage);
			//TRACE("Closing connection\n", error);
			this->numReceivedBytes = 0;           // No need to process anything
			if(this->isReadPending == true)
				SetEvent(this->onReadEvent);
			Disconnect();
			return;
		}
		
		numReceivedBytes = 0;
		if(FD_ISSET(this->tcpSocket, &fd))
		{
			int res = this->wsaIf->recv(this->tcpSocket, this->readBuffer + this->numReceivedBytes, (int)(this->bufferSize - this->numReceivedBytes - 1), 0);
			Sleep(0);  // Solves problems when debugging.
			if(res > 0)
			{
				// Received a number of bytes.
				numReceivedBytes = res;
				//TRACE("Received %d bytes\n", numReceivedBytes);
			}
			else if(res == 0)
			{
				// Connection was closed gracefully.
				//TRACE(",");
			}
			else
			{
				// Socket error.
				assert(res == SOCKET_ERROR); // No other values < 0 are expected.

				//TRACE("recv() failed.\n");
				int error = this->wsaIf->WSAGetLastError();
				std::string errorMessage = FormatSystemError(error);
				//TRACE("Error: %d: %s\n", error, errorMessage);
				//TRACE("Closing connection\n", error);
				this->numReceivedBytes = 0;           // No need to process anything
				if(this->isReadPending == true)
					SetEvent(this->onReadEvent);
				Disconnect();
				return;
			}
		}
		else
		{
			//TRACE(".");
		}
		if(numReceivedBytes > 0)
		{
			//TRACE("Obs: received %d bytes\n", this->numReceivedBytes);
			this->numReceivedBytes += numReceivedBytes;
			hasReceivedAll = (this->readBuffer[this->numReceivedBytes - 1] == EOT);
			//TRACE("hasReceivedAll: %d\n", (int)hasReceivedAll);
		}
	}

	bool isReadFuncCalled = false;

	if(this->isReadPending == true)
	{
		if(strlen(this->readFilter) > 0)
		{
			//TRACE("Readfilter\n");
			// If the read operation waits for a given string filter it
			if(strncmp(this->readFilter, this->readBuffer + 1, strlen(this->readFilter)) == 0)
			{
				this->readClientLen = stripInput(this->readClientBuffer, this->readClientLen);
				SetEvent(this->onReadEvent);
				isReadFuncCalled = true;
			}
		}
		else
		{
			this->readClientLen = stripInput(this->readClientBuffer, this->readClientLen);
			//TRACE("Stripped len: %d\n", this->readClientLen);
			SetEvent(this->onReadEvent);
			isReadFuncCalled = true;
		}
	}

	if(isReadFuncCalled == true)
	{
		//TRACE("Called receive function\n");
	}
	
	if(isReadFuncCalled == false)
	{
		/*  Go through the list of callbacks and call a desired callback. */
		list<CallBackEntry>::iterator it;
		for(it = this->callbackList.begin(); it != this->callbackList.end(); ++it)
		{
			size_t bufferSize;
			char* destBuffer;

			if(!strncmp(it->Filter, this->readBuffer + 1, strlen(it->Filter)))
			{
				destBuffer = it->Buffer;
				bufferSize = it->BufferSize;
				
				/* Strip the input from special characters and write it to a new buffer */
				bufferSize = stripInput(destBuffer, bufferSize);

				if(it->CallbackObj != NULL && it->CallbackFunc != NULL)
				{
				          /*
					       To call a member function of a class we need two things:
					             1) The address of the function       ptr_to_function
					             2) The address of the object         ptr_to_object

					            (ptr_to_object->*ptr_to_function) (argument_list)
					         */
					it->CallbackMember(it->CallbackObj, destBuffer, bufferSize);

				}
				else if(it->CallbackFunc != NULL)
				{
					it->CallbackFunc(destBuffer, bufferSize);					
				}
				else
				{
					this->callbackList.erase(it);
				}
				
			}
		}
	}
}


size_t TcpClientImp::stripInput(char* dest, size_t dest_size)
{
	int in;
	size_t out;

	in = 1;
	out = 0;
	memset(dest, 0, dest_size);
	while(in < this->numReceivedBytes - 1 && out < dest_size - 1)
	{
		if(this->readBuffer[in] != ESC)
		{                // ESC character
			dest[out++] = this->readBuffer[in++];
		}
		else
		{
			in++;
			dest[out++] = ~this->readBuffer[in++];
		}
	}
	if(in < this->numReceivedBytes)
	{
//		LogEngineError5(
//			_T("Socket: received data exceeds buffer size. Truncating. (Host: %s, port: %d, data size: %d, consumed data: %d, buffer size: %d)"),
//			this->serverIpName, this->port, this->numReceivedBytes, in, dest_size);
	}
	dest[out] = '\0';
	return out;	
}


size_t TcpClientImp::setOutput(const char* src, size_t src_len)
{
	const char special[] = {SOH, EOT, ESC, 0}; // 0 is not special, it is an indicator for end of string
	char ch;
	size_t i;

	this->numTransmitBytes = 0; // Number of bytes in the transmit buffer
	this->writeBuffer[this->numTransmitBytes++] = SOH;

	// Escape special characters.
	for(i = 0; (i < src_len) && (this->numTransmitBytes < this->bufferSize - 2); i++)
	{
		ch = src[i];

		if(NULL != strchr(special, ch))
		{
			this->writeBuffer[this->numTransmitBytes++] = ESC;
			ch = ~ch;
		}
		this->writeBuffer[this->numTransmitBytes++] = ch;
	}
	this->writeBuffer[this->numTransmitBytes++] = EOT;
	return this->numTransmitBytes;
}


std::string TcpClientImp::FormatWaitError(DWORD errorCode)
{
	std::string messageString;
	std::string systemErrorString;
	switch(errorCode)
	{
	case WAIT_ABANDONED:
		messageString = _T("WAIT_ABANDONED");
		break;
	case WAIT_OBJECT_0:
		messageString = _T("WAIT_OBJECT_0");
		break;
	case WAIT_TIMEOUT:
		messageString = _T("WAIT_TIMEOUT");
		break;
	case WAIT_FAILED:
		systemErrorString = FormatSystemError(GetLastError());
		messageString=_T("WAIT_FAILED : ") + systemErrorString;
		break;
	default:
    std::ostringstream s; 
    s << "Unknown error: " << errorCode << std::ends; 
    messageString=s.str();
		break;
	}
	return messageString;
}


std::string TcpClientImp::FormatSystemError(DWORD error) const
{
	LPVOID messageBuffer;
	DWORD res = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &messageBuffer,
		0, NULL );
	std::string formattedString;
	if(res == 0)
	{
    // FormatMessage failed.
    std::ostringstream s; 
    s << "***" << error << "***" << std::ends; 
    formattedString=s.str();
	}
	else
	{
		formattedString = (LPTSTR)messageBuffer;
		LocalFree(messageBuffer);
	}
	return formattedString;
}
