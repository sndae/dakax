/* ----------------------------------------------------------------------------
 *  DakaXIMUNormal
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */
  
import iadZhdk.dakaX.*;
import processing.serial.*;

DakaX  dakaX;

PVector    dirNormal = new PVector(0,-100,0);
PVector    dir = new PVector();
PMatrix3D  rotMatrix = new PMatrix3D();

void setup()
{
  size(800,600,P3D);
  
  // check which os version and select the right serialPort
  String portName;
  String os = System.getProperty("os.name").toLowerCase();
  if(os.indexOf( "nix") >=0 || os.indexOf( "nux") >=0)
  {  // linux/unix
    //portName = "/dev/ttyUSB0";
    portName = "/dev/rfcomm0"; 
  }
  else if(os.indexOf( "mac" ) >= 0)
  {  // mac
    //portName = "/dev/tty.usbserial-FTF5HT10"; // cable
    portName = "/dev/tty.dakaX_Bt-DevB";  // bluetooth
  }
  else
  {  // windows
    portName = "COM9"; 
  }
  
  // setup the dakaX 
  println("Open SerialPort: " + portName );
  dakaX = new DakaX(this, 
                    portName, 
                    230400);
  // change update rate
  //dakaX.setSerialUpdateRate(50);                            
                    
  smooth();  
  strokeWeight(3);   
}

void draw()
{
  // update the sensor data
  dakaX.update();
  
  // get the current rotation matrix
  dakaX.getRotMatrix(rotMatrix);
  
  // draw the 3d graph obj
  background(0); 
  perspective(radians(60),float(width)/float(height),5,10);
  translate(width/2,height/2);
  /*
  
  dakaX.drawRefObj(dakaX.eulerX(),
                   dakaX.eulerY(),
                   dakaX.eulerZ()); 
 */
  dakaX.drawRefObj(rotMatrix);             
             
  // draw direction
  rotMatrix.mult(dirNormal,dir);

  stroke(200,200,0);
  line(0,0,0,dir.x,dir.y,dir.z);  
}


void keyPressed()
{
  switch(key)
  {
  case ' ':
    // set home pos
    dakaX.setHomeRot();
    break;
  case 'x':
    // reset home pos
    dakaX.resetHomeRot();
  // set the digital port
    break;
  }
}

void onDakaXMessage(DataPacket packet,DakaX dakaX)
{
  /*
  switch(packet.cmd())
  {
  case ????:
    packet.getByte();
    break;
  }
  */
}
