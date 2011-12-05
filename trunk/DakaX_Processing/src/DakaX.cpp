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

#ifdef WIN32
    #define _CRT_SECURE_NO_WARNINGS
    #define NOMINMAX	// eigen + windows.h will have conflict without this
#endif

// eigen
// #include <Eigen/Geometry>


#include "DakaX.h"

#define   MIN_PACKET_LENGTH    5 // 2(header) + 1(commandId) + 1(length) + 1(checksum) = 5
#define   TRY_OUT_COUNT        20

unsigned char gHeader[] = {DATAHEADER1,DATAHEADER2};

///////////////////////////////////////////////////////////////////////////////
// DataPacket

void DataPacket::reset()
{
    _bufferPos      = 0;
    _bufferLength   = 0;
    _cmdId          = 0;
    _bufferFillPos  = 0;
    _readBufferPos  = 0;
    _checkSumFlag   = false;
    _checkSum       = 0;
}

bool DataPacket::testChecksum(unsigned char checksumVal)
{
    _checkSum = checksum(_bufferLength,_buffer);
    _checkSumFlag = (_checkSum == checksumVal);
    return _checkSumFlag;
}

unsigned char DataPacket::checksum(unsigned char len,unsigned char* data)
{
  unsigned short checkSum = 0;
  for(int i=0;i < len;i++)
    checkSum += data[i];
  return(0xff - (0xff & checkSum));
}

bool DataPacket::fillData(unsigned char* data,int len,int* fillCount)
{
    *fillCount = 0;
    if(_bufferFillPos >= _bufferLength)
        return true;

    // fill up the data
    int length = len;
    if(length > (_bufferLength - _bufferFillPos))
        length = _bufferLength - _bufferFillPos;

    memcpy(_buffer + _bufferFillPos,data,length);
    *fillCount = length;

    _bufferFillPos += length;
    if(_bufferFillPos >= _bufferLength)
        return true;
    else
        return false;
}


unsigned char DataPacket::getByte()
{
    if(_readBufferPos < _bufferLength)
        return _buffer[_readBufferPos++];
    return 0;
}

unsigned short DataPacket::getUShort()
{
    if(_readBufferPos+1 < _bufferLength)
    {
        return(_buffer[_readBufferPos++] + (_buffer[_readBufferPos++] << 8));
    }
    return 0;
}

short DataPacket::getShort()
{
    if(_readBufferPos+1 < _bufferLength)
    {
        short val = (_buffer[_readBufferPos] + (_buffer[_readBufferPos+1] << 8));
       /*
        std::cout << "getShort: " << std::dec << val << ": " << std::hex << (int)_buffer[_readBufferPos] << "," <<
                                                     (int)_buffer[_readBufferPos+1] << std::endl;
*/
        _readBufferPos += 2;
        return val;
        //return(_buffer[_readBufferPos++] + (_buffer[_readBufferPos++] << 8));
    }
    return 0;
}

float DataPacket::getFloat()
{
   if(_readBufferPos+4 < _bufferLength)
    {
        float retVal = 0.0f;
        unsigned char* ptr = (unsigned char*)&retVal;

        *(ptr+0) = _buffer[_readBufferPos++];
        *(ptr+1) = _buffer[_readBufferPos++];
        *(ptr+2) = _buffer[_readBufferPos++];
        *(ptr+3) = _buffer[_readBufferPos++];

        return retVal;
    }

    return 0.0f;
}

unsigned char  DataPacket::getString(unsigned char strLen,char* buffer)
{
    if(_readBufferPos < _bufferLength)
    {
        unsigned char len = _buffer[_readBufferPos++];
        if((len > 0) &&
           (_readBufferPos + len < _bufferLength))
        {
            len = len > strLen ? strLen: len;

            memcpy(buffer,_buffer,len);
            if(len+1 < strLen)
                buffer[len+1] = 0;
        }

        return len;
    }
    return 0;
}

// write
void DataPacket::beginWrite(unsigned char cmd)
{
    _bufferPos = 0;
    _cmdId = cmd;
}

void DataPacket::writeByte(unsigned char data)
{
    _buffer[_bufferPos++] = data;
}

void DataPacket::writeUShort(unsigned short data)
{
    _buffer[_bufferPos++] = 0xff & data;
    _buffer[_bufferPos++] = (0xff00 & data) >> 8;
}

void DataPacket::writeShort(short data)
{
    _buffer[_bufferPos++] = 0xff & data;
    _buffer[_bufferPos++] = (0xff00 & data) >> 8;
}

void DataPacket::writeInt(int data)
{
    _buffer[_bufferPos++] = 0xff & data;
    _buffer[_bufferPos++] = (0xff00 & data) >> 8;
}

void DataPacket::writeFloat(float data)
{
    unsigned char* ptr = (unsigned char*)&data;
    _buffer[_bufferPos++] = *(ptr+0);
    _buffer[_bufferPos++] = *(ptr+1);
    _buffer[_bufferPos++] = *(ptr+2);
    _buffer[_bufferPos++] = *(ptr+3);
}

void DataPacket::writeString(const char* str)
{
    unsigned char len = strlen(str);
    _buffer[_bufferPos++] = len;
    if(len > 0)
    {
        for(int i=0;i < len;i++)
            _buffer[_bufferPos++] = str[i];
    }
}

void DataPacket::endWrite()
{
    _bufferLength = _bufferPos;

    // set checksum
    _checkSum = checksum(_bufferLength,_buffer);
}

