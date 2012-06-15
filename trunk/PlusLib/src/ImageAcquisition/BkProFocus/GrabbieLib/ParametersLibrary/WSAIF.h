#pragma once

#include "Winsock.h" // for SOCKET

class WSAIF
{
public:
	//////////////////////////////////////////////////////////////////////////
	//  WinSock wrappers
	//////////////////////////////////////////////////////////////////////////
	virtual int closesocket(SOCKET s);
	virtual int connect(SOCKET s, const struct sockaddr FAR * name, int namelen);
	virtual struct hostent FAR * gethostbyaddr(const char FAR * addr, int len, int type);
	virtual struct hostent FAR * gethostbyname(const char FAR * name);
	virtual u_short htons(u_short hostshort);
	virtual int recv(SOCKET s, char FAR* buf, int len, int flags);
	virtual int select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, const struct timeval FAR * timeout);
	virtual int send(SOCKET s, const char FAR * buf, int len, int flags);
	virtual int setsockopt(SOCKET s, int level, int optname, const char FAR * optval, int optlen);
	virtual SOCKET socket(int af, int type, int protocol);
	virtual int WSACleanup();
	virtual int WSAGetLastError();
	virtual int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
};

