/* --------------------------------------------------------------------------
 * BluetoothBee Helper Class for DakaX
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>

#include "BluetoothBee.h"

//#define _BDEBUG
#define BUFFER_SIZE   64


bool BluetoothBeeImpl::init(HardwareSerial& serialPort,
                            const char* name,const char* pinCode,unsigned long baudRate)
{
  char buffer[BUFFER_SIZE];
  
  _serial   = &serialPort;
  _baudRate = baudRate;

#ifdef _BDEBUG
  Serial.begin(9600);
  Serial.print("------------------\n");
  Serial.print("baudrate:");
  snprintf(buffer,BUFFER_SIZE,"Baudrate: %lu\t current:%lu\n",_baudRate);
  Serial.print(buffer);

#endif

  _baudRate = baudRate;

  // needs to be right, otherwise we can't reprogramm the module
  // the module keeps the baudrate
  _serial->begin(_baudRate);

  // set the bluetooth bee mode
  delay(1000);      
  // mode
  writeInit("\r\n+STWMOD=0\r\n");
 
  /*
  // baudrade
  snprintf(buffer,BUFFER_SIZE,"\r\n+STBD=%lu\r\n",_baudRate);
  writeInit(buffer);
  */
    
  // device name
  snprintf(buffer,BUFFER_SIZE,"\r\n+STNA=%s\r\n",name);
  writeInit(buffer);
  
  writeInit("\r\n+STAUTO=0\r\n");
  
  writeInit("\r\n+STOAUT=1\r\n");
  
  // pin
  snprintf(buffer,BUFFER_SIZE,"\r\n+STPIN=%s\r\n",pinCode);
  writeInit(buffer);
  
  delay(2000); // This delay is required.
  _serial->print("\r\n+INQ=1\r\n");
  delay(2000); // This delay is required.

#ifdef _BDEBUG
  Serial.print("endinit");
#endif

  _init = true; 

  _serial->end();
  delay(100);

  return _init;
}

void BluetoothBeeImpl::setBaudRate(HardwareSerial& serialPort,
                                   unsigned long baudRate,
                                   unsigned long currentBaudRate)
{
    char buffer[BUFFER_SIZE];

    _serial   = &serialPort;
    _baudRate = baudRate;

  #ifdef _BDEBUG
    Serial.begin(9600);
    Serial.print("------------------\n");
    Serial.print("Reprog. Baudrate:");
    snprintf(buffer,BUFFER_SIZE,"Baudrate: %lu\t current:%lu\n",_baudRate,currentBaudRate);
    Serial.print(buffer);

  #endif

    _baudRate = baudRate;

    // needs to be right, otherwise we can't reprogramm the module
    // the module keeps the baudrate
    _serial->begin(currentBaudRate);

    // set the bluetooth bee mode
    delay(1000);

    // baudrade
    snprintf(buffer,BUFFER_SIZE,"\r\n+STBD=%lu\r\n",_baudRate);
    writeInit(buffer);

  #ifdef _BDEBUG
    Serial.print("endinit");
  #endif


    _serial->end();
    delay(100);
}

void BluetoothBeeImpl::writeInit(const char* cmd)
{
  _serial->print(cmd);

#ifdef _BDEBUG  
   Serial.print(cmd);                          //For debugging, Comment this line if not required    
   delay(3000);
    
   while(_serial->available())              //For debugging, Comment this line if not required
   {                                               //For debugging, Comment this line if not required   
      Serial.print(char(_serial->read()));  //For debugging, Comment this line if not required
   }                                               //For debugging, Comment this line if not required                                           //For debugging, Comment this line if not required       
#else   
/*
  delay(3000);
   while(_serial->available())              //For debugging, Comment this line if not required
   {                                               //For debugging, Comment this line if not required   
      _serial->read();
   }  
*/   
  checkOk();
#endif
}

//Checks if the response "OK" is received
void BluetoothBeeImpl::checkOk()
{
  char a,b;
  while(1)
  {
    if(_serial->available() >= 2)
    {
      a = _serial->read();
      if('O' == a)
      {
        // Wait for next character K. available() is required in some cases, as K is not immediately available.
        while(_serial->available())
        {
           b = _serial->read();
           break;
        }
        if('K' == b)
        {
          break;
        }
      }
    }
  }
 
  while( (a = _serial->read()) != -1)
  {
    //Wait until all other response chars are received
  }
}
