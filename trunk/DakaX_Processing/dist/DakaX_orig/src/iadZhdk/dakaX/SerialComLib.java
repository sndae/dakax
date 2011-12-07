/* ----------------------------------------------------------------------------
 *  Serial Communication Lib							
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */

package iadZhdk.dakaX;

import processing.core.*; 
import processing.serial.*;
import java.lang.reflect.Method;
import java.nio.*;

public class SerialComLib
{
  final static int Ok = 0;
  final static int Error_Command = 1;
  final static int Error_Checksum = 2;
  final static int Error_Timeout = 3;
  final static int Error_DataHeader = 4;
  final static int Error_Data = 5;
  final static int Error_DataSize = 6;
  final static int Error_NoData = 7;

  final static int MAX_BUFFER_SIZE = 255;
  final static int MIN_PACKET_LENGTH = 5; //2(header) + 1(commandId) + 1(length) + 1(checksum) = 5
  final static int DATAHEADER1 = 0x1a;
  final static int DATAHEADER2 = 0xd0;

  protected int     _bufferLength;
  protected int     _bufferPos;
  protected int[]   _buffer;

  protected int     _inBufferLength;
  protected int     _inBufferPos;
  protected int[]   _inBuffer;

  protected int    _cmdId;
  protected int    _inCmdId;

  protected Serial   _serial;
  protected PApplet  _parent;
  protected Object   _cbRef;

  protected Method  _onReceiveSerialComMethod;
  protected Lock	_lock;

  public SerialComLib(PApplet parent, String serialPort, int baudRate)
  {
	_lock = new Lock();
    _parent = parent;
    _cbRef = parent;
    
    _serial = new Serial(parent, serialPort, baudRate) ;

    _buffer = new int[MAX_BUFFER_SIZE];
    _bufferLength = 0;
    _bufferPos = 0;

    _inBuffer = new int[MAX_BUFFER_SIZE];
    _inBufferLength = 0;
    _inBufferPos = 0;

    _cmdId = 0;
	_inCmdId = 0;

    _onReceiveSerialComMethod = getMethodRef(_parent,"onReceiveSerialCom", new Class[] { int.class,SerialComLib.class });
  }

 public SerialComLib(PApplet parent,Object cbRef, String serialPort, int baudRate)
  {
 	_lock = new Lock();
    _parent = parent;
    _cbRef = cbRef;
    _serial = new Serial(parent, serialPort, baudRate) ;

    _buffer = new int[MAX_BUFFER_SIZE];
    _bufferLength = 0;
    _bufferPos = 0;

    _inBuffer = new int[MAX_BUFFER_SIZE];
    _inBufferLength = 0;
    _inBufferPos = 0;

    _cmdId = 0;
	_inCmdId = 0;

    _onReceiveSerialComMethod = getMethodRef(_cbRef,"onReceiveSerialCom", new Class[] { int.class,SerialComLib.class });
  }

  public void update()
  {
    if(readPacket() == Ok)
    {  
      try 
      {
        _onReceiveSerialComMethod.invoke(_cbRef, new Object[] {_cmdId, this});
      } 
      catch (Exception e) 
      {
      }
    }  
  } 

  public static Method getMethodRef(Object obj, String methodName, Class[] paraList)
  {
    Method	ret = null;
    try {
      ret = obj.getClass().getMethod(methodName, paraList);
    } 
    catch (Exception e) 
    { // no such method, or an error.. which is fine, just ignore
    }
    return ret;
  }

  // send packet

  public void beginSend(int cmdId)
  {
	_lock.lock();

    _bufferPos = 0;
    _cmdId = cmdId;
  }

  public void sendByte(byte data)
  {
    _buffer[_bufferPos++] = data;
  }

  public void sendByte(int data)
  {
    _buffer[_bufferPos++] = data;
  }

  public void sendUShort(int data)
  {
    _buffer[_bufferPos++] = (byte)(0xff & data);
    _buffer[_bufferPos++] = (byte)((0xff00 & data) >> 8);
  }

  public void sendString(String str)
  {
	_buffer[_bufferPos++] = str.length();
    if(str.length() > 0)
    {
        for(int i=0;i < str.length();i++)
            _buffer[_bufferPos++] = str.charAt(i);
    }
  }

  public void sendFloat(float data)
  {
    /*
     float val = Float.intBitsToFloat(((_buffer[_bufferPos+3]&0xff)<<24)|
                                      ((_buffer[_bufferPos+2]&0xff)<<16)|
                                      ((_buffer[_bufferPos+1]&0xff)<<8) |
                                       (_buffer[_bufferPos+0]&0xff));       
    */                                       
  }

  public void endSend()
  {
    writeCmd(_cmdId, _bufferPos, _buffer);

	_lock.unlock();
  }

  protected byte checksum(int len, int[] data)
  {
    int checkSum = 0;
    for (int i=0;i < len;i++)
      checkSum += data[i];
    return((byte)(0xff - (0xff & checkSum)));
  }

