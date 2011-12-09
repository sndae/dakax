/* --------------------------------------------------------------------------
 * DakaXBluetoothTest
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 */

#include <Wire.h>
#include "DakaX.h"

#define CMD_SET_D0 (DAKAX_MSG_ID_USER + 1)
#define CMD_SET_PWM15 (DAKAX_MSG_ID_USER + 2)

void onDakaXMessage(int cmd,DakaXImpl& dakaX);

void setup() 
{ 
  // activate i2c
  pinMode(23, OUTPUT);     
  digitalWrite(23, LOW);    
  
  pinMode(0, OUTPUT);        
  pinMode(15, OUTPUT);   
  
  digitalWrite(0, HIGH);    
  analogWrite(15,0);
  
  // the baudrate has to be set just one time, 
  // even after power-off the module keeps the baudrate
  // Attention !!! You need to know the right baudrates
  //DakaX.setBtModuleBaud(38400,115200);    // set from 115100 to 38400
  //DakaX.setBtModuleBaud(115200,38400);    // set from 38400 to 115100
 

  DakaX.begin(onDakaXMessage,
              DAKAX_WIRELESS_BLUETOOTH,
              115200,          // baudrate       
              "dakaX_Bt",    // bluetooth name
              "0000");       // pinCode
           
} 

void loop() 
{ 
  DakaX.update();
} 

void onDakaXMessage(int cmd,DakaXImpl& dakaX)
{
  switch(cmd)
  {
  case CMD_SET_D0:
    // example
    digitalWrite(0, dakaX.serialCom().getByte() > 0 ? HIGH : LOW);
    break;
  case CMD_SET_PWM15:
    // example
    analogWrite(15,dakaX.serialCom().getByte());
    break;
  }
}

