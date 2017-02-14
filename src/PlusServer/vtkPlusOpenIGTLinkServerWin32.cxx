#include <WS2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

void PrintServerInfo(vtkPlusOpenIGTLinkServer* self)
{
  DWORD dwRetVal = 0;
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  ULONG outBufferSize = 0;
  ULONG iterations = 0;
  DWORD ipv4StringBufferLength = 63;

  // Allocate a 15 KB buffer to start with.
  outBufferSize = 15 * 1024;

  do
  {
    pAddresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, (outBufferSize));

    if (pAddresses == NULL)
    {
      return;
    }

    dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufferSize);

    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
      HeapFree(GetProcessHeap(), 0, (pAddresses));
      pAddresses = NULL;
    }
    else
    {
      break;
    }

    iterations++;
  }
  while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (iterations < 3));

  std::vector<std::string> addresses;

  if (dwRetVal == NO_ERROR)
  {
    PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
    while (pCurrAddresses)
    {
      PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
      if (pUnicast != NULL)
      {
        for (unsigned int i = 0; pUnicast != NULL; i++)
        {
          sockaddr_in* sa_in = (sockaddr_in*)pUnicast->Address.lpSockaddr;
          std::string address = inet_ntoa(sa_in->sin_addr);
          addresses.push_back(address);
          pUnicast = pUnicast->Next;
        }
      }

      pCurrAddresses = pCurrAddresses->Next;
    }
  }

  if (pAddresses)
  {
    HeapFree(GetProcessHeap(), 0, (pAddresses));
  }

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