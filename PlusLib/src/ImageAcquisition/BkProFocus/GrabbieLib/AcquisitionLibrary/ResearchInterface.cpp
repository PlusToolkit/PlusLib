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
        const char* comportName = acquisitionSettings.GetComportName();
        result = researchInterfaceCommunicator.OpenSerial(comportName, acquisitionSettings.GetBaudRate());
        acquisitionSettings.GetComportName().ReleaseBuffer();
    }

    // communication failed
    if (result == FALSE)
    {
        std::string error;
        if (acquisitionSettings.GetComportNumber() != -1)
        {
            error.Format(_T(": Cannot open com port no %d"), acquisitionSettings.GetComportNumber());
        }
        else
            error.Format(_T(": Cannot open com port called %s"), acquisitionSettings.GetComportName());
        errorText.Append(std::string(__FUNCTION__) + error);
        return false;
    }
    return true;
}

bool ResearchInterface::SetupConnection(const AcquisitionSettings& acquisitionSettings)
{
    std::string reply = _T("");
    std::string currentCommand = _T("");
    int tokenIndex = 0;
    while(AfxExtractSubString(currentCommand, acquisitionSettings.GetSetupCommands(), tokenIndex, ';'))
    {
        reply = _T("");
        currentCommand.Trim();
        if(!this->SendCommand(currentCommand + _T("\r"), &reply))
        {
            std::string error = std::string(__FUNCTION__) + _T(": Failed sending command \"") 
                + currentCommand + _T("\" in sequence \"")
                + acquisitionSettings.GetSetupCommands() + _T("\" to research interface\n");
            errorText.Append(error);
            return false;
        }
        ++tokenIndex;
    }
    return true;
}

bool ResearchInterface::CloseConnection()
{
    return researchInterfaceCommunicator.CloseSerial() == TRUE;
}

bool ResearchInterface::SendCommand(const std::string& command, std::string* reply)
{
    if (command.IsEmpty())
    {
        errorText.Append(std::string(__FUNCTION__) + std::string(_T(": Empty command\n")));
        return false;
    }

    // send data to the research interface
    for (int i = 0; i < command.GetLength(); i++)
    {
        researchInterfaceCommunicator.PutSerial(command[i]);
    }

    // give it a little time to react
    Sleep(50);

    unsigned char ch;
    while( researchInterfaceCommunicator.GetSerial(&ch) )
    {
        reply->AppendChar(ch);
    }

    // get the last char in reply that contain a status message from the Research Interface
    if(reply->GetLength() <= 0)
    {
        // no replies received
        errorText.Append(std::string(__FUNCTION__) + std::string(_T(": No replies received\n")));
        return false;
    }
    ch = (*reply)[reply->GetLength()-1];
    if (ch=='$')
    {
        // R.I. returned as expected
        return true;
    }
    else if (ch=='!')
    {
        // R.I. encountered an error
        errorText.Append(std::string(__FUNCTION__) + std::string(_T(": Research Interface encountered an error. Reply: " + *reply + "\n")));
        return false;
    }
    else
    {
        // R.I. sent us some data we do not understand
        errorText.Append(std::string(__FUNCTION__) + std::string(_T(": Received unknown response. Reply: " + *reply + "\n")));
        return true;
    }
}

std::string ResearchInterface::GetError() const
{
    return errorText;
}

void ResearchInterface::ResetError()
{
    errorText.Empty();
}
