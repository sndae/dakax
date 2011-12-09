/* --------------------------------------------------------------------------
 * dakaXSerialTest
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 * On OSX and Linux can have troubles with upload the sketch if you use a lot
 * of serial com over the serialPort where the upload goes.
 * If you have this error message:
 *   avrdude: stk500_getsync(): not in sync: resp=0x00
 * 
 * 1. Also this should work: Open the serialMonitor and then upload
 *
 * 2. Type this before you upload in the Terminalconsole(ttyUSB0 is your serialport):
 *   cat /dev/ttyUSB0 >> /dev/null
 * 
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
  
  // open terminal for output              
  Serial.begin(115200);              
  
  Serial.println("---------------------------");
  Serial.println("DakaX Serial Test\n");

  // start dakaX
  DakaX.begin(onDakaXMessage,
              DAKAX_WIRELESS_NO);    

} 

void loop() 
{  
  DakaX.update();
  
  if(printTime + printUpdate < millis() && 0)
  {
    printTime = millis();
    
    Serial.println("---");
    Serial.println("Sensor Values:");
    
    Serial.print("QuatA:\t");  Serial.print(DakaX.quatA()); Serial.print("\t"); 
    Serial.print("QuatB:\t");  Serial.print(DakaX.quatB()); Serial.print("\t");  
    Serial.print("QuatC:\t");  Serial.print(DakaX.quatC()); Serial.print("\t");  
    Serial.print("QuatD:\t");  Serial.print(DakaX.quatD()); Serial.print("\n"); 
    
    Serial.print("EulerX:\t");  Serial.print(DakaXImpl::radToDeg(DakaX.eulerX())); Serial.print("\t"); 
    Serial.print("EulerY:\t");  Serial.print(DakaXImpl::radToDeg(DakaX.eulerY())); Serial.print("\t"); 
    Serial.print("EulerZ:\t");  Serial.print(DakaXImpl::radToDeg(DakaX.eulerZ())); Serial.print("\n"); 
     
    Serial.print("AccelX:\t");  Serial.print(DakaX.accelX()); Serial.print("\t"); 
    Serial.print("AccelY:\t");  Serial.print(DakaX.accelY()); Serial.print("\t"); 
    Serial.print("AccelZ:\t");  Serial.print(DakaX.accelZ()); Serial.print("\n"); 
    
    Serial.print("GyroX:\t");  Serial.print(DakaX.gyroX()); Serial.print("\t"); 
    Serial.print("GyroY:\t");  Serial.print(DakaX.gyroY()); Serial.print("\t"); 
    Serial.print("GyroZ:\t");  Serial.print(DakaX.gyroZ()); Serial.print("\n"); 
    
    Serial.print("MagX:\t");  Serial.print(DakaX.magX()); Serial.print("\t"); 
    Serial.print("MagY:\t");  Serial.print(DakaX.magY()); Serial.print("\t"); 
    Serial.print("MagZ:\t");  Serial.print(DakaX.magZ()); Serial.print("\n"); 
   
    //delay(50);
    
  }
 
} 

void onDakaXMessage(int cmd,DakaXImpl& dakaX)
{
  unsigned char res;
  switch(cmd)
  {
  case CMD_SET_D0:
    // example
    res = dakaX.serialCom().getByte();
    Serial.print("Set D0:\t");  Serial.println(res);
    digitalWrite(0, res > 0 ? HIGH : LOW);
    break;
  }
}

