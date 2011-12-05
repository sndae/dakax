/* ----------------------------------------------------------------------------
 * DakaX
 * ----------------------------------------------------------------------------
 * Copyright (C) 2011 Max Rheiner / Interaction Design Zhdk
 *
 * This file is part of DakaX.
 *
 * SimpleOpenNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version (subject to the "Classpath" exception
 * as provided in the LICENSE.txt file that accompanied this code).
 *
 * SimpleOpenNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleOpenNI.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 */

#ifndef _DAKAXWRAPPER_H_
#define _DAKAXWRAPPER_H_

#include <vector>
#include <iostream>

// boost
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

#include "AsyncSerial.h"

#define DATAHEADER1  0x1a
#define DATAHEADER2  0xd0

#define  MAX_BUFFER_SIZE (255 + 2)  //

///////////////////////////////////////////////////////////////////////////////
// DataPacket

class DataPacket
{
public:

    DataPacket():
        _bufferPos(0),
        _bufferLength(0),
        _cmdId(0),
        _checkSumFlag(false),
        _checkSum(0),
        _bufferFillPos(0),
        _bufferSize(MAX_BUFFER_SIZE),
        _readBufferPos(0)
    {
        memset(_buffer,0,_bufferSize);
    }

    DataPacket(const DataPacket& copy):
        _bufferPos(0),
        _bufferLength(0),
        _cmdId(0),
        _checkSumFlag(false),
        _checkSum(0),
        _bufferFillPos(0),
        _bufferSize(MAX_BUFFER_SIZE),
        _readBufferPos(0)
    {
        *this = copy;
    }

    DataPacket(unsigned char*  buffer,
               unsigned char   bufferLength,
               unsigned char   cmdId)
    {
        _bufferLength   = bufferLength;
        _cmdId          = cmdId;
        _bufferPos      = 0;
        _bufferFillPos  = 0;
        _readBufferPos  = 0;

        if(_bufferLength > _bufferSize)
            _bufferLength = _bufferSize;

        memcpy(_buffer,buffer,_bufferLength);

        _checkSum       = checksum(_bufferLength,_buffer);
        _checkSumFlag   = true;
    }

    DataPacket& operator=(const DataPacket& copy)
    {
         std::cout << "DataPacket copy" << std::endl;

        _bufferPos      = copy._bufferPos;
        _bufferLength   = copy._bufferLength;
        _cmdId          = copy._cmdId;
        _checkSum       = copy._checkSum;
        _checkSumFlag   = copy._checkSumFlag;

        if(_bufferLength > _bufferSize)
            _bufferLength = _bufferSize;

        memcpy(_buffer,copy._buffer,_bufferLength);

		return this;
    }

    void setCmd(unsigned char cmd) { _cmdId = cmd; }
    void setBufferLength(unsigned char bufferLength) { _bufferLength = bufferLength; }

    const unsigned char*  buffer()const { return _buffer; }
    unsigned char   cmd()const { return _cmdId; }
    unsigned char   bufferLength()const { return _bufferLength; }
    unsigned char   checksum()const { return _checkSum;}
    bool            checksumFlag()const { return _checkSumFlag;}

    bool fillData(unsigned char* data,int len,int* fillCount);

    void reset();

    bool testChecksum(unsigned char checksumVal);
    static unsigned char checksum(unsigned char len,unsigned char* data);

    // read methods
    void startRead()
    {
        _readBufferPos = 0;
    }

    // read
    unsigned char  getByte();
    unsigned short getUShort();
    short          getShort();
    float          getFloat();
    unsigned char  getString(unsigned char strLen,char* buffer);

    // write
    void beginWrite(unsigned char cmd);
    void writeByte(unsigned char data);
    void writeUShort(unsigned short data);
    void writeShort(short data);
    void writeInt(int data);
    void writeFloat(float data);
    void writeString(const char* str);
    void endWrite();

    void println();

protected:

    unsigned char   _buffer[MAX_BUFFER_SIZE];
    int             _bufferSize;

    int             _bufferPos;
    int             _bufferLength;
    int             _cmdId;

    int             _bufferFillPos;

    int             _readBufferPos;

    int             _checkSum;
    bool            _checkSumFlag;
};

///////////////////////////////////////////////////////////////////////////////
// SerialComLibBase

class SerialComLibBase
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

   enum ParsePos{
       Parse_Start,
       Parse_Header,
       Parse_Cmd,
       Parse_Length,
       Parse_Data,
       Parse_Checksum,
   };

   SerialComLibBase();
   virtual ~SerialComLibBase();

   bool open(const char* serialPortName,unsigned long baudRate = 9600);
   void close();

   void update();

   // send packet
   void beginSend(int cmdId);
   void sendByte(int data);    // 1 byte
   void sendUShort(int data);   // 2 byte
   void sendInt(int data);   // 2 byte
   void sendShort(int data);   // 2 byte
   void sendFloat(float data);            // 4 byte
   void sendString(const char* str);    // 1Byte - length + nBytes str data
   void endSend();

   void send(DataPacket& dataPacket);

   bool isOpen() { return _open; }

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

   // serial out

   void write(unsigned char byte);
   void received(const char *data, unsigned int len);

   virtual void newPacket(DataPacket* packete);

protected:

  void             parsePacket(unsigned int offset,const char unsigned *data, unsigned int len);


  bool            _open;

  CallbackAsyncSerial*   _serialPort;

  unsigned char   _buffer[MAX_BUFFER_SIZE];
  unsigned char   _bufferPos;
  unsigned char   _bufferLength;
  unsigned char   _cmdId;

  int             _parserPos;

  DataPacket      _curDataPacket;

  DataPacket      _sendDataPacket;
  boost::mutex    _sendMutex;
  boost::mutex    _sendSerialMutex;

};

#endif // _DAKAXWRAPPER_H_