void DataPacket::println()
{
    std::cout << "DataPacket dump" << std::endl;
    std::cout << "cmd:\t\t" << std::dec << _cmdId << std::endl;
    std::cout << "bufferLength:\t\t" << _bufferLength << std::endl;
    std::cout << "data:" << std::endl;
    for(int i=0;i<_bufferLength;i++)
        std::cout << std::dec << i << ":0x" << std::hex << (_buffer[i] & 0xff) << " , ";
    std::cout << "\ncheckSum:\t" << std::dec << _checkSum << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
// SerialComLibBase

SerialComLibBase::SerialComLibBase():
    _open(false),
    _serialPort(NULL),
    _bufferPos(0),
    _parserPos(Parse_Start)
{
}

SerialComLibBase::~SerialComLibBase()
{
    close();
}

void SerialComLibBase::close()
{
    if(_serialPort)
    {
        std::cout << "serialPort closed" << std::endl;

        _serialPort->close();
        delete _serialPort;
        _serialPort = NULL;
   }
    _open = false;
}

bool SerialComLibBase::open(const char* serialPortName,unsigned long baudRate)
{
  if(_open)
    return true;

  try{
    _serialPort = new CallbackAsyncSerial(std::string(serialPortName),baudRate);
    _serialPort->setCallback(boost::bind( &SerialComLibBase::received,this,_1,_2 ));
  }
  catch(std::exception& e)
  {
    return false;
  }

  std::cout << "serial port open" << std::endl;
  _open = true;
  return _open;
}

void SerialComLibBase::update()
{

}

// send packet
void SerialComLibBase::beginSend(int cmdId)
{
    _sendMutex.lock();

    _sendDataPacket.beginWrite(cmdId);
}

void SerialComLibBase::sendByte(int data)
{
    _sendDataPacket.writeByte(data);
}

void SerialComLibBase::sendUShort(int data)
{
    _sendDataPacket.writeUShort(data);
}

void SerialComLibBase::sendInt(int data)
{
    _sendDataPacket.writeInt(data);
}

void SerialComLibBase::sendFloat(float data)
{
    _sendDataPacket.writeFloat(data);
}

void SerialComLibBase::sendString(const char* str)
{
    _sendDataPacket.writeString(str);

}

void SerialComLibBase::endSend()
{
    _sendDataPacket.endWrite();

    send(_sendDataPacket);

    _sendMutex.unlock();
}

void SerialComLibBase::write(unsigned char byte)
{
    _serialPort->write((char*)&byte,1);
}

void SerialComLibBase::send(DataPacket& dataPacket)
{
    boost::mutex::scoped_lock l(_sendSerialMutex);

    //std::cout << "SerialComLibBase::send" << std::endl;


    // write header
    _serialPort->write((char*)gHeader,2);

    // write commandId
    write(dataPacket.cmd());

    // write data
    write(dataPacket.bufferLength());

    if(dataPacket.bufferLength() == 0)
        write(0xff);  // checksum
    else
    {   // write the data
        _serialPort->write((const char*)dataPacket.buffer(),dataPacket.bufferLength());

        // calc the checksume and write it
        write(dataPacket.checksum());
    }
}

void SerialComLibBase::newPacket(DataPacket* packete)
{
    std::cout << "parsed packet" << std::endl;

    // send it to
}


void SerialComLibBase::parsePacket(unsigned int offset,const unsigned char *data, unsigned int len)
{
    if(offset >= len)
        return;

    //std::cout << "parsePacket len: " << std::dec << (int)len << std::endl;

    for(; offset < len;)
    {
        //std::cout << "offset: "<< offset << " : 0x" << std::hex << (int)(data[offset] & 0xff) << ",";
        switch(_parserPos)
        {
        case Parse_Start:
            //std::cout << "Parse_Start" << std::endl;

            _curDataPacket.reset();

            if(data[offset] == DATAHEADER1)
                _parserPos = Parse_Header;
            offset++;
            break;
        case Parse_Header:
            // std::cout << "Parse_Header" << std::endl;
            if(data[offset] == DATAHEADER2)
                // valid packet, read the rest
                _parserPos = Parse_Cmd;
            else
            {
                if(data[offset] == DATAHEADER1)
                    _parserPos = Parse_Header;
                else
                    _parserPos = Parse_Start;
            }
            offset++;
            break;
        case Parse_Cmd:
            //std::cout << "Parse_Cmd" << std::endl;

            _curDataPacket.setCmd(data[offset]);
            _parserPos = Parse_Length;
            offset++;
            break;
        case Parse_Length:
            //std::cout << "Parse_Length" << std::endl;

            _curDataPacket.setBufferLength(data[offset]);
            _parserPos = Parse_Data;
            offset++;
            break;
        case Parse_Data:
            {
                //std::cout << "Parse_Data" << std::endl;

                int fillCount;
                if(_curDataPacket.fillData((unsigned char*)(data+offset),len-offset,&fillCount))
                    _parserPos = Parse_Checksum;

                offset += fillCount;
            }
            break;
        case Parse_Checksum:
            //std::cout << "Parse_Checksum" << std::endl;
            if(_curDataPacket.testChecksum(data[offset]))
            {   // data valid
                newPacket(&_curDataPacket);
                _parserPos = Parse_Start;

                //std::cout << "get one packet" << std::endl;
            }
            else
                _parserPos = Parse_Start;
            offset++;
            break;
        }
    }

    std::flush(std::cout);

}

void SerialComLibBase::received(const char *data, unsigned int len)
{
    if(len == 0)
        return;

    parsePacket(0,(const unsigned char*)data,len);

/*
    for(int i=0;i<len;i++)
    {
        std::cout << "0x" << std::hex << (int)(data[i] & 0xff) << ",";
    }
    std::cout << std::endl;
*/
}
