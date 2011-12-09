/* --------------------------------------------------------------------------
 * DakaXIMU 
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 * The IMU code is mainly based on the great work of Noah Zerkin / /MPU-6050 6-axis DMP Demo v0.01
 * ----------------------------------------------------------------------------
 */

#ifndef _DAKAXIMU_H
#define _DAKAXIMU_H

#include "WProgram.h"
#include <Wire.h>

class DakaXIMUImpl
{
public:
  DakaXIMUImpl():
      _foundChip(false)
  {}
  
  bool begin();
  void update();
  void sendQuat(HardwareSerial& serial);
  
  float  _quat[4];
  
protected:
    
  void serialPrintFloatArr(HardwareSerial& serial,float * arr, int length);
  void serialFloatPrint(HardwareSerial& serial,float f);

  bool _foundChip;
  
};

#endif // _DAKAXIMU_H
