// Minimal test for Windows SDK compatibility

#include <Mfapi.h>

int main()
{
  
  // We use these constants and they are only defined in Windows SDK 7.1 and later
  GUID guid1=MF_MT_FRAME_RATE_RANGE_MIN;
  GUID guid2=MF_MT_FRAME_RATE_RANGE_MAX;
  
  return 0;
}
