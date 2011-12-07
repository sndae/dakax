/* ----------------------------------------------------------------------------
 *  Serial Communication Lib							
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */

package iadZhdk.dakaX;

import processing.core.*; 
import processing.core.PApplet;

import java.lang.reflect.Method;
import java.lang.Math.*;

public class DakaX extends Thread
{
  public final static int DAKAX_MSGID_PING         = 0;
  public final static int DAKAX_MSGID_SENSORDATA   = 1;
  
  public final static int DAKAX_MSG_ID_USER        = 100;
  
  
  protected SerialComLib  _serialCom;
  protected PApplet _parent;
  protected Method  _onDakaXMessage; 
  protected boolean _sendFlag = true;
  protected int _sendTime = 0;
  protected int _sendTimeStep = 1000 / 100;	// update rate of the sensor
  
  protected float[] _euler = new float[3];
  protected float[] _quat = new float[4];
  protected float[] _homeQuat = null;

  protected int[] _accel = new int[3];
  protected int[] _gyro = new int[3];
  protected int[] _mag = new int[3];

  protected boolean _running = false;
  protected int		_sleepTime = 2;	// update to check if there is any serial data

  
  public DakaX(PApplet cbRef,
               String portName,
               int    baudRate)
  {

    _parent = cbRef;
    _serialCom = new SerialComLib(cbRef,this,
                                  portName, baudRate);
      
    _onDakaXMessage = SerialComLib.getMethodRef(_parent,"onDakaXMessage", new Class[] { int.class,DakaX.class });
	
	_sendTimeStep = (int)(1000.0f / (baudRate / 8.0f / (39.0f * 8f)) + .5f);
	//_sendTimeStep = 12;
	System.out.println("_sendTimeStep: " + _sendTimeStep);
    // start the thread
	start();
  }
  
  public SerialComLib serialCom() { return _serialCom; }

  public void setSerialUpdateRate(int updatesPerSecond)
  {
	  _sendTimeStep = 1000 / updatesPerSecond;
  }

  public int serialUpdateRate() { return _sendTimeStep; }

  public void start()
  {
	_running = true;
	setPriority(MAX_PRIORITY );
	super.start();
  }
  
  public void run()
  {
	while(_running && !_parent.finished )
	{
	  _serialCom.update(); 

	  if(_sendTime + _sendTimeStep < _parent.millis())
	  {
	  // $test
		requestSensorData();

		_sendTime = _parent.millis(); 
		_sendFlag = false;
	  }

      try 
	  {
        sleep((long)(_sleepTime));
      }
	  catch (Exception e) 
	  {}

	}
	//System.out.println("end thread");
/*
	while(_running)
	{
	  _serialCom.update(); 

	  requestSensorData();

      try 
	  {
        sleep((long)(50));
      }
	  catch (Exception e) 
	  {}

	}
*/
  }

  public void quit()
  {
	_running = false;
	interrupt();
	_serialCom = null;
  }
  
  public void update()
  {
/*
    _serialCom.update(); 
    if(_sendTime + _sendTimeStep < _parent.millis())
    {
       requestSensorData();
       //PApplet.print("-");
      _sendTime = _parent.millis(); 
      _sendFlag = false;
    }
*/

  }
  
  
  public void requestSensorData()
  {    
    _serialCom.beginSend(DAKAX_MSGID_SENSORDATA);
    _serialCom.endSend(); 
  }
    
