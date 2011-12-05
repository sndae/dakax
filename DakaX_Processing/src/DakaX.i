# -----------------------------------------------------------------------------
# DakaX
# -----------------------------------------------------------------------------
# Processing Library for DakaX
# prog:  Max Rheiner / Interaction Design / Zhdk / http://iad.zhdk.ch/
# -----------------------------------------------------------------------------
%module(directors="1") DakaX

%{
#include "DakaX.h"
#include <string>
#include <vector>
%}


# ----------------------------------------------------------------------------
# DataPacket
%feature("director") DataPacket;
class DataPacket
{
public:
    DataPacket();
    DataPacket(const DataPacket& copy);

    const unsigned char*  buffer()const;
    unsigned char         cmd()const;
    unsigned char         bufferLength()const;
    unsigned char         checksum()const;
    bool                  checksumFlag()const;


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
};

# ----------------------------------------------------------------------------
# SerialComLibBase

%feature("director") SerialComLibBase;
class SerialComLibBase
{
public:
    SerialComLibBase();
    virtual ~SerialComLibBase();

   bool open(const char* portName,unsigned long baudRate = 9600);
   void close();

    void update();

   // send packet
   void beginSend(int cmdId);
   void sendByte(int data);       // 1 byte
   void sendUShort(int data);    // 2 byte
   void sendInt(int data);                  // 2 byte
   //void sendShort(int data);              // 2 byte
   void sendFloat(float data);              // 4 byte
   void sendString(const char* str);        // 1Byte - length + nBytes str data
   void endSend();

   void send(DataPacket& dataPacket);

   bool isOpen();

   virtual void newPacket(DataPacket* packete);
};