  protected boolean testChecksum(int len, int[] data, int checksumVal)
  {
    return checksum(len, data) == checksumVal;
  }


  protected void writeCmd(int cmdId, int dataLen, int[] data)
  {
    // write header
    _serial.write((byte)DATAHEADER1);
    _serial.write((byte)DATAHEADER2);

    // write commandId
    _serial.write((byte)cmdId);

    // write data
    _serial.write((byte)dataLen);
    if (dataLen == 0)
      _serial.write((byte)0xff);  // checksum
    else
    {
      for (int i=0;i<dataLen;i++)
        _serial.write((byte)data[i]);

      // calc the checksume and write it
      _serial.write(checksum(dataLen, data));
    }
  } 

  
   // readn packet
   int readPacket()
   {
     if(available() == false)
       return Error_DataHeader;
    
    // debug
    /*
     int count = _serial.available();
     _parent.println("_serial.available(): " + _serial.available());
     for(int i=0;i < count;i++)
       _parent.print(hex(_serial.read()) + ",");
     println();
     return Error_DataHeader;
   */
      if(findDataHeader() == false)
        return Error_DataHeader;
        
      _inCmdId = 0;
      _inBufferLength = 0;
      _inBufferPos = 0;
      return readCmd();          
   }
   
   int getCmd()
   {
    return _inCmdId;
   }   
   
   int getCmdLength()
   {
    return _inBufferLength;
   }
   
   int getByte()
   {
      if(_inBufferPos < _inBufferLength)
          return _inBuffer[_inBufferPos++];
      return 0;   
   }
   
   int getUShort()
   {
     if(_inBufferPos+1 < _inBufferLength)
     {     
        return((_inBuffer[_inBufferPos++] & 0xff) + ((_inBuffer[_inBufferPos++] & 0xff) << 8 ));
     }
     return 0;  
   }
   
   int getInt()
   {
     if(_inBufferPos+1 < _inBufferLength)
     {    
//		return((_buffer[_bufferPos++] & 0xff) + ((_buffer[_bufferPos++] & 0xff) << 8 ));
		return( (short)((_inBuffer[_inBufferPos++] & 0xff) + ((_inBuffer[_inBufferPos++] & 0xff) << 8 )));
	  /*
		short val = (short)((_buffer[_bufferPos] & 0xff) + ((_buffer[_bufferPos+1] & 0xff) << 8 ));
       _bufferPos += 2;
		return val;        
	  */
     }
     return 0;  
   }
   
  public int getString(String str)
  {
	  str = "";
	  if(_inBufferPos < _inBufferLength)
	  {		  
		  int len = _inBuffer[_inBufferPos++];
		  if((len > 0) &&
			(_inBufferPos + len < _inBufferLength))
		  {			  
			  for(int i=0;i < len;i++)
				  str += _inBuffer[_inBufferPos++];
		  }

		  return len;
	  }
	  return 0;
  }

   float getFloat()
   {  
     float val = Float.intBitsToFloat(((_inBuffer[_inBufferPos+3]&0xff)<<24)|
                                      ((_inBuffer[_inBufferPos+2]&0xff)<<16)|
                                      ((_inBuffer[_inBufferPos+1]&0xff)<<8) |
                                       (_inBuffer[_inBufferPos+0]&0xff));   
                 
      _inBufferPos += 4;                                 
      return val;                                 
                                 
   }
   
   boolean isPacketEnd()
   {
     return(_inBufferPos >= _inBufferLength);
   }
   
   int[] getData()
   {
     return _inBuffer;
   }
   
   
   public boolean available()
   {
     return (_serial.available() >= MIN_PACKET_LENGTH)? true : false;
   }
   
   
   public int readCmd()
   {
     _inBufferPos = 0;
     _inBufferLength =0;
     
     // read cmd id
     _inCmdId = _serial.read();
     
     // read data length
     _inBufferLength = _serial.read();  
     
     // read data
	 int maxCount =1000;
	 while(_inBufferLength+1 > _serial.available() && maxCount-- > 0)
	 {
	  	_parent.delay(1);
	 }

	 if(_inBufferLength <= _serial.available())
	 {
	  if(_inBufferLength < _inBuffer.length)
	  {
		for(int i=0;i<_inBufferLength;i++)
		  _inBuffer[i] = (byte)_serial.read();
	  }       
	  else
		return Error_DataSize;
	 }
	 else
	 {
		//System.out.println("11111111111111111111");
		return Error_DataSize;
	 }
     /*
     _parent.println("_cmdId: " + hex(_cmdId));
     _parent.println("_bufferLength: " + _bufferLength);
     */
     
     // read checksum
     if(testChecksum(_inBufferLength,_inBuffer,(byte)_serial.read()))
     {
	  //System.out.println("ok");
	  return Ok;	
	 }
	

     return Error_Checksum;
   }
   
   protected boolean findDataHeader()
   { 
     if(_serial.read() == DATAHEADER1)
     {
       if(_serial.read() == DATAHEADER2)
         return true;
     }
    return false;
  }
  
 
}

