#include "SEIDrv.h"
#include <string>
#include <Windows.h> // for QueryDosDevice
#include <chrono>

ref struct Globals
{
    static System::Collections::Generic::List<USDigital::A2^> encoders;
    static System::Collections::Generic::Dictionary<unsigned char, USDigital::A2^> addresses;
    static unsigned initialized = 0; // how many times initialization was called
    static long activeCOM = 0;
};

void enumerateEncoders(long comPort, long mode, int devicesExpected)
{
    System::String^ comPortString = "COM" + System::Convert::ToString(comPort);

    USDigital::SEIBusManager mgr;
    USDigital::SEIBus^ mSEIBus = mgr.GetBus(comPortString);
    mSEIBus->ScanSettings->LimitDeviceCount = devicesExpected;
    mSEIBus->Initialize((USDigital::InitializationFlags)mode);

    for (int i = 0; i < mSEIBus->Devices->Length; i++)
    {
        USDigital::A2^ a2Dev = (USDigital::A2^)mSEIBus->Devices[i];
        Globals::encoders.Add(a2Dev);
        Globals::addresses.Add(a2Dev->Address, a2Dev);
    }
    Globals::activeCOM = comPort;
}

void enumerateEncodersAll(long mode, int devicesExpected)
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
            try
            {
                enumerateEncoders(i, mode, devicesExpected);
            }
            catch (System::UnauthorizedAccessException^)
            {
                // port is open by somebody else, ignore it
            }
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

long InitializeSEI(long comm, long mode, int devicesExpected)
{
    if (Globals::initialized > 0)
    {
        ++Globals::initialized; // just increase the count
        return 0;
    }

    try
    {
        int mRetryLimit = 5;
        int retryCount = 0;
        for (int retryCount = 0; retryCount < mRetryLimit; retryCount++)
        {
            if (comm == 0)
            {
                enumerateEncodersAll(mode, devicesExpected);
            }
            else
            {
                enumerateEncoders(comm, mode, devicesExpected);
            }
            if(GetNumberOfDevices() != devicesExpected && retryCount < mRetryLimit)
            {
                System::Console::WriteLine("SEI Retrying...Current number of devices: {0}", GetNumberOfDevices());
                Globals::encoders.Clear();
                Globals::addresses.Clear();
                Sleep(30);
            }
            else
            {
                break;
            }
        }
        Globals::initialized = 1;
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
    --Globals::initialized;
    if (Globals::initialized == 0) // clean up for real
    {
        Globals::encoders.Clear();
        Globals::addresses.Clear();
    }

    System::String^ comPortString = "COM" + System::Convert::ToString(Globals::activeCOM);
    USDigital::SEIBusManager mgr;
    USDigital::SEIBus^ mSEIBus = mgr.GetBus(comPortString);
    mSEIBus->Close();
    Globals::activeCOM = 0;
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
