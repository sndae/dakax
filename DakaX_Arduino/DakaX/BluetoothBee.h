/* --------------------------------------------------------------------------
 * BluetoothBee Helper Class for DakaX
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 */

#ifndef _BLUETOOTHBEE_H
#define _BLUETOOTHBEE_H

#include "WProgram.h"

 class BluetoothBeeImpl
 {
 public:
    
    BluetoothBeeImpl():
    _init(false),
    _baudRate(38400)
    {

    }  
    
    bool init(HardwareSerial& serialPort,
              const char* name,const char* pinCode,unsigned long baudRate=38400);
    void setBaudRate(HardwareSerial& serialPort,
                    unsigned long baudRate=38400,
                    unsigned long currentBaudRate=38400);

 protected:
   
   void writeInit(const char* str);
   void checkOk();
   
   unsigned long    _baudRate;
   bool             _init;
   HardwareSerial*  _serial;
      
 };


extern BluetoothBeeImpl BluetoothBee;
 
 #endif //_BLUETOOTHBEE_H
