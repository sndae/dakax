/* --------------------------------------------------------------------------
 * DakaXImpl
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 * Main wrapper for the daka
 * ----------------------------------------------------------------------------
 */

#ifndef _DAKAX_H
#define _DAKAX_H

#include "WProgram.h"
//#include "Wire.h"

#include "serialComLib.h"
#include "DakaXIMU.h"
#include "BluetoothBee.h"

///////////////////////////////////////////////////////////////////////////////
// Jeff Rowberg's  i2c devlib
// http://www.i2cdevlib.com/
#include <MPU6050.h>
#include <HMC5883L.h>

///////////////////////////////////////////////////////////////////////////////
// dakaX defines
#define DAKAX_MSG_ID_USER    100

#define DAKAX_WIRELESS_NO           0
#define DAKAX_WIRELESS_BLUETOOTH    1
#define DAKAX_WIRELESS_XBEE         2
#define DAKAX_SERIAL                3

#define DAKAX_WIRELESS_NAME         "dakaX"
#define DAKAX_WIRELESS_PIN          "0000"

#define DAKAX_VERISON        0001   // 1234 = 12.34 version

///////////////////////////////////////////////////////////////////////////////
// callback def
class DakaXImpl;

typedef void (*MessageReceiveFunc)(int cmd,DakaXImpl& daka);

///////////////////////////////////////////////////////////////////////////////
// DakaXImpl

class DakaXImpl
{
public:

  enum WirelessType{
        Wireless_No         = DAKAX_WIRELESS_NO,
        Wireless_Bluetooth  = DAKAX_WIRELESS_BLUETOOTH,
        Wireless_Xbee       = DAKAX_SERIAL,
  };

  DakaXImpl();
  
  bool begin(MessageReceiveFunc retFunc,
             unsigned char wirelessType,
             unsigned long baudrate = 38400,
             const char* wirelessName = DAKAX_WIRELESS_NAME,
             const char* pin = DAKAX_WIRELESS_PIN);
  void setBtModuleBaud(unsigned long baudrate = 38400,
                       unsigned long curBaudrate = 38400);
  void update();
  
  SerialComLib& serialCom() { return _serialCom; }
  
  // data methods
  float eulerX();
  float eulerY();
  float eulerZ();


  float quatA() const { return _imuImpl._quat[0]; }
  float quatB() const { return _imuImpl._quat[1]; }
  float quatC() const { return _imuImpl._quat[2]; }
  float quatD() const { return _imuImpl._quat[3]; }

  float accelX() const { return _accel[0]; }
  float accelY() const { return _accel[1]; }
  float accelZ() const { return _accel[2]; }

  float gyroX() const { return _gyro[0]; }
  float gyroY() const { return _gyro[1]; }
  float gyroZ() const { return _gyro[2]; }

  float magX() const { return _mag[0]; }
  float magY() const { return _mag[1]; }
  float magZ() const { return _mag[2]; }

  static void quaternionToEuler(float* q, float* euler);
  static float radToDeg(float val);

protected:

  void updateEuler();


  SerialComLib          _serialCom;
  MessageReceiveFunc    _messageRet;

  // sensor
  DakaXIMUImpl          _imuImpl;
  HMC5883L              _magSensor;
  MPU6050               _imuSensor;

  //WirelessImple*        _wirelessImpl;
  BluetoothBeeImpl      _bluetoothImpls;

  bool                  _eulerCalc;
  float                 _euler[3];

  int16_t               _accel[3];
  int16_t               _gyro[3];
  int16_t               _mag[3];

};

///////////////////////////////////////////////////////////////////////////////
// extern var. dekl
extern DakaXImpl DakaX;

#endif // _DAKAX_H
