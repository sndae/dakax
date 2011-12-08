/* ----------------------------------------------------------------------------
 *  DakaX Processing Library
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
	static 
	{	// load the nativ shared lib
		System.loadLibrary("DakaX");

		System.out.println("DakaX Version " + versionStr());
	}
  
  protected final static int DAKAX_VERSION = 12;	// version x.xx

  public final static int DAKAX_MSGID_PING         		= 0;
  public final static int DAKAX_MSGID_SENSORDATA   		= 1;
  public final static int DAKAX_MSGID_SENSORDATA_AUTO   = 2;	// unsigned short for the update frequency
  
  public final static int DAKAX_MSG_ID_USER        = 100;
  
  
  protected SerialComLib  	_serialCom;
  protected PApplet 		_parent;
  protected Method  		_onDakaXMessage; 
  protected boolean 		_sendFlag = true;
  protected int 			_sendTime = 0;
  protected int 			_sendTimeStep = 1000 / 100;	// update rate of the sensor
  
  protected float[] 	_euler = new float[3];
  protected float[] 	_quat = new float[4];
  protected float[] 	_homeQuat = new float[4];
  protected float[] 	_curQuat = new float[4];
  protected boolean		_homeQuatFlag = false;

  protected int[] 		_accel = new int[3];
  protected int[] 		_gyro = new int[3];
  protected int[] 		_mag = new int[3];

  protected boolean _running = false;
  protected int		_sleepTime = 2;	// update to check if there is any serial data

  protected boolean		_open = false;

  public DakaX(PApplet cbRef,
               String portName,
               int    baudRate)
  {

    _parent = cbRef;
    _serialCom = new SerialComLib(portName, baudRate,
								  this);
      
    _onDakaXMessage = SerialComLib.getMethodRef(_parent,"onDakaXMessage", new Class[] { DataPacket.class,DakaX.class });
	
	_sendTimeStep = (int)(1000.0f / (baudRate / 8.0f / (39.0f * 8f)) + .5f);
	_sendTimeStep = (int)(1000.0f / 60.0f);
	
	System.out.println("_sendTimeStep: " + _sendTimeStep);

	_parent.registerDispose(this);

	if(_serialCom.isOpen())
	{  // start the thread
	  _open = true;
	  start();	
	}
	else
	  _open = false;
  }
  
  public boolean isOpen() { return _open; }

  public static int version()
  {
	return DAKAX_VERSION;
  }

  public static String versionStr()
  {
	return String.format("%.2f", (float)DAKAX_VERSION / 100.0f);
  }

  public void dispose()
  {
	_running = false;
	try{
	  wait();
	}
	catch(Exception e)
	{}
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
	super.start();
  }

  public void run()
  {
	while(_running && !_parent.finished )
	{
	  //_serialCom.update(); 

	  if(_sendTime + _sendTimeStep < _parent.millis())
	  {
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
  
	_serialCom.close();
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
    
  public void update()
  {
//     _serialCom.update(); 
//     if(_sendTime + _sendTimeStep < _parent.millis())
//     {
//        requestSensorData();
//        //PApplet.print("-");
//       _sendTime = _parent.millis(); 
//       _sendFlag = false;
//     }

  }
  
  public void requestSensorData()
  {    
    _serialCom.beginSend(DAKAX_MSGID_SENSORDATA);
    _serialCom.endSend(); 
  }
    
  public void onNewPacket(DataPacket packet)
  {      
	//packet.println();

    if(packet.cmd() < DAKAX_MSG_ID_USER)
    {
      switch(packet.cmd())
      {
      case DAKAX_MSGID_PING:
        // should do here something 
        break;
      case DAKAX_MSGID_SENSORDATA:
        // read quaternion
        _quat[0] = packet.getFloat();
        _quat[1] = packet.getFloat();
        _quat[2] = packet.getFloat();
        _quat[3] = packet.getFloat();

        // read accelorometer
        _accel[0] = packet.getShort();
        _accel[1] = packet.getShort();
        _accel[2] = packet.getShort();

        // read gyroscope
        _gyro[0] = packet.getShort();
        _gyro[1] = packet.getShort();
        _gyro[2] = packet.getShort();

        // read mag
        _mag[0] = packet.getShort();
        _mag[1] = packet.getShort();
        _mag[2] = packet.getShort();

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
		_onDakaXMessage.invoke(_parent, new Object[] {packet, this});
      } 
      catch (Exception e) 
      {
      }
    }
  }


  public void setHomeRot()
  {
    _homeQuat = quatConjugate(_quat);
	_homeQuatFlag = true;
  }
  
  public void resetHomeRot()
  {
	_homeQuatFlag = false;
  }

  public float[] quat() { return _quat; }
  public float[] quatHome() { return _homeQuat; }
  public float[] curQuat() { return _curQuat; }

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
    if(_homeQuatFlag == false)
    {
	  _curQuat[0] = _quat[0];
	  _curQuat[1] = _quat[1];
	  _curQuat[2] = _quat[2];
	  _curQuat[3] = _quat[3];

	  quaternionToEuler(_quat,_euler);
	}
    else
	{
	  //_curQuat = quatProd(_quat,_homeQuat);
	  _curQuat = quatProd(_homeQuat,_quat);

      // calc with offset
      quaternionToEuler(quatProd(_homeQuat, _quat), _euler);      

//         _parent.println("recalc:");
//         _parent.println("q0= " + _quat[0]);
//         _parent.println("q1= " + _quat[1]);
//         _parent.println("q2= " + _quat[2]);
//         _parent.println("q3= " + _quat[3]);
// 
//         _parent.println("euler0= " + PApplet.degrees(_euler[0]));
//         _parent.println("euler1= " + PApplet.degrees(_euler[1]));
//         _parent.println("euler2= " + PApplet.degrees(_euler[2]));
	}
      
   }
 
  // quaternion order
  // q[0] = w 
  // q[1] = a = x
  // q[2] = b = y
  // q[3] = c = z

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


  public void getRotMatrix(PMatrix3D rotMat)
  {
	getRotMatrix(curQuat(),rotMat);
  }

  public static void getRotMatrix(float[] quat, PMatrix3D rotMat)
  {
	/*
	float x = quat[1];
	float y = quat[2];
	float z = quat[3];
	float w = quat[0];
	*/

	float x = -quat[1];
	float y = quat[3];
	float z = quat[2];
	float w = quat[0]; 

	float xx      = x * x;
	float xy      = x * y;
	float xz      = x * z;
	float xw      = x * w;

	float yy      = y * y;
	float yz      = y * z;
	float yw      = y * w;

	float zz      = z * z;
	float zw      = z * w;

	rotMat.m00 = 1.0f - 2.0f * ( yy + zz );
	rotMat.m01 =     2.0f * ( xy - zw );
	rotMat.m02 =     2.0f * ( xz + yw );

	rotMat.m10 =     2.0f * ( xy + zw );
	rotMat.m11 = 1.0f - 2.0f * ( xx + zz );
	rotMat.m12 =     2.0f * ( yz - xw );

	rotMat.m20 =     2.0f * ( xz - yw );
	rotMat.m21 =     2.0f * ( yz + xw );
	rotMat.m22 = 1.0f - 2.0f* ( xx + yy );

	rotMat.m03 = rotMat.m13 = rotMat.m23 = rotMat.m30 = rotMat.m31 = rotMat.m32 = 0.0f;
	rotMat.m33 = 1.0f;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // drawing helpers
  public void drawRefObj(PMatrix3D rotMat)
  {
	_parent.pushMatrix();
	  // sensor rotation
	  _parent.applyMatrix(rotMat);
	  //scale(.5,.5,.5);
	  drawCoordSys();

	  _parent.scale(2);
	  drawBox();
	_parent.popMatrix();
  }

  public void drawRefObj(float rotX,float rotY,float rotZ)
  {
	_parent.pushMatrix();
	  // sensor rotation
  /*
	  rotateZ(-rotZ);
	  rotateX(-rotY);
	  rotateY(-rotX);
  */  
	  _parent.rotateZ(-rotY);
	  _parent.rotateX(rotZ);
	  _parent.rotateY(-rotX);
	  
	  drawCoordSys();

	  _parent.scale(2);
	  drawBox();
	_parent.popMatrix();
  }
  
  public void drawCoordSys()
  {
	_parent.pushMatrix();  
	  _parent.scale(100,100,100);    
	  
	  // x axis
	  _parent.stroke(255,110,110);
	  _parent.line(0,0,0,1,0,0);
	  
	  // y axis
	  _parent.stroke(110,255,110);
	  _parent.line(0,0,0,0,1,0);
	  
	  // z axis
	  _parent.stroke(110,110,255);
	  _parent.line(0,0,0,0,0,1);
	  
	_parent.popMatrix();
  }

  public void drawBox() 
  {
	_parent.noStroke();
	_parent.beginShape(_parent.QUADS);
	
	//Z+ (to the drawing area)
	_parent.fill(0,0,255);
	_parent.vertex(-20, -5, 30);
	_parent.vertex(20, -5, 30);
	_parent.vertex(20, 5, 30);
	_parent.vertex(-20, 5, 30);
	
	//Z-
	_parent.fill(0,255,255);
	_parent.vertex(-20, -5, -30);
	_parent.vertex(20, -5, -30);
	_parent.vertex(20, 5, -30);
	_parent.vertex(-20, 5, -30);
	
	//X-
	_parent.fill(255,255,0);
	_parent.vertex(-20, -5, -30);
	_parent.vertex(-20, -5, 30);
	_parent.vertex(-20, 5, 30);
	_parent.vertex(-20, 5, -30);
	
	//X+
	_parent.fill(255,0,0);
	_parent.vertex(20, -5, -30);
	_parent.vertex(20, -5, 30);
	_parent.vertex(20, 5, 30);
	_parent.vertex(20, 5, -30);
	
	//Y-
	_parent.fill(255,0,255);
	_parent.vertex(-20, -5, -30);
	_parent.vertex(20, -5, -30);
	_parent.vertex(20, -5, 30);
	_parent.vertex(-20, -5, 30);
	
	//Y+
	_parent.fill(0,255,0);
	_parent.vertex(-20, 5, -30);
	_parent.vertex(20, 5, -30);
	_parent.vertex(20, 5, 30);
	_parent.vertex(-20, 5, 30);
	
	_parent.endShape();
  }
  
}
