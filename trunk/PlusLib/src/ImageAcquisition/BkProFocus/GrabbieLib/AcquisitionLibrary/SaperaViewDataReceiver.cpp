#pragma once

// as sapera comes with its own version of stdint.h (in cordef.h), the macro redefinition warnings are momentarily disabled
#pragma warning(push)
#pragma warning (disable: 4005)
#include "SapClassBasic.h"
#pragma warning(pop)

#include "SaperaViewDataReceiver.h"

SaperaViewDataReceiver::SaperaViewDataReceiver(SapBuffer* saperaBuffer)
{
    sapView = new SapView(saperaBuffer, SapHwndAutomatic);
}

SaperaViewDataReceiver::~SaperaViewDataReceiver()
{
    delete sapView;
}

bool SaperaViewDataReceiver::DataAvailable(int /*lines*/, int /*pitch*/, void const* /*frameData*/)
{
    sapView->ShowNext();
    return true;
}

bool SaperaViewDataReceiver::Prepare(int /*samples*/, int /*lines*/, int /*pitch*/)
{
    int result = sapView->Create();
    return result == TRUE;
}

bool SaperaViewDataReceiver::Cleanup() 
{
    int result = sapView->Destroy();
    return result == TRUE;
}