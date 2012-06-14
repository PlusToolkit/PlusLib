#include "stdafx.h"

#include <assert.h>
#include <list>

#include "AcquisitionInjector.h"
#include "AcquisitionSettings.h"
#include "ResearchInterface.h"

/// <summary>   Implementation of the functions and private variables of the AcquisitionInjector. </summary>
class AcquisitionInjectorImpl
{
public:
    AcquisitionInjectorImpl() : dataReceivers(0), injectData(false)
    {
    }
    ~AcquisitionInjectorImpl()
    {
    }

    bool InjectData(int lines, int pitch, void const* frameData)
    {
        bool result = true;
        if(this->injectData)
        {
            std::list<IAcquisitionDataReceiver*>::iterator iter = dataReceivers.begin();
            for( ; iter != dataReceivers.end(); ++iter)
            {
                result &= (*iter)->DataAvailable(lines, pitch, frameData);
            }
        }
        return result;
    }


    bool PrepareInjection(int samples, int lines, int pitch)
    {
        this->injectData = true;
        bool result = true;
        std::list<IAcquisitionDataReceiver*>::iterator iter = dataReceivers.begin();
        for( ; iter != dataReceivers.end(); ++iter)
        {
            result &= (*iter)->Prepare(samples, lines, pitch);
        }
        return result;
    }

    bool CleanupInjection()
    {
        this->injectData = false;
        bool result = true;
        std::list<IAcquisitionDataReceiver*>::iterator iter = dataReceivers.begin();
        for( ; iter != dataReceivers.end(); ++iter)
        {
            result &= (*iter)->Cleanup();
        }
        return result;
    }

    void AddDataReceiver(IAcquisitionDataReceiver* dataReceiver)
    {
        dataReceivers.push_back(dataReceiver);
    }

    void RemoveDataReceiver(IAcquisitionDataReceiver* dataReceiver)
    {
        dataReceivers.remove(dataReceiver);
    }

    void StopInjection()
    {
        this->injectData = false;
    }

private:
    std::list<IAcquisitionDataReceiver*> dataReceivers;
    bool injectData;

};




AcquisitionInjector::AcquisitionInjector()
{
    impl = new AcquisitionInjectorImpl();
}

AcquisitionInjector::~AcquisitionInjector()
{
    delete impl;
}


bool AcquisitionInjector::InjectData(int lines, int pitch, void const* frameData) 
{
    return impl->InjectData(lines, pitch, frameData);
}


bool AcquisitionInjector::PrepareInjection(int samples, int lines, int pitch)
{
    return impl->PrepareInjection(samples, lines, pitch);
}

bool AcquisitionInjector::CleanupInjection()
{
    return impl->CleanupInjection();
}

void AcquisitionInjector::AddDataReceiver(IAcquisitionDataReceiver* dataReceiver)
{
    impl->AddDataReceiver(dataReceiver);
}

void AcquisitionInjector::RemoveDataReceiver(IAcquisitionDataReceiver* dataReceiver)
{
    impl->RemoveDataReceiver(dataReceiver);
}

void AcquisitionInjector::StopInjection()
{
    impl->StopInjection();
}
