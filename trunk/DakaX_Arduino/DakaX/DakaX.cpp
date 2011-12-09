/* --------------------------------------------------------------------------
 * DakaXImpl
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
 * date:  11/12/2011 (m/d/y)
 * ----------------------------------------------------------------------------
 * Main wrapper for the daka
 * ----------------------------------------------------------------------------
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <math.h>
#include "WProgram.h"

#include "DakaX.h"

///////////////////////////////////////////////////////////////////////////////
// global var
DakaXImpl DakaX;

///////////////////////////////////////////////////////////////////////////////
// defines
#define    DAKAX_RF_SERIAL  Serial1

#define    DAKAX_MSGID_PING             0
#define    DAKAX_MSGID_SENSORDATA       1

///////////////////////////////////////////////////////////////////////////////
// DakaXImpl

DakaXImpl::DakaXImpl():
    _messageRet(NULL),
    _eulerCalc(false)
{
}

bool DakaXImpl::begin(MessageReceiveFunc retFunc,
                      unsigned char wirelessType,
                      unsigned long baudrate,
                      const char* wirelessName,
                      const char* pin)
{
    _messageRet = retFunc;

    // activate i2c
    pinMode(23, OUTPUT);
    digitalWrite(23, LOW);
    delay(500);

    // wireless
    switch(wirelessType)
    {
    case DAKAX_WIRELESS_BLUETOOTH:
        _bluetoothImpls.init(Serial1,
                             wirelessName,
                             pin,
                             baudrate);
        // start serial port
        _serialCom.open(Serial1,baudrate);
        break;
    case DAKAX_SERIAL:
        // wrap com over serial port
        _serialCom.open(Serial,baudrate);
        break;
    }
   
    // start sensors
   _magSensor.initialize();;
   _imuSensor.initialize();;

   // start imu
    _imuImpl.begin();



    return true;
}

void DakaXImpl::setBtModuleBaud(unsigned long baudrate,
                                unsigned long curBaudrate)
{
    _bluetoothImpls.setBaudRate(Serial1,
                                baudrate,
                                curBaudrate);
}


void DakaXImpl::update()
{
    // update the sensor data
    _imuImpl.update();
    _eulerCalc = false;

    // read out sensor data
    _imuSensor.getMotion6(&(_accel[0]), &(_accel[1]), &(_accel[2]),
                          &(_gyro[0]),  &(_gyro[1]),  &(_gyro[2]));
    _magSensor.getHeading(&(_mag[0]), &(_mag[1]), &(_mag[2]));

    // update the sensor data
    // check if there is serial data
    if(_serialCom.isOpen() &&
       _serialCom.readPacket() == SerialComLib::Ok)
    { // there is some data on the serial port
        // interprete the command
        if(_serialCom.getCmd() >= DAKAX_MSG_ID_USER)
        {   // pass message to client
            (*_messageRet)(_serialCom.getCmd(),*this);
        }
        else
        {
            switch(_serialCom.getCmd())
            {
            case DAKAX_MSGID_PING:
                break;
            case DAKAX_MSGID_SENSORDATA:
                // send all sensordata
                _serialCom.beginSend(DAKAX_MSGID_SENSORDATA);
                // quat
                _serialCom.sendFloat(_imuImpl._quat[0]); // a
                _serialCom.sendFloat(_imuImpl._quat[1]); // b
                _serialCom.sendFloat(_imuImpl._quat[2]); // c
                _serialCom.sendFloat(_imuImpl._quat[3]); // d

                // accel
                _serialCom.sendInt(_accel[0]); // a
                _serialCom.sendInt(_accel[1]); // a
                _serialCom.sendInt(_accel[2]); // a

                // gyro
                _serialCom.sendInt(_gyro[0]); // a
                _serialCom.sendInt(_gyro[1]); // a
                _serialCom.sendInt(_gyro[2]); // a

                // mag
                _serialCom.sendInt(_mag[0]); // a
                _serialCom.sendInt(_mag[1]); // a
                _serialCom.sendInt(_mag[2]); // a

                /*

                _serialCom.sendInt(-32001); // a
                _serialCom.sendInt(32001); // a
                _serialCom.sendInt(-32001); // a

                _serialCom.sendInt(-32002); // a
                _serialCom.sendInt(32002); // a
                _serialCom.sendInt(-32002); // a

                _serialCom.sendInt(-32003); // a
                _serialCom.sendInt(-32003); // a
                _serialCom.sendInt(-32003); // a
                */

                _serialCom.endSend();
                break;
            }
        }
    }

}

// data methods
float DakaXImpl::eulerX()
{
    if(!_eulerCalc)
        updateEuler();
    return _euler[1];
}


float DakaXImpl::eulerY()
{
    if(!_eulerCalc)
        updateEuler();
    return _euler[0];
}


float DakaXImpl::eulerZ()
{
    if(!_eulerCalc)
        updateEuler();
    return _euler[2];
}

void DakaXImpl::updateEuler()
{
    quaternionToEuler(_imuImpl._quat,_euler);
    _eulerCalc = true;
}

// code is taken from the freeImu project
void DakaXImpl::quaternionToEuler(float* q, float* euler)
{
  euler[0] = atan2(2.0f * q[1] * q[2] - 2.0f * q[0] * q[3], 2.0f * q[0]*q[0] + 2.0f * q[1] * q[1] - 1.0f); // psi
  euler[1] = -asin(2.0f * q[1] * q[3] + 2.0f * q[0] * q[2]); // theta
  euler[2] = atan2(2.0f * q[2] * q[3] - 2.0f * q[0] * q[1], 2.0f * q[0] * q[0] + 2.0f * q[3] * q[3] - 1.0f); // phi
}

float DakaXImpl::radToDeg(float val)
{
    static float faq = 180.0f / M_PI;
    return(faq * val);
}
