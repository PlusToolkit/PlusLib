#include "WSAIF.h"
#include "WinSock.h"

//////////////////////////////////////////////////////////////////////////
//
//
// WSAIF Implementation
//
//
//////////////////////////////////////////////////////////////////////////

int WSAIF::closesocket(SOCKET s)
{
	return ::closesocket(s);
}
int WSAIF::connect(SOCKET s, const struct sockaddr FAR * name, int namelen)
{
	return ::connect(s, name, namelen);
}
struct hostent FAR * WSAIF::gethostbyname(const char FAR * name)
{
	return ::gethostbyname(name);
}
struct hostent FAR * WSAIF::gethostbyaddr(const char FAR * addr, int len, int type)
{
	return ::gethostbyaddr(addr, len, type);
}
u_short WSAIF::htons(u_short hostshort)
{
	return ::htons(hostshort);
}
int WSAIF::recv(SOCKET s, char FAR* buf, int len, int flags)
{
	return ::recv(s, buf, len, flags);
}
int WSAIF::select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, const struct timeval FAR * timeout)
{
	return ::select(nfds, readfds, writefds, exceptfds, timeout);
}
int WSAIF::send(SOCKET s, const char FAR * buf, int len, int flags)
{
	return ::send(s, buf, len, flags);
}
int WSAIF::setsockopt(SOCKET s, int level, int optname, const char FAR * optval, int optlen)
{
	return ::setsockopt(s, level, optname, optval, optlen);
}
SOCKET WSAIF::socket(int af, int type, int protocol)
{
	return ::socket(af, type, protocol);
}
int WSAIF::WSACleanup()
{
	return ::WSACleanup();
}
int WSAIF::WSAGetLastError()
{
	return ::WSAGetLastError();
}
int WSAIF::WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData)
{
	return ::WSAStartup(wVersionRequested, lpWSAData);
}
