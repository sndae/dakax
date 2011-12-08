/* ----------------------------------------------------------------------------
 *  DakaXTest
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */
  
import iadZhdk.dakaX.*;
import processing.serial.*;

final static int CMD_SET_D0 = DakaX.DAKAX_MSG_ID_USER + 1;
final static int CMD_SET_PWM15 = DakaX.DAKAX_MSG_ID_USER + 2;

DakaX  dakaX;

IMUGraph graph;
PFont    font;
boolean  showGraph = true;
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
    portName = "/dev/rfcomm1";
  }
  else if(os.indexOf( "mac" ) >= 0)
  {  // mac
    portName = "/dev/tty.usbserial-FTF5HT10"; // cable
    //portName = "/dev/tty.dakaX_Bt-DevB";  // bluetooth
  }
  else
  {  // windows
    portName = "COM9";  // cable
    portName = "COM43";  // bluetooth
  }
  
  // setup the dakaX 
  println("Open SerialPort: " + portName );
  dakaX = new DakaX(this, 
                    portName, 
                    //115200);
                    230400);
  if(dakaX.isOpen() == false)
  {
	println("Can't open SerialPort !");
	exit();
  }

  // change update rate
  //dakaX.setSerialUpdateRate(50);                    
                    
  // init the graphs                    
  graph = new IMUGraph(500,dakaX,this);
  font = loadFont("Ubuntu-20.vlw"); 
  textFont(font, 20);
  textAlign(LEFT, TOP);

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

  // draw graph
  if(showGraph)
  {
    graph.update();
    graph.draw();
  }    
}


void keyPressed()
{
  switch(key)
  {
  case 'g':
    showGraph = !showGraph;
    break;
  case ' ':
    // set home pos
    dakaX.setHomeRot();
    break;
  case 'x':
    // reset home pos
    dakaX.resetHomeRot();
  // set the digital port
    break;
  case '1':
    // off pin d0
    dakaX.serialCom().beginSend(CMD_SET_D0);
      dakaX.serialCom().sendByte(0);
    dakaX.serialCom().endSend();
    println("D0 -> ON");
    break;
  case '2':
    // on pin d0
    dakaX.serialCom().beginSend(CMD_SET_D0);
      dakaX.serialCom().sendByte(100);
    dakaX.serialCom().endSend();
    println("DO -> OFF");
    break;
  case '3':
    // on pin d0
    dakaX.serialCom().beginSend(CMD_SET_PWM15);
      dakaX.serialCom().sendByte(0);
    dakaX.serialCom().endSend();
    println("AO -> " + 0);
    break;
  case '4':
    // on pin d0
    dakaX.serialCom().beginSend(CMD_SET_PWM15);
      dakaX.serialCom().sendByte(100);
    dakaX.serialCom().endSend();
    println("AO -> " + 100);
    break;
  case '5':
    // on pin d0
    dakaX.serialCom().beginSend(CMD_SET_PWM15);
      dakaX.serialCom().sendByte(255);
    dakaX.serialCom().endSend();
    println("AO -> " + 255);
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