  public void onReceiveSerialCom(int cmdId,SerialComLib serialCom)
  {  
    _sendFlag = true;
    //_parent.print(",");  
    
    if(cmdId < DAKAX_MSG_ID_USER)
    {
      switch(cmdId)
      {
      case DAKAX_MSGID_PING:
        //println("DIN3: " + serialCom.getByte());     
        break;
      case DAKAX_MSGID_SENSORDATA:
        // read quaternion
        _quat[0] = serialCom.getFloat();
        _quat[1] = serialCom.getFloat();
        _quat[2] = serialCom.getFloat();
        _quat[3] = serialCom.getFloat();

        // read accelorometer
        _accel[0] = serialCom.getInt();
        _accel[1] = serialCom.getInt();
        _accel[2] = serialCom.getInt();

        // read gyroscope
        _gyro[0] = serialCom.getInt();
        _gyro[1] = serialCom.getInt();
        _gyro[2] = serialCom.getInt();

        // read mag
        _mag[0] = serialCom.getInt();
        _mag[1] = serialCom.getInt();
        _mag[2] = serialCom.getInt();

        // recalc values
        recalc();
        break;
      default:
        // unknown command
        break;     
      }
    }
    else
    {  // user message
      try 
      {
		_onDakaXMessage.invoke(_parent, new Object[] {cmdId, this});
      } 
      catch (Exception e) 
      {
      }
    }
  }
 
  public void setHomeRot()
  {
    _homeQuat = quatConjugate(_quat);
  }
  
  public void resetHomeRot()
  {
    _homeQuat = null;
  }

  public float[] quat() { return _quat; }
  public float[] euler() { return _euler; }

  public float eulerX() { return _euler[0]; }
  public float eulerY() { return _euler[1]; }
  public float eulerZ() { return _euler[2]; }

  public int[] accel() { return _accel; }
  public int accelX() { return _accel[0]; }
  public int accelY() { return _accel[1]; }
  public int accelZ() { return _accel[2]; }

  public int[] gyro() { return _gyro; }
  public int gyroX() { return _gyro[0]; }
  public int gyroY() { return _gyro[1]; }
  public int gyroZ() { return _gyro[2]; }

  public int[] mag() { return _mag; }
  public int magX() { return _mag[0]; }
  public int magY() { return _mag[1]; }
  public int magZ() { return _mag[2]; }
 
  protected void recalc()
  {
    if(_homeQuat == null)
      quaternionToEuler(_quat,_euler);
    else
      // calc with offset
      quaternionToEuler(quatProd(_homeQuat, _quat), _euler);      
    /*
        _parent.println("recalc:");
        _parent.println("q0= " + _quat[0]);
        _parent.println("q1= " + _quat[1]);
        _parent.println("q2= " + _quat[2]);
        _parent.println("q3= " + _quat[3]);

        _parent.println("euler0= " + PApplet.degrees(_euler[0]));
        _parent.println("euler1= " + PApplet.degrees(_euler[1]));
        _parent.println("euler2= " + PApplet.degrees(_euler[2]));
    */        
   }
  
  // code is taken from the freeImu project
  public static void quaternionToEuler(float[] q, float[] euler) 
  {
    euler[0] = PApplet.atan2(2.0f * q[1] * q[2] - 2.0f * q[0] * q[3], 2.0f * q[0]*q[0] + 2.0f * q[1] * q[1] - 1.0f); // psi
    euler[1] = -PApplet.asin(2.0f * q[1] * q[3] + 2.0f * q[0] * q[2]); // theta
    euler[2] = PApplet.atan2(2.0f * q[2] * q[3] - 2.0f * q[0] * q[1], 2.0f * q[0] * q[0] + 2.0f * q[3] * q[3] - 1.0f); // phi
  }
  
  public static float [] quatProd(float [] a, float [] b) 
  {
    float [] q = new float[4];
  
    q[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3];
    q[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2];
    q[2] = a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1];
    q[3] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0];
    return q;
  }
  
   // returns a quaternion from an axis angle representation
  public float []quatAxisAngle(float [] axis, float angle) 
  {
    float [] q = new float[4];
  
    float halfAngle = angle / 2.0f;
    float sinHalfAngle = PApplet.sin(halfAngle);
    q[0] = PApplet.cos(halfAngle);
    q[1] = -axis[0] * sinHalfAngle;
    q[2] = -axis[1] * sinHalfAngle;
    q[3] = -axis[2] * sinHalfAngle;
  
    return q;
  } 
  
  // return the quaternion conjugate of quat
  public static float []quatConjugate(float [] quat) 
  {
    float [] conj = new float[4];

    conj[0] = quat[0];
    conj[1] = -quat[1];
    conj[2] = -quat[2];
    conj[3] = -quat[3];
  
    return conj;
  }
}
