/* --------------------------------------------------------------------------
 * dakaXSerialTest
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 */

#include <Wire.h>    // has to be there, otherwise the Wire lib will not be included in DakaX
#include <DakaX.h>

#define CMD_SET_D0 (DAKAX_MSG_ID_USER + 1)

void onDakaXMessage(int cmd,DakaXImpl& daka);

unsigned long printUpdate = 1000;
unsigned long printTime = 0;

void setup() 
{ 
  // activate i2c
  pinMode(23, OUTPUT);     
  digitalWrite(23, LOW);    
 
  pinMode(0, OUTPUT);     
  digitalWrite(0, LOW);  
  
  // start dakaX
  DakaX.begin(onDakaXMessage,
              DAKAX_SERIAL,
              115200);    
} 

void loop() 
{  
  DakaX.update();
   
} 

void onDakaXMessage(int cmd,DakaXImpl& dakaX)
{
  unsigned char res;
  switch(cmd)
  {
  case CMD_SET_D0:
    // example
    res = dakaX.serialCom().getByte();
    digitalWrite(0, res > 0 ? HIGH : LOW);
    break;
  }
}

