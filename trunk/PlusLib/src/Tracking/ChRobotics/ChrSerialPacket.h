#ifndef __ChrSerialPacket_h
#define __ChrSerialPacket_h

#include <vector>

class ChrSerialPacket
{
public:
  ChrSerialPacket()
  {
    m_Address = 0;
    m_PacketDescriptor=0;
    m_Checksum = 0;
    m_Data.resize(16*4);
  }

  unsigned char GetAddress()
  {
    return m_Address;
  }

  void SetAddress(unsigned char address)
  {
    m_Address=address;
  }

  void SetPacketDescriptor(unsigned char packetDescriptor)
  {
    m_PacketDescriptor=packetDescriptor;
  }

  bool GetBatchEnable()
  {
    if( m_PacketDescriptor & 0x40 )
      return true;
    else
      return false;
  }

  void SetBatchEnable(bool enable)
  {
    if( enable )
    {
      m_PacketDescriptor |= 0x40;
    }
    else
    {
      m_PacketDescriptor |= 0x40;
      m_PacketDescriptor ^= 0x40;
    }
  }

  bool GetHasData()
  {
    if( m_PacketDescriptor & 0x80 )
      return true;
    else
      return false;
  }

  void SetHasData( bool has_data )
  {
    if( has_data )
    {
      m_PacketDescriptor |= 0x80;
    }
    else
    {
      m_PacketDescriptor |= 0x80;
      m_PacketDescriptor ^= 0x80;
    }
  }

  unsigned char GetBatchLength()
  {
    return (m_PacketDescriptor >> 2) & 0x0F;
  }

  void SetBatchLength( unsigned char length )
  {
    length &= 0x0F;
    // Clear batch length bits
    m_PacketDescriptor |= (0x0F << 2);
    m_PacketDescriptor ^= (0x0F << 2);
    // Set batch length bits
    m_PacketDescriptor |= (length << 2);
  }

  unsigned char GetCommandFailed()
  {
    return m_PacketDescriptor & 0x01;
  }
  void SetCommandFailed( unsigned char failed)
  {
    failed &= 0x01;
    m_PacketDescriptor |= 0x01;
    m_PacketDescriptor ^= 0x01;
    m_PacketDescriptor |= failed;
  }

  unsigned char GetDataLength()
  {
    if( !GetHasData() )
    {
      return 0;
    }
    if( GetBatchEnable() )
    {
      return 4*GetBatchLength();
    }
    return 4;    
  }

  unsigned char GetPacketLength()
  {
    return GetHeaderLength() + GetDataLength() + GetChecksumLength();
  }

  unsigned char GetPacketByte( int index )
  {
    if (index<GetHeaderLength())
    {
      return GetHeaderByte(index);
    }
    index-=GetHeaderLength();
    if (index<GetDataLength())
    {
      return GetDataByte(index);
    }
    index-=GetDataLength();
    if (index<GetChecksumLength())
    {
      return GetChecksumByte(index);
    }
    return 0;
  }

  unsigned char GetDataByte( int index ) 
  { 
    return m_Data[index]; 
  }
  void SetDataByte( int index, unsigned char value )
  {
    m_Data[index] = value;
  }

  unsigned char GetHeaderByte( int index )
  {
    switch (index)
    {
      case 0: return 's';
      case 1: return 'n';
      case 2: return 'p';
      case 3: return m_PacketDescriptor;
      case 4: return m_Address;
      default:
        return 0;
    }
  }

  unsigned char GetHeaderLength()
  {
    return 5;
  }

  unsigned char GetChecksumLength()
  {
    return 2;
  }

  unsigned char GetChecksumByte( int index )
  {
    switch (index)
    {
      case 0: return (m_Checksum >> 8);
      case 1: return (m_Checksum & 0x0FF);
      default:
        return 0;
    }
  }

  void ComputeChecksum( void )
  {
    unsigned short int checksum;

    checksum = 0;

    checksum += (unsigned char)'s';
    checksum += (unsigned char)'n';
    checksum += (unsigned char)'p';
    checksum += m_PacketDescriptor;
    checksum += m_Address;

    unsigned char dataLength=GetDataLength();
		for( int i = 0; i < dataLength; i++ )
		{
			checksum += m_Data[i];
		}

    m_Checksum = checksum;
  }

private:
  unsigned char m_Address;
  unsigned char m_PacketDescriptor;
  unsigned short m_Checksum;

  std::vector<unsigned char> m_Data;
};

#endif
