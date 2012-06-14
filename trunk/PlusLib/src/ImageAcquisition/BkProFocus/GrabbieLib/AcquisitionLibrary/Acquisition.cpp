#include "stdafx.h"

#include "Acquisition.h"

/// <summary>   Intended for use with CommandAndControl. Currently unused. </summary>
class AcquisitionImpl
{
public:
	AcquisitionImpl();
	virtual ~AcquisitionImpl();

private:
};


// ----------------------------------------------------------------------------
//
//    Command And Control - Private implementation
//
// ----------------------------------------------------------------------------



AcquisitionImpl::AcquisitionImpl()
{

}

AcquisitionImpl::~AcquisitionImpl()
{

}


// ----------------------------------------------------------------------------
// 
//    Command and Control class
//    
// ----------------------------------------------------------------------------

Acquisition::Acquisition():
impl(new AcquisitionImpl)
{

}



Acquisition::~Acquisition()
{
	delete impl;
}


