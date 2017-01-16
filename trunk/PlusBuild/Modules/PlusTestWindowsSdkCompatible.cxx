// Minimal test for Windows SDK compatibility

#include <Mfapi.h>

#pragma comment(lib, "evr.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Strmiids.lib")

int main()
{
  
  // We use these constants and they are only defined in Windows SDK 7.1 and later
  GUID guid1=MF_MT_FRAME_RATE_RANGE_MIN;
  GUID guid2=MF_MT_FRAME_RATE_RANGE_MAX;
  
  return 0;
}
