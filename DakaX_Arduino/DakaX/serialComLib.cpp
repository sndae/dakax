/* ----------------------------------------------------------------------------
 *  Serial Communication Lib
 * ----------------------------------------------------------------------------
 *  serialComLib.cpp								
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */

#include "serialComLib.h"

#define   MIN_PACKET_LENGTH    5 // 2(header) + 1(commandId) + 1(length) + 1(checksum) = 5
#define   TRY_OUT_COUNT        20

SerialComLib::SerialComLib():
_open(false),
_serialPort(NULL),
_bufferPos(0)
{
}

bool SerialComLib::open(HardwareSerial& serialPort,unsigned long baudRate)
{
  if(_open)
    return true;
  
  _serialPort = &serialPort;
  
  // start the hardware uart
  _serialPort->begin(baudRate);
  _open = true;

  return _open;
}

// send packet
void SerialComLib::beginSend(unsigned char cmdId)
{
    _bufferPos = 0;
    _cmdId = cmdId;
}

void SerialComLib::sendByte(unsigned char data)
{
    _buffer[_bufferPos++] = data;
}

void SerialComLib::sendUShort(unsigned short data)
{
    _buffer[_bufferPos++] = 0xff & data;
    _buffer[_bufferPos++] = (0xff00 & data) >> 8;
}

void SerialComLib::sendInt(int data)
{
  
    _buffer[_bufferPos++] = 0xff & data;
    _buffer[_bufferPos++] = (0xff00 & data) >> 8;
  /*  
    _buffer[_bufferPos++] = 0xff;
    _buffer[_bufferPos++] = 0xff;
    */
}

void SerialComLib::sendFloat(float data)
{
  unsigned char* ptr = (unsigned char*)&data;
    _buffer[_bufferPos++] = *(ptr+0);
    _buffer[_bufferPos++] = *(ptr+1);
    _buffer[_bufferPos++] = *(ptr+2);
    _buffer[_bufferPos++] = *(ptr+3);
}

void SerialComLib::sendString(const char* str)
{
    unsigned char len = strlen(str);
    _buffer[_bufferPos++] = len;
    if(len > 0)
    {
        for(int i=0;i < len;i++)
            _buffer[_bufferPos++] = str[i];
    }
}

void SerialComLib::endSend()
{
    writeCmd(_cmdId, _bufferPos,_buffer);
}
   
// readn packet
int SerialComLib::readPacket()
{
    if(available() == false)
        return Error_NoData;

    _cmdId = 0;
    _bufferLength = 0;
    _bufferPos = 0;
    
    return readCmd(&_cmdId,
                   &_bufferLength, _buffer,MAX_BUFFER_SIZE);
}

unsigned char SerialComLib::getCmd()
{
    return _cmdId;
}

unsigned char SerialComLib::getCmdLength()
{
    return _bufferLength;
}
   
unsigned char  SerialComLib::getByte()
{
    if(_bufferPos < _bufferLength)
        return _buffer[_bufferPos++];
    return 0;
}

unsigned short SerialComLib::getUShort()
{
    if(_bufferPos+1 < _bufferLength)
    {
        return(_buffer[_bufferPos++] | (_buffer[_bufferPos++] << 8));
    }
    return 0;
}

unsigned char SerialComLib::getString(unsigned char strLen,char* buffer)
{
    if(_bufferPos < _bufferLength)
    {
        unsigned char len = _buffer[_bufferPos++];
        if((len > 0) &&
           (_bufferPos + len < _bufferLength))
        {
            for(int i=0;i < len && i < strLen;i++)
                buffer[i] = _buffer[_bufferPos++];
        }

        return len;
    }
    return 0;
}

float SerialComLib::getFloat()
{
   if(_bufferPos+4 < _bufferLength)
    {
        float retVal = 0.0f;
        unsigned char* ptr = (unsigned char*)&retVal;
        
        *(ptr+0) = _buffer[_bufferPos++];
        *(ptr+1) = _buffer[_bufferPos++];
        *(ptr+2) = _buffer[_bufferPos++];
        *(ptr+3) = _buffer[_bufferPos++];
        
        return retVal;
    }
    
    return 0.0f;
}
   
bool SerialComLib::isPacketEnd()
{
    return(_bufferPos >= _bufferLength);
}

unsigned char* SerialComLib::getData()
{
    return _buffer;
}


bool SerialComLib::available()
{
  return (_serialPort->available() >= MIN_PACKET_LENGTH)? true : false;
}

void SerialComLib::writeCmd(unsigned char cmdId, unsigned char dataLen, unsigned char* data)
{
  static unsigned char i;
   
  // write header
  _serialPort->write(DATAHEADER1);
  _serialPort->write(DATAHEADER2);
  
  // write commandId
  _serialPort->write(cmdId);

  // write data
  _serialPort->write(dataLen);
  if(dataLen == 0)
    _serialPort->write(0xff);  // checksum
  else
  { 
    for(i=0;i<dataLen;i++)
    {
        _serialPort->write(data[i]);
        //delay(1);
    }

   //   _serialPort->write(data,dataLen);

    // calc the checksume and write it
    _serialPort->write(checksum(dataLen,data));
  }
}

unsigned char SerialComLib::readCmd(unsigned char* cmdId, unsigned char* dataLen, unsigned char* data,unsigned char dataLenMax)
{
  if(findDataHeader())
  {  // read the data
  
     // read commandId
     if(readBlocked(*_serialPort,1,cmdId) != Ok)
       return Error_NoData;
     
     // read dataLen
     if(readBlocked(*_serialPort,1,dataLen) != Ok)
       return Error_NoData;
     
     // read data
     if(*dataLen > dataLenMax)
       return Error_DataSize;
       
     if(*dataLen > 0)
     {
       if(readBlocked(*_serialPort,*dataLen,data) != Ok)
         return Error_NoData;
     }
       
     // checksum 
     if(readBlocked(*_serialPort,1,&_byte) != Ok)
       return Error_NoData;
     
     if(testChecksum(*dataLen,data,_byte))
       return Ok;
  }
  else
    return Error_DataHeader;
}
  
unsigned char SerialComLib::readBlocked(HardwareSerial& serialPort,unsigned char length,unsigned char* readValue,unsigned char readCount,unsigned char delayMS)
{
   for(int i=0;i < length;i++)
     readValue[i] = serialPort.read();
   return Ok;        
/*
  while(readCount > 0)
  {
    if(serialPort.available() >= length)
    {  // read out the data
      for(int i=0;i < length;i++)
        readValue[i] = serialPort.read();
      return Ok;
    }
    else
      // wait a little
      delay(delayMS); 
    --readCount;
  }
  
  return 1;
  */
}
  
bool SerialComLib::findDataHeader()
{ 
  char count = TRY_OUT_COUNT;
  while(count > 0)
  {
    if(readBlocked(*_serialPort,1,&_byte) == Ok && _byte == DATAHEADER1)
    {  // read dataHeader1
      if(readBlocked(*_serialPort,1,&_byte) == Ok && _byte == DATAHEADER2)
        // read dataHeader2
        return true;         
    } 
    --count;
  }
  return false;
}

unsigned char SerialComLib::checksum(unsigned char len,unsigned char* data)
{
  unsigned short checkSum = 0;
  for(int i=0;i < len;i++)
    checkSum += data[i];
  return(0xff - (0xff & checkSum));
}

bool SerialComLib::testChecksum(unsigned char len,unsigned char* data,unsigned char checksumVal)
{
  return checksum(len,data) == checksumVal;
}

