/* ----------------------------------------------------------------------------
 *  DakaX Processing Library
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */

package iadZhdk.dakaX;

import java.lang.reflect.Method;
import java.lang.Math.*;

public class SerialComLib extends SerialComLibBase
{
  
  //protected SerialComLib  _serialCom;
  protected Object  _cbObj; 
  protected Method  _onNewPacket; 

  public SerialComLib(String portName,int baudRate,Object cbObj)
  {
	_cbObj = cbObj;
	_onNewPacket = getMethodRef(_cbObj,"onNewPacket", new Class[] { DataPacket.class });

	open(portName,baudRate);
  }

  public void newPacket(DataPacket packet)
  {
	try {
	  _onNewPacket.invoke(_cbObj, new Object[] { packet });
	} 
	catch (Exception e) 
	{
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
}
