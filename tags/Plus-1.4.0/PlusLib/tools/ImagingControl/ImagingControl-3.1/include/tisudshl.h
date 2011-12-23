#ifndef TISUDSHL_H
#define TISUDSHL_H

#include <string>
#include <cassert>
#include <vector>
#include <shlwapi.h>

#pragma warning(disable:4100) // warning C4100: unreferenced formal parameter

// DLL public interface 
#include "libbasedefs.h"
#include "udshl_defs.h"

#include "dshow_header.h"

// linker helper, to bind to the UDSHL
#include "udshl_lnk.h"

#include "smart_ptr.h"
#include "smart_com.h"

#include "simplectypes.h"

#include "int_interface_pre.h"

#include "VideoFormatItem.h"
#include "VideoNormItem.h"
#include "VideoCaptureDeviceItem.h"
#include "AnalogChannelItem.h"

#include "Error.h"
#include "Grabber.h"
#include "MemBufferCollection.h"
#include "MemBuffer.h"
#include "Framegrabbersink.h"
#include "AviSink.h"
#include "OverlayBitmap.h"
#include "GrabberListener.h"

#include "FrameHandlerSink.h"
#include "MediaStreamSink.h"

#include "IVCDProperty.h"
#include "IVCDPropertyInterfaces.h"
#include "VCDPropertyDef.h"

#include "VCDPropertyID.h"
#include "VCDPropertyIDTIS.h"

#include "FilterLoader.h"
#include "FrameFilter.h"
#include "FrameFilterImpl.h"

#endif /* TISUDSHL_H */
