/* ----------------------------------------------------------------------------
 *  OsciGraph
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */
  
  
import iadZhdk.dakaX.*;

public class OsciGraph
{
  protected ArrayList _sampleList;
  protected int  _samples;
  protected color _clrFrame;
  protected color _clrData;
  protected float _dataRangeStart;
  protected float _dataRangeEnd;
  
  public OsciGraph(float dataRangeStart,float dataRangeEnd,
                   int samples,
                   color lineClr,
                   color frameClr)
  {
    _dataRangeStart = dataRangeStart;
    _dataRangeEnd = dataRangeEnd;
     _sampleList = new ArrayList(samples);
     _samples = samples;
     _clrData = lineClr;
     _clrFrame = frameClr;
  }

  public void addSample(float sample)
  {
    _sampleList.add(sample); 
   if(_sampleList.size() > _samples)
     _sampleList.remove(0);    
  }
  
  float lastValue()
  {
    if(_sampleList.size() > 0)
      return (Float)_sampleList.get(_sampleList.size() - 1);
    return 0.0f;
  }
  
  public void draw(int x,int y,int width,int height)
  {
    pushStyle();
    
    // draw frame
    strokeWeight(1);
    noFill();
    stroke(_clrFrame);
    
    // horiz line
    line(x,y,x+width,y);
    
    
    // vertical line
    float faqV = (float)height / (float)(_dataRangeEnd - _dataRangeStart);
    int top = (int)(-_dataRangeEnd * faqV +.5f);
    int bottom = (int)(-_dataRangeStart * faqV +.5f);
   
    line(x,top+y,x,bottom+y); 
    
    // data line
    stroke(_clrData);
    
    float faqH = (float)width / (float)_samples;
    beginShape();
    for(int i=0;i < _samples && i < _sampleList.size();i++)
    {
      Float val = (Float)_sampleList.get(i);
      vertex(x + width - i * faqH, - val * faqV + y);
    }
    endShape();
    
    popStyle();
  }  
                   
}

public class IMUGraph
{
  OsciGraph _accelGraphX;
  OsciGraph _accelGraphY;
  OsciGraph _accelGraphZ;
  OsciGraph _gyroGraphX;
  OsciGraph _gyroGraphY;
  OsciGraph _gyroGraphZ;
  OsciGraph _magGraphX;
  OsciGraph _magGraphY;
  OsciGraph _magGraphZ;
  OsciGraph _eulerGraphX;
  OsciGraph _eulerGraphY;
  OsciGraph _eulerGraphZ;
  
  DakaX      _dakaX;
  PApplet    _parent;

  public IMUGraph(int sampleSize,DakaX dakaX,PApplet parent)
  {
    _dakaX = dakaX;
    _parent = parent;
    
   
    _accelGraphX = new OsciGraph(-32768,32768,
                                 sampleSize,
                                 color(255,0,0,200),
                                 color(255,255,255,200));  
    _accelGraphY = new OsciGraph(-32768,32768,
                                 sampleSize,
                                 color(0,255,0,200),
                                 color(255,255,255,200));  
    _accelGraphZ = new OsciGraph(-32768,32768,
                                 sampleSize,
                                 color(50,50,255,200),
                                 color(255,255,255,200));  

    _gyroGraphX = new OsciGraph(-32768,32768,
                                 sampleSize,
                                 color(255,0,0,200),
                                 color(255,255,255,200));  
    _gyroGraphY = new OsciGraph(-32768,32768,
                                 sampleSize,
                                 color(0,255,0,200),
                                 color(255,255,255,200));  
    _gyroGraphZ = new OsciGraph(-32768,32768,
                                 sampleSize,
                                 color(50,50,255,200),
                                 color(255,255,255,200));  

    _magGraphX = new OsciGraph(-4096,4096,
                                 sampleSize,
                                 color(255,0,0,200),
                                 color(255,255,255,200));  
    _magGraphY = new OsciGraph(-4096,4096,
                                 sampleSize,
                                 color(0,255,0,200),
                                 color(255,255,255,200));  
    _magGraphZ = new OsciGraph(-4096,4096,
                                 sampleSize,
                                 color(50,50,255,200),
                                 color(255,255,255,200));  

    _eulerGraphX = new OsciGraph(-180,180,
                                 sampleSize,
                                 color(255,0,0,200),
                                 color(255,255,255,200));  
    _eulerGraphY = new OsciGraph(-180,180,
                                 sampleSize,
                                 color(0,255,0,200),
                                 color(255,255,255,200));  
    _eulerGraphZ = new OsciGraph(-180,180,
                                 sampleSize,
                                 color(50,50,255,200),
                                 color(255,255,255,200));  
  }
  
