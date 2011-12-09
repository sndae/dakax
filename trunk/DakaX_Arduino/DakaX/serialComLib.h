/* ----------------------------------------------------------------------------
 *  Serial Communication Lib
 * ----------------------------------------------------------------------------
 *  serialComLib.h								
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */
 
#ifndef _SERIALCOMLIB_H_
#define _SERIALCOMLIB_H_

#include "WProgram.h"


#define DATAHEADER1  0x1a
#define DATAHEADER2  0xd0

#define  MAX_BUFFER_SIZE 100

class SerialComLib
{
public:

   enum Errors{
    Ok                  =   0,
    Error_Command       =   1,
    Error_Checksum      =   2,
    Error_Timeout       =   3,
    Error_DataHeader    =   4 ,    
    Error_Data          =   5,
    Error_DataSize      =   6,
    Error_NoData        =   7,
   };
   
   SerialComLib();
 
   bool open(HardwareSerial& serialPort,unsigned long baudRate = 9600);
   
   // send packet
   void beginSend(unsigned char cmdId);
   void sendByte(unsigned char data);    // 1 byte
   void sendUShort(unsigned short data);   // 2 byte
   void sendInt(int data);   // 2 byte
   void sendFloat(float data);            // 4 byte
   void sendString(const char* str);    // 1Byte - length + nBytes str data
   void endSend();
   
   // readn packet
   int readPacket();
   unsigned char getCmd();
   unsigned char getCmdLength();
   
   unsigned char  getByte();
   unsigned short getUShort();
   int            getInt();
   float          getFloat();
   unsigned char  getString(unsigned char strLen,char* buffer);
   
   bool isPacketEnd();
   unsigned char* getData();
   
   
   bool           available();
   void           writeCmd(unsigned char cmdId, unsigned char dataLen, unsigned char* data);
   unsigned char  readCmd(unsigned char* cmdId, unsigned char* dataLen, unsigned char* data,unsigned char dataLenMax);
   
   bool isOpen() { return _open; }
  
   static unsigned char readBlocked(HardwareSerial& serialPort,unsigned char length,unsigned char* readValue,unsigned char readCount = 10,unsigned char delayMS = 1);

   // helper func.
   static void ConvShortToStr(unsigned short value,unsigned char* buf)
   {
     buf[0] = 0xff & value;
     buf[1] = (0xff00 & value) >> 8;
   }
   
   static void ConvStrToShort(unsigned char* buf, unsigned short* value)
   {
     *value = buf[0] & (buf[1] << 8);
   }
   
protected:

  bool             findDataHeader();
  unsigned char    checksum(unsigned char len,unsigned char* data);
  bool             testChecksum(unsigned char len,unsigned char* data,unsigned char checksum);
  
  bool            _open;
  unsigned char   _byte;
  
  HardwareSerial* _serialPort;  
  unsigned char   _buffer[MAX_BUFFER_SIZE];
  unsigned char   _bufferPos;
  unsigned char   _bufferLength;
  unsigned char   _cmdId;
};

#endif // _SERIALCOMLIB_H_
