#include <tchar.h> // for _T
#include <sstream>
#include <vector>

#include "PlusCommon.h"

#include "ResearchInterface.h"

ResearchInterface::ResearchInterface() : researchInterfaceCommunicator(), errorText(_T(""))
{
  // do nothing
}

ResearchInterface::~ResearchInterface()
{
  // do nothing
}

bool ResearchInterface::OpenConnection(const AcquisitionSettings& acquisitionSettings)
{
  // try to open communication with the research interface
  // The old dalsa cards registrered port number (usually 3), while PC Express cards register a name
  int result;
  if (acquisitionSettings.GetComportNumber() != -1)
  {
    result = researchInterfaceCommunicator.OpenSerial(acquisitionSettings.GetComportNumber(), acquisitionSettings.GetBaudRate());
  }
  else
  {
    std::string comPortName=acquisitionSettings.GetComportName();
    result = researchInterfaceCommunicator.OpenSerial(comPortName.c_str(), acquisitionSettings.GetBaudRate());
  }

  // communication failed
  if (result == FALSE)
  {
    std::ostringstream error;
    if (acquisitionSettings.GetComportNumber() != -1)
    {
      error << ": Cannot open com port no " << acquisitionSettings.GetComportNumber() << std::ends; 
    }
    else
    {
      error << ": Cannot open com port called " << acquisitionSettings.GetComportName() << std::ends; 
    }
    errorText += std::string(__FUNCTION__) + error.str();
    return false;
  }
  return true;
}

bool ResearchInterface::SetupConnection(const AcquisitionSettings& acquisitionSettings)
{
  std::string reply = _T("");
  std::string currentCommand = _T("");

  std::string setupCommands=acquisitionSettings.GetSetupCommands();
  
  // split the command, using ';' as separator
  const char *str=setupCommands.c_str();
  std::vector<std::string> setupCommandList;
  const char separator=';';
  while(true)
  {
    const char *begin = str;
    while(*str != separator && *str != 0)
    {
      str++;
    }
    setupCommandList.push_back(std::string(begin, str));
    if(0 == *str++)
    {
      break;
    }
  }
  
  for (std::vector<std::string>::iterator currentCommandIt=setupCommandList.begin(); currentCommandIt!=setupCommandList.end(); currentCommandIt++)
  {        
    reply = _T("");
    PlusCommon::Trim(*currentCommandIt);
    if(!this->SendCommand((*currentCommandIt)+ _T("\r"), &reply))
    {
      std::string error = std::string(__FUNCTION__) + _T(": Failed sending command \"") 
        + (*currentCommandIt) + _T("\" in sequence \"")
        + acquisitionSettings.GetSetupCommands() + _T("\" to research interface\n");
      errorText+=error;
      return false;
    }
  }
  return true;
}

bool ResearchInterface::CloseConnection()
{
  return researchInterfaceCommunicator.CloseSerial() == TRUE;
}

bool ResearchInterface::SendCommand(const std::string& command, std::string* reply)
{
  if (command.empty())
  {
    errorText +=  std::string(__FUNCTION__) + std::string(_T(": Empty command\n"));
    return false;
  }

  // send data to the research interface
  for (int i = 0; i < command.size(); i++)
  {
    researchInterfaceCommunicator.PutSerial(command[i]);
  }

  // give it a little time to react
  Sleep(50);

  unsigned char ch;
  while( researchInterfaceCommunicator.GetSerial(&ch) )
  {
    (*reply) += ch;
  }

  // get the last char in reply that contain a status message from the Research Interface
  if(reply->size() <= 0)
  {
    // no replies received
    errorText += std::string(__FUNCTION__) + std::string(_T(": No replies received\n"));
    return false;
  }
  ch = (*reply)[reply->size()-1];
  if (ch=='$')
  {
    // R.I. returned as expected
    return true;
  }
  else if (ch=='!')
  {
    // R.I. encountered an error
    errorText += std::string(__FUNCTION__) + std::string(_T(": Research Interface encountered an error. Reply: " + *reply + "\n"));
    return false;
  }
  else
  {
    // R.I. sent us some data we do not understand
    errorText += std::string(__FUNCTION__) + std::string(_T(": Received unknown response. Reply: " + *reply + "\n"));
    return true;
  }
}

std::string ResearchInterface::GetError() const
{
  return errorText;
}

void ResearchInterface::ResetError()
{
  errorText.clear();
}