  public void update()
  {
    _accelGraphX.addSample(dakaX.accelX());
    _accelGraphY.addSample(dakaX.accelY());
    _accelGraphZ.addSample(dakaX.accelZ());
    
    _gyroGraphX.addSample(dakaX.gyroX());
    _gyroGraphY.addSample(dakaX.gyroY());
    _gyroGraphZ.addSample(dakaX.gyroZ());
    
    _magGraphX.addSample(dakaX.magX());
    _magGraphY.addSample(dakaX.magY());
    _magGraphZ.addSample(dakaX.magZ());
    
    /*
    _eulerGraphX.addSample(degrees(dakaX.eulerX()));
    _eulerGraphY.addSample(degrees(dakaX.eulerY()));
    _eulerGraphZ.addSample(degrees(dakaX.eulerZ()));    
    */
    _eulerGraphX.addSample(degrees(dakaX.eulerZ()));
    _eulerGraphY.addSample(degrees(dakaX.eulerY()));
    _eulerGraphZ.addSample(degrees(dakaX.eulerX()));    
  }
  
  public void draw()
  {
    pushMatrix();
    ortho();
    pushStyle();
    noStroke();
    fill(0,0,0,100);
    rect(0,0,width,height);
    
    int yPos=80;
    int hSize = 140;
    
    _accelGraphX.draw(10,yPos,_parent.width -20 ,hSize);
    _accelGraphY.draw(10,yPos,_parent.width -20 ,hSize);
    _accelGraphZ.draw(10,yPos,_parent.width -20 ,hSize);
    fill(255,100,100);
    text("AccelX: " + _accelGraphX.lastValue(), width - 200,yPos-75,0);
    fill(100,255,100);
    text("AccelY: " + _accelGraphY.lastValue(), width - 200,yPos-50,0);
    fill(100,100,255);
    text("AccelZ: " + _accelGraphZ.lastValue(), width - 200,yPos-25,0);
    
    yPos += hSize + 10;
    _gyroGraphX.draw(10,yPos,_parent.width -20 ,hSize);
    _gyroGraphY.draw(10,yPos,_parent.width -20 ,hSize);
    _gyroGraphZ.draw(10,yPos,_parent.width -20 ,hSize);
    fill(255,100,100);
    text("GyroX: " + _gyroGraphX.lastValue(), width - 200,yPos-75,0);
    fill(100,255,100);
    text("GyroY: " + _gyroGraphY.lastValue(), width - 200,yPos-50,0);
    fill(100,100,255);
    text("GyroZ: " + _gyroGraphZ.lastValue(), width - 200,yPos-25,0);
 
     yPos += hSize + 10;
    _magGraphX.draw(10,yPos,_parent.width -20 ,hSize);
    _magGraphY.draw(10,yPos,_parent.width -20 ,hSize);
    _magGraphZ.draw(10,yPos,_parent.width -20 ,hSize);
    fill(255,100,100);
    text("MagX: " + _magGraphX.lastValue(), width - 200,yPos-75,0);
    fill(100,255,100);
    text("MagY: " + _magGraphY.lastValue(), width - 200,yPos-50,0);
    fill(100,100,255);
    text("MagZ: " + _magGraphZ.lastValue(), width - 200,yPos-25,0);
    
     yPos += hSize + 10;
    _eulerGraphX.draw(10,yPos,_parent.width -20 ,hSize);
    _eulerGraphY.draw(10,yPos,_parent.width -20 ,hSize);
    _eulerGraphZ.draw(10,yPos,_parent.width -20 ,hSize);
    fill(255,100,100);
    text("EulerX: " + _eulerGraphX.lastValue(), width - 200,yPos-75,0);
    fill(100,255,100);
    text("EulerY: " + _eulerGraphY.lastValue(), width - 200,yPos-50,0);
    fill(100,100,255);
    text("EulerZ: " + _eulerGraphZ.lastValue(), width - 200,yPos-25,0);
    
    popStyle();
    popMatrix();
   
  }
}
