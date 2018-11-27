/*=Plus=header=begin======================================================
 Program: Plus
 Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
 See License.txt for details.
 =========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusV4L2VideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

// OS includes
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusV4L2VideoSource);

//----------------------------------------------------------------------------

namespace
{
  int xioctl(int fh, unsigned long int request, void* arg)
  {
    int r;

    do
    {
      r = ioctl(fh, request, arg);
    }
    while (-1 == r && EINTR == errno);

    return r;
  }
}

#define CLEAR(x) memset(&(x), 0, sizeof(x))

//----------------------------------------------------------------------------
vtkPlusV4L2VideoSource::vtkPlusV4L2VideoSource()
  : DeviceName("")
  , IOMethod(IO_METHOD_READ)
  , FileDescriptor(-1)
  , FrameBuffers(nullptr)
  , BufferCount(0)
  , DeviceFormat(std::make_shared<v4l2_format>())
  , FormatWidth(nullptr)
  , FormatHeight(nullptr)
  , PixelFormat(nullptr)
  , FieldOrder(nullptr)
  , DataSource(nullptr)
{
  memset(this->DeviceFormat.get(), 0, sizeof(struct v4l2_format));

  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusV4L2VideoSource::~vtkPlusV4L2VideoSource()
{
}

//----------------------------------------------------------------------------
void vtkPlusV4L2VideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "DeviceName: " << this->DeviceName << std::endl;
  os << indent << "IOMethod: " << this->IOMethodToString(this->IOMethod) << std::endl;
  os << indent << "BufferCount: " << this->BufferCount << std::endl;

  if (this->FileDescriptor != -1)
  {
    os << indent << "Available formats: " << std::endl;

    struct v4l2_fmtdesc fmtdesc;
    CLEAR(fmtdesc);
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while (ioctl(this->FileDescriptor, VIDIOC_ENUM_FMT, &fmtdesc) == 0)
    {
      os << indent << fmtdesc.description << std::endl;
      fmtdesc.index++;
    }
  }
  else
  {
    os << indent << "Cannot enumerate known formats. Camera not connected." << std::endl;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(DeviceName, deviceConfig);
  std::string ioMethod;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(IOMethod, ioMethod, deviceConfig);
  if (vtkPlusV4L2VideoSource::StringToIOMethod(ioMethod) != IO_METHOD_UNKNOWN)
  {
    this->IOMethod = vtkPlusV4L2VideoSource::StringToIOMethod(ioMethod);
  }
  else
  {
    LOG_WARNING("Unknown method: " << ioMethod << ". Defaulting to " << vtkPlusV4L2VideoSource::IOMethodToString(this->IOMethod));
  }

  int frameSize[2];
  XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 2, FrameSize, frameSize, deviceConfig);
  if (deviceConfig->GetAttribute("FrameSize") != nullptr)
  {
    if (frameSize[0] < 0 || frameSize[1] < 0)
    {
      LOG_WARNING("Invalid frame size in configuration file. Not setting parameters to device.");
    }
    else
    {
      this->FormatHeight = std::make_shared<unsigned int>(static_cast<unsigned int>(frameSize[0]));
      this->FormatWidth = std::make_shared<unsigned int>(static_cast<unsigned int>(frameSize[1]));
    }
  }

  std::string pixelFormat;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(PixelFormat, pixelFormat, deviceConfig);
  if (deviceConfig->GetAttribute("PixelFormat") != nullptr)
  {
    this->PixelFormat = std::make_shared<unsigned int>(vtkPlusV4L2VideoSource::StringToPixelFormat(pixelFormat));
  }

  std::string fieldOrder;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(FieldOrder, fieldOrder, deviceConfig);
  if (deviceConfig->GetAttribute("FieldOrder") != nullptr)
  {
    this->FieldOrder = std::make_shared<v4l2_field>(vtkPlusV4L2VideoSource::StringToFieldOrder(fieldOrder));
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DeviceName, deviceConfig);

  deviceConfig->SetAttribute("IOMethod", vtkPlusV4L2VideoSource::IOMethodToString(this->IOMethod).c_str());

  int frameSize[2] = { static_cast<int>(this->DeviceFormat->fmt.pix.width), static_cast<int>(this->DeviceFormat->fmt.pix.height) };
  deviceConfig->SetVectorAttribute("FrameSize", 2, frameSize);

  deviceConfig->SetAttribute("PixelFormat", vtkPlusV4L2VideoSource::PixelFormatToString(this->DeviceFormat->fmt.pix.pixelformat).c_str());

  deviceConfig->SetAttribute("FieldOrder", vtkPlusV4L2VideoSource::FieldOrderToString(static_cast<v4l2_field>(this->DeviceFormat->fmt.pix.field)).c_str());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InitRead(unsigned int bufferSize)
{
  this->FrameBuffers = (FrameBuffer*) calloc(1, sizeof(FrameBuffer));

  if (!this->FrameBuffers)
  {
    LOG_ERROR("Unable to allocate " << sizeof(FrameBuffer) << " bytes for capture frame.");
    return PLUS_FAIL;
  }

  this->FrameBuffers[0].length = bufferSize;
  this->FrameBuffers[0].start = malloc(bufferSize);

  if (!this->FrameBuffers[0].start)
  {
    LOG_ERROR("Unable to allocate " << bufferSize << " bytes for capture frame.");
    return PLUS_FAIL;
  }

  this->BufferCount = 1;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InitMmap()
{
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_REQBUFS, &req))
  {
    if (EINVAL == errno)
    {
      LOG_ERROR(this->DeviceName << " does not support memory mapping");
    }
    else
    {
      LOG_ERROR("VIDIOC_REQBUFS" << ": " << strerror(errno));
    }
    return PLUS_FAIL;
  }

  if (req.count < 2)
  {
    LOG_ERROR("Insufficient buffer memory on " << this->DeviceName);
    return PLUS_FAIL;
  }

  this->FrameBuffers = (FrameBuffer*) calloc(req.count, sizeof(FrameBuffer));

  if (!this->FrameBuffers)
  {
    LOG_ERROR("Out of memory");
    return PLUS_FAIL;
  }

  for (this->BufferCount = 0; this->BufferCount < req.count; ++this->BufferCount)
  {
    v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = this->BufferCount;

    if (-1 == xioctl(this->FileDescriptor, VIDIOC_QUERYBUF, &buf))
    {
      LOG_ERROR("VIDIOC_QUERYBUF" << ": " << strerror(errno));
      return PLUS_FAIL;
    }

    this->FrameBuffers[this->BufferCount].length = buf.length;
    this->FrameBuffers[this->BufferCount].start = mmap(NULL /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, this->FileDescriptor, buf.m.offset);

    if (MAP_FAILED == this->FrameBuffers[this->BufferCount].start)
    {
      LOG_ERROR("mmap" << ": " << strerror(errno));
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InitUserp(unsigned int bufferSize)
{
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_USERPTR;

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_REQBUFS, &req))
  {
    if (EINVAL == errno)
    {
      LOG_ERROR(this->DeviceName << " does not support user pointer i/o");
    }
    else
    {
      LOG_ERROR("VIDIOC_REQBUFS" << ": " << strerror(errno));
    }
    return PLUS_FAIL;
  }

  this->FrameBuffers = (FrameBuffer*) calloc(4, sizeof(FrameBuffer));

  if (!this->FrameBuffers)
  {
    LOG_ERROR("Out of memory");
    return PLUS_FAIL;
  }

  for (this->BufferCount = 0; this->BufferCount < 4; ++this->BufferCount)
  {
    this->FrameBuffers[this->BufferCount].length = bufferSize;
    this->FrameBuffers[this->BufferCount].start = malloc(bufferSize);

    if (!this->FrameBuffers[this->BufferCount].start)
    {
      LOG_ERROR("Out of memory");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InternalConnect()
{
  // Ensure we can detect the device in the OS
  struct stat st;
  if (-1 == stat(this->DeviceName.c_str(), &st))
  {
    LOG_ERROR("Cannot identify " << this->DeviceName << ": " << strerror(errno));
    return PLUS_FAIL;
  }
  if (!S_ISCHR(st.st_mode))
  {
    LOG_ERROR(this->DeviceName << " is not a valid device.");
    return PLUS_FAIL;
  }

  // Open the device
  this->FileDescriptor = open(this->DeviceName.c_str(), O_RDWR | O_NONBLOCK, 0);
  if (-1 == this->FileDescriptor)
  {
    LOG_ERROR("Cannot open " << this->DeviceName << ": " << strerror(errno));
    return PLUS_FAIL;
  }

  // Confirm requested device is capable
  v4l2_capability cap;
  if (-1 == xioctl(this->FileDescriptor, VIDIOC_QUERYCAP, &cap))
  {
    if (EINVAL == errno)
    {
      LOG_ERROR(this->DeviceName << " is not a V4L2 device");
    }
    else
    {
      LOG_ERROR("VIDIOC_QUERYCAP" << ": " << strerror(errno));
    }
    return PLUS_FAIL;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
  {
    LOG_ERROR(this->DeviceName << " is not a video capture device");
    return PLUS_FAIL;
  }

  switch (this->IOMethod)
  {
    case IO_METHOD_READ:
    {
      if (!(cap.capabilities & V4L2_CAP_READWRITE))
      {
        LOG_ERROR(this->DeviceName << " does not support read i/o");
        return PLUS_FAIL;
      }
      break;
    }
    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
    {
      if (!(cap.capabilities & V4L2_CAP_STREAMING))
      {
        LOG_ERROR(this->DeviceName << " does not support streaming i/o");
        return PLUS_FAIL;
      }
      break;
    }
    default:
    {
      LOG_ERROR("Unknown IO method.");
      return PLUS_FAIL;
    }
  }

  // Select video input, video standard and tune here
  struct v4l2_cropcap cropcap;
  CLEAR(cropcap);
  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (0 == xioctl(this->FileDescriptor, VIDIOC_CROPCAP, &cropcap))
  {
    struct v4l2_crop crop;
    CLEAR(crop);
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;

    // TODO : get clip information from data source and set to device
    if (-1 == xioctl(this->FileDescriptor, VIDIOC_S_CROP, &crop))
    {
      switch (errno)
      {
        case EINVAL:
          // Cropping not supported
          break;
      }
    }
  }

  // Retrieve current v4l2 format settings
  this->DeviceFormat->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(this->FileDescriptor, VIDIOC_G_FMT, this->DeviceFormat.get()))
  {
    LOG_ERROR("VIDIOC_G_FMT" << ": " << strerror(errno));
    return PLUS_FAIL;
  }

  if (this->FormatWidth != nullptr)
  {
    this->DeviceFormat->fmt.pix.width = *this->FormatWidth;
  }
  if (this->FormatHeight != nullptr)
  {
    this->DeviceFormat->fmt.pix.height = *this->FormatHeight;
  }
  if (this->PixelFormat != nullptr)
  {
    this->DeviceFormat->fmt.pix.pixelformat = *this->PixelFormat;
  }
  if (this->FieldOrder != nullptr)
  {
    this->DeviceFormat->fmt.pix.field = *this->FieldOrder;
  }

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_S_FMT, this->DeviceFormat.get()))
  {
    LOG_WARNING("Unable to set requested video format. Continuing with existing format: " << strerror(errno));
    if (-1 == xioctl(this->FileDescriptor, VIDIOC_G_FMT, this->DeviceFormat.get()))
    {
      LOG_ERROR("VIDIOC_G_FMT" << ": " << strerror(errno));
      return PLUS_FAIL;
    }
  }

  assert(this->DataSource != nullptr);

  this->DataSource->SetInputFrameSize(this->DeviceFormat->fmt.pix.width, this->DeviceFormat->fmt.pix.height, 1);
  this->ImageSize[0] = this->DeviceFormat->fmt.pix.width;
  this->ImageSize[1] = this->DeviceFormat->fmt.pix.height;
  this->ImageSize[2] = 1;
  this->DataSource->SetPixelType(VTK_UNSIGNED_CHAR);
  this->NumberOfScalarComponents = this->DeviceFormat->fmt.pix.sizeimage / this->DeviceFormat->fmt.pix.width / this->DeviceFormat->fmt.pix.height;
  this->DataSource->SetNumberOfScalarComponents(this->NumberOfScalarComponents);

  this->FrameFields["pixelformat"] = vtkPlusV4L2VideoSource::PixelFormatToString(this->DeviceFormat->fmt.pix.pixelformat);

  // Use this->DeviceFormat to initialize data source
  switch (this->IOMethod)
  {
    case IO_METHOD_READ:
    {
      return this->InitRead(this->DeviceFormat->fmt.pix.sizeimage);
    }
    case IO_METHOD_MMAP:
    {
      return this->InitMmap();
    }
    case IO_METHOD_USERPTR:
    {
      return this->InitUserp(this->DeviceFormat->fmt.pix.sizeimage);
    }
    default:
    {
      return PLUS_FAIL;
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InternalDisconnect()
{
  switch (this->IOMethod)
  {
    case IO_METHOD_READ:
    {
      free(this->FrameBuffers[0].start);
      break;
    }
    case IO_METHOD_MMAP:
    {
      for (unsigned int i = 0; i < this->BufferCount; ++i)
      {
        if (-1 == munmap(this->FrameBuffers[i].start, this->FrameBuffers[i].length))
        {
          LOG_ERROR("munmap" << ": " << strerror(errno));
          return PLUS_FAIL;
        }
      }
      break;
    }
    case IO_METHOD_USERPTR:
    {
      for (unsigned int i = 0; i < this->BufferCount; ++i)
      {
        free(this->FrameBuffers[i].start);
      }
      break;
    }
  }

  free(this->FrameBuffers);

  if (-1 == close(this->FileDescriptor))
  {
    LOG_ERROR("Close" << ": " << strerror(errno));
    return PLUS_FAIL;
  }

  this->BufferCount = 0;
  this->FileDescriptor = -1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InternalUpdate()
{
  fd_set fds;
  timeval tv;
  int r;

  FD_ZERO(&fds);
  FD_SET(this->FileDescriptor, &fds);

  // Timeout
  tv.tv_sec = 2;
  tv.tv_usec = 0;

  r = select(this->FileDescriptor + 1, &fds, NULL, NULL, &tv);

  if (-1 == r)
  {
    LOG_ERROR("Unable to select video device" << ": " << strerror(errno));
    return PLUS_FAIL;
  }

  if (0 == r)
  {
    LOG_ERROR("Select timeout.");
    return PLUS_FAIL;
  }

  unsigned int currentBufferIndex;
  unsigned int bytesUsed;
  if (this->ReadFrame(currentBufferIndex, bytesUsed) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->DataSource->AddItem(this->FrameBuffers[currentBufferIndex].start, this->ImageSize, bytesUsed, US_IMG_BRIGHTNESS, this->FrameNumber, UNDEFINED_TIMESTAMP, UNDEFINED_TIMESTAMP, &this->FrameFields) != PLUS_SUCCESS)
  {
    LOG_ERROR("vtkPlusV4L2VideoSource::Unable to add item to the buffer.");
    return PLUS_FAIL;
  }

  this->FrameNumber++;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::ReadFrame(unsigned int& currentBufferIndex, unsigned int& bytesUsed)
{
  switch (this->IOMethod)
  {
    case IO_METHOD_READ:
    {
      return ReadFrameFileDescriptor(currentBufferIndex, bytesUsed);
    }
    case IO_METHOD_MMAP:
    {
      return ReadFrameMemoryMap(currentBufferIndex, bytesUsed);
    }
    case IO_METHOD_USERPTR:
    {
      return ReadFrameUserPtr(currentBufferIndex, bytesUsed);
    }
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::ReadFrameFileDescriptor(unsigned int& currentBufferIndex, unsigned int& bytesUsed)
{
  if (-1 == read(this->FileDescriptor, this->FrameBuffers[0].start, this->FrameBuffers[0].length))
  {
    switch (errno)
    {
      case EAGAIN:
      {
        return PLUS_FAIL;
      }
      case EIO:
      {
        // Could ignore EIO, see spec
      }
      default:
      {
        LOG_ERROR("Read" << ": " << strerror(errno));
        return PLUS_FAIL;
      }
    }
  }

  currentBufferIndex = 0;
  bytesUsed = this->FrameBuffers[0].length;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::ReadFrameMemoryMap(unsigned int& currentBufferIndex, unsigned int& bytesUsed)
{
  struct v4l2_buffer buf;
  CLEAR(buf);

  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_DQBUF, &buf))
  {
    switch (errno)
    {
      case EAGAIN:
      {
        return PLUS_FAIL;
      }
      case EIO:
      {
        // Could ignore EIO, see spec
      }
      default:
      {
        LOG_ERROR("VIDIOC_DQBUF" << ": " << strerror(errno));
        return PLUS_FAIL;
      }
    }
  }

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_QBUF, &buf))
  {
    LOG_ERROR("VIDIOC_QBUF" << ": " << strerror(errno));
    return PLUS_FAIL;
  }

  currentBufferIndex = buf.index;
  bytesUsed = buf.length;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::ReadFrameUserPtr(unsigned int& currentBufferIndex, unsigned int& bytesUsed)
{
  v4l2_buffer buf;
  CLEAR(buf);

  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_USERPTR;

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_DQBUF, &buf))
  {
    switch (errno)
    {
      case EAGAIN:
      {
        return PLUS_FAIL;
      }
      case EIO:
      {
        // Could ignore EIO, see spec
      }
      default:
      {
        LOG_ERROR("VIDIOC_DQBUF" << ": " << strerror(errno));
        return PLUS_FAIL;
      }
    }
  }

  for (currentBufferIndex = 0; currentBufferIndex < this->BufferCount; ++currentBufferIndex)
  {
    if (buf.m.userptr == (unsigned long) this->FrameBuffers[currentBufferIndex].start && buf.length == this->FrameBuffers[currentBufferIndex].length)
    {
      break;
    }
  }

  if (-1 == xioctl(this->FileDescriptor, VIDIOC_QBUF, &buf))
  {
    LOG_ERROR("VIDIOC_QBUF" << ": " << strerror(errno));
    return PLUS_FAIL;
  }

  bytesUsed = buf.bytesused;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::NotifyConfigured()
{
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusV4L2VideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  if (this->GetFirstActiveOutputVideoSource(this->DataSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot retrieve first output video source. Unable to start device.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InternalStopRecording()
{
  v4l2_buf_type type;

  switch (this->IOMethod)
  {
    case IO_METHOD_READ:
    {
      // Nothing to do
      break;
    }
    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
    {
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (-1 == xioctl(this->FileDescriptor, VIDIOC_STREAMOFF, &type))
      {
        LOG_ERROR("VIDIOC_STREAMOFF" << ": " << strerror(errno));
        break;
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusV4L2VideoSource::InternalStartRecording()
{
  enum v4l2_buf_type type;

  switch (this->IOMethod)
  {
    case IO_METHOD_MMAP:
    {
      for (unsigned int i = 0; i < this->BufferCount; ++i)
      {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(this->FileDescriptor, VIDIOC_QBUF, &buf))
        {
          LOG_ERROR("VIDIOC_QBUF" << ": " << strerror(errno));
          return PLUS_FAIL;
        }
      }
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (-1 == xioctl(this->FileDescriptor, VIDIOC_STREAMON, &type))
      {
        LOG_ERROR("VIDIOC_STREAMON" << ": " << strerror(errno));
        return PLUS_FAIL;
      }
      break;
    }
    case IO_METHOD_USERPTR:
    {
      for (unsigned int i = 0; i < this->BufferCount; ++i)
      {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;
        buf.m.userptr = (unsigned long) this->FrameBuffers[i].start;
        buf.length = this->FrameBuffers[i].length;

        if (-1 == xioctl(this->FileDescriptor, VIDIOC_QBUF, &buf))
        {
          LOG_ERROR("VIDIOC_QBUF" << ": " << strerror(errno));
          return PLUS_FAIL;
        }
      }
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (-1 == xioctl(this->FileDescriptor, VIDIOC_STREAMON, &type))
      {
        LOG_ERROR("VIDIOC_STREAMON" << ": " << strerror(errno));
        return PLUS_FAIL;
      }
      break;
    }
    default:
    {}
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusV4L2VideoSource::IOMethodToString(V4L2_IO_METHOD ioMethod)
{
  switch (ioMethod)
  {
    case IO_METHOD_READ:
      return "IO_METHOD_READ";
    case IO_METHOD_MMAP:
      return "IO_METHOD_MMAP";
    case IO_METHOD_USERPTR:
      return "IO_METHOD_USERPTR";
    default:
      return "IO_METHOD_UNKNOWN";
  }
}

//----------------------------------------------------------------------------
vtkPlusV4L2VideoSource::V4L2_IO_METHOD vtkPlusV4L2VideoSource::StringToIOMethod(const std::string& method)
{
  if (igsioCommon::IsEqualInsensitive(method, "IO_METHOD_READ"))
  {
    return IO_METHOD_READ;
  }
  else if (igsioCommon::IsEqualInsensitive(method, "IO_METHOD_MMAP"))
  {
    return IO_METHOD_MMAP;
  }
  else if (igsioCommon::IsEqualInsensitive(method, "IO_METHOD_USERPTR"))
  {
    return IO_METHOD_USERPTR;
  }
  else
  {
    return IO_METHOD_UNKNOWN;
  }
}

//----------------------------------------------------------------------------
#define PIXEL_FORMAT_CASE(format) case format: return #format
std::string vtkPlusV4L2VideoSource::PixelFormatToString(unsigned int pixelFormat)
{
  switch (pixelFormat)
  {
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB332);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB444);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_ARGB444);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_XRGB444);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB555);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_ARGB555);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_XRGB555);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB565);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB555X);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_ARGB555X);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_XRGB555X);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB565X);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_BGR666);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_BGR24);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB24);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_BGR32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_ABGR32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_XBGR32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_RGB32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_ARGB32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_XRGB32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_GREY);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y4);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y6);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y10);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y12);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y16);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y16_BE);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y10BPACK);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_PAL8);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_UV8);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUYV);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YYUV);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YVYU);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_UYVY);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_VYUY);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y41P);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV444);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV555);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV565);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV32);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_HI240);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_HM12);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_M420);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV12);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV21);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV16);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV61);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV24);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV42);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV12M);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV21M);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV16M);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV61M);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV12MT);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_NV12MT_16X16);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV410);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YVU410);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV411P);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV420);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YVU420);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV422P);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YUV420M);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_YVU420M);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_MJPEG);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_JPEG);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_DV);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_MPEG);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_H264);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_H264_NO_SC);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_H264_MVC);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_H263);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_MPEG1);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_MPEG2);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_MPEG4);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_XVID);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_VC1_ANNEX_G);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_VC1_ANNEX_L);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_VP8);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_CPIA1);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_WNVA);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SN9C10X);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SN9C20X_I420);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_PWC1);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_PWC2);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_ET61X251);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SPCA501);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SPCA505);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SPCA508);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SPCA561);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_PAC207);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_MR97310A);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_JL2005BCD);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SN9C2028);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SQ905C);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_PJPG);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_OV511);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_OV518);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_STV0680);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_TM6000);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_CIT_YYVYUY);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_KONICA420);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_JPGL);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_SE401);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_S5C_UYVY_JPG);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y8I);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Y12I);
      PIXEL_FORMAT_CASE(V4L2_PIX_FMT_Z16);
    default:
    {return "V4L2_PIX_FMT_XXXX";}
  }
}
#undef PIXEL_FORMAT_CASE

//----------------------------------------------------------------------------
#define PIXEL_FORMAT_STRING_COMPARE(format, formatStr) else if (igsioCommon::IsEqualInsensitive(#format, formatStr)) return format
unsigned int vtkPlusV4L2VideoSource::StringToPixelFormat(const std::string& format)
{
  if (igsioCommon::IsEqualInsensitive("V4L2_PIX_FMT_RGB332", format)) {return V4L2_PIX_FMT_RGB332;}
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB444, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_ARGB444, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_XRGB444, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB555, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_ARGB555, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_XRGB555, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB565, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB555X, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_ARGB555X, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_XRGB555X, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB565X, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_BGR666, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_BGR24, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB24, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_BGR32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_ABGR32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_XBGR32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_RGB32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_ARGB32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_XRGB32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_GREY, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y4, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y6, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y10, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y12, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y16, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y16_BE, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y10BPACK, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_PAL8, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_UV8, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUYV, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YYUV, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YVYU, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_UYVY, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_VYUY, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y41P, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV444, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV555, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV565, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV32, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_HI240, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_HM12, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_M420, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV12, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV21, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV16, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV61, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV24, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV42, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV12M, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV21M, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV16M, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV61M, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV12MT, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_NV12MT_16X16, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV410, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YVU410, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV411P, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV420, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YVU420, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV422P, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YUV420M, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_YVU420M, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_MJPEG, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_JPEG, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_DV, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_MPEG, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_H264, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_H264_NO_SC, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_H264_MVC, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_H263, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_MPEG1, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_MPEG2, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_MPEG4, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_XVID, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_VC1_ANNEX_G, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_VC1_ANNEX_L, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_VP8, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_CPIA1, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_WNVA, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SN9C10X, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SN9C20X_I420, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_PWC1, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_PWC2, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_ET61X251, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SPCA501, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SPCA505, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SPCA508, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SPCA561, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_PAC207, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_MR97310A, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_JL2005BCD, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SN9C2028, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SQ905C, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_PJPG, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_OV511, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_OV518, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_STV0680, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_TM6000, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_CIT_YYVYUY, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_KONICA420, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_JPGL, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_SE401, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_S5C_UYVY_JPG, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y8I, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Y12I, format);
  PIXEL_FORMAT_STRING_COMPARE(V4L2_PIX_FMT_Z16, format);
  else {return v4l2_fourcc('x', 'x', 'x', 'x');}
}
#undef PIXEL_FORMAT_STRING_COMPARE

//----------------------------------------------------------------------------
#define FIELD_ORDER_CASE(field) case field: return #field
std::string vtkPlusV4L2VideoSource::FieldOrderToString(v4l2_field field)
{
  switch (field)
  {
      FIELD_ORDER_CASE(V4L2_FIELD_ANY);
      FIELD_ORDER_CASE(V4L2_FIELD_NONE);
      FIELD_ORDER_CASE(V4L2_FIELD_TOP);
      FIELD_ORDER_CASE(V4L2_FIELD_BOTTOM);
      FIELD_ORDER_CASE(V4L2_FIELD_INTERLACED);
      FIELD_ORDER_CASE(V4L2_FIELD_SEQ_TB);
      FIELD_ORDER_CASE(V4L2_FIELD_SEQ_BT);
      FIELD_ORDER_CASE(V4L2_FIELD_ALTERNATE);
      FIELD_ORDER_CASE(V4L2_FIELD_INTERLACED_TB);
      FIELD_ORDER_CASE(V4L2_FIELD_INTERLACED_BT);
    default:
    {return "V4L2_FIELD_ANY";}
  }
}
#undef FIELD_ORDER_CASE

//----------------------------------------------------------------------------
v4l2_field vtkPlusV4L2VideoSource::StringToFieldOrder(const std::string& field)
{
  if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_ANY", field)) {return V4L2_FIELD_ANY;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_TOP", field)){return V4L2_FIELD_TOP;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_BOTTOM", field)){return V4L2_FIELD_BOTTOM;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_INTERLACED", field)){return V4L2_FIELD_INTERLACED;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_SEQ_TB", field)){return V4L2_FIELD_SEQ_TB;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_SEQ_BT", field)){return V4L2_FIELD_SEQ_BT;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_ALTERNATE", field)){return V4L2_FIELD_ALTERNATE;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_INTERLACED_TB", field)){return V4L2_FIELD_INTERLACED_TB;}
  else if (igsioCommon::IsEqualInsensitive("V4L2_FIELD_INTERLACED_BT", field)){return V4L2_FIELD_INTERLACED_BT;}
  else {return V4L2_FIELD_ANY;}
}
