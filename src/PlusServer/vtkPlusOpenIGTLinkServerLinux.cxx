#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <stdio.h>

void PrintServerInfo(vtkPlusOpenIGTLinkServer* self)
{
  struct ifaddrs* ifap, *ifa;
  struct sockaddr_in* sa;
  char* addr;

  std::vector<std::string> addresses;

  getifaddrs(&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr->sa_family == AF_INET)
    {
      sa = (struct sockaddr_in*) ifa->ifa_addr;
      addr = inet_ntoa(sa->sin_addr);
      addresses.push_back(addr);
    }
  }
  freeifaddrs(ifap);

  std::stringstream ss;
  ss << "Plus OpenIGTLink server listening on IPs: ";
  for (unsigned int i = 0; i < addresses.size(); ++i)
  {
    ss << addresses[i];
    if (i + 1 != addresses.size())
    {
      ss << ", ";
    }
  }
  ss << " -- port " << self->GetListeningPort();
  LOG_INFO(ss.str());
}