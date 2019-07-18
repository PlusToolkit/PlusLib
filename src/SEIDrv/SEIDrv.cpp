#include "SEIDrv.h"
#include <string>
#include <Windows.h> // for QueryDosDevice

ref struct Globals
{
    static System::Collections::Generic::List<USDigital::A2^> encoders;
    static System::Collections::Generic::Dictionary<unsigned char, USDigital::A2^> addresses;
    static System::Collections::Generic::List<unsigned> counts;
    static bool initialized = false;
};

void enumerateEncoders(long comPort, long mode)
{
    System::String^ comPortString = "COM"+ System::Convert::ToString(comPort);

    USDigital::SEIBusManager mgr;
    USDigital::SEIBus^ mSEIBus = mgr.GetBus(comPortString);
    mSEIBus->Initialize((USDigital::InitializationFlags)mode);

    for (int i = 0; i < mSEIBus->Devices->Length; i++)
    {
        USDigital::A2^ a2Dev = (USDigital::A2^)mSEIBus->Devices[i];
        Globals::encoders.Add(a2Dev);
        Globals::addresses.Add(a2Dev->Address, a2Dev);
        Globals::counts.Add(a2Dev->Count);
    }
}

void enumerateEncodersAll(long mode)
{
    const unsigned bufferSize = 5000;
    char lpTargetPath[bufferSize]; // buffer to store the path of the COMPORTS
    for (long i = 0; i < 255; i++)
    {
        std::string str = "COM" + std::to_string(i);
        DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, bufferSize);

        if (test != 0) // port exists
        {
            //std::cout << str << ": " << lpTargetPath << std::endl;
            enumerateEncoders(i, mode);
        }
    }
}

#define CatchAndReturn1 \
catch (System::Exception^ exc) \
{ \
    System::Console::WriteLine("A managed exception occurred in InitializeSEI!\n{0}", exc->ToString()); \
} \
catch (...) \
{ \
    System::Console::WriteLine("An unknown exception occurred in InitializeSEI!"); \
} \
return 1

long InitializeSEI(long comm, long mode)
{
    try
    {
        if (comm == 0)
        {
            enumerateEncodersAll(mode);
        }
        else
        {
            enumerateEncoders(comm, mode);
        }
        Globals::initialized = true;
        return 0;
    }
    CatchAndReturn1;
}

bool IsInitialized()
{
    return Globals::initialized;
}

long GetNumberOfDevices()
{
    return Globals::encoders.Count;
}

long GetDeviceInfo(long devnum, long& serialnum, long& addr, char* model, char* firmwareVersion)
{
    try
    {
        USDigital::A2^ dev = Globals::encoders[devnum];
        addr = dev->Address;
        serialnum = dev->SerialNumber;
        if (model != nullptr)
        {
            char* temp = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dev->Model).ToPointer();
            strcpy(model, temp);
            System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)temp);
        }
        if (firmwareVersion != nullptr)
        {
            char* temp = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(dev->Model).ToPointer();
            strcpy(firmwareVersion, temp);
            System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)temp);
        }
        return 0;
    }
    CatchAndReturn1;
}

void CloseSEI()
{
    Globals::encoders.Clear();
    Globals::addresses.Clear();
    Globals::counts.Clear();
    Globals::initialized = false;
}


long A2SetStrobe()
{
    try
    {
        if (Globals::encoders.Count > 0)
            if (Globals::encoders[0]->SEIBus->Strobe())
                return 0;
    }
    CatchAndReturn1;
}

long A2SetSleep()
{
    return 0; // sleeping not exposed in USDigital .net library
}

long A2SetWakeup()
{
    return 0; // sleeping not exposed in USDigital .net library
}

long A2SetOrigin(long address)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            if (dev->SetOrigin())
                return 0;
        }
    }
    CatchAndReturn1;
}

long A2GetMode(long address, long * mode)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            unsigned char value;
            bool success = dev->GetCounterMode(value);
            if (success)
            {
                *mode = value;
                return 0;
            }
        }
    }
    CatchAndReturn1;
}

long A2SetMode(long address, long mode)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            if (dev->SetCounterMode(mode))
                return 0;
        }
    }
    CatchAndReturn1;
}

long A2GetResolution(long address, long * res)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            unsigned int value;
            bool success = dev->GetResolution(value);
            if (success)
            {
                *res = value;
                return 0;
            }
        }
    }
    CatchAndReturn1;
}

long A2SetResolution(long address, long res)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            if (dev->SetCounterMode(res))
                return 0;
        }
    }
    CatchAndReturn1;
}

long A2GetPosition(long address, long * pos)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            // The GetCount(count) signature does not work properly
            // so we use GetCount(count, status).
            USDigital::A2::A2EncoderStatus status;
            unsigned int value;
            bool success = dev->GetCount(value, status);
            if (success)
            {
                *pos = value;
                return 0;
            }
        }
    }
    CatchAndReturn1;
}

long A2SetPosition(long address, long pos)
{
    try
    {
        USDigital::A2^ dev;
        if (Globals::addresses.TryGetValue(address, dev))
        {
            if (dev->SetCount(pos))
                return 0;
        }
    }
    CatchAndReturn1;
}
