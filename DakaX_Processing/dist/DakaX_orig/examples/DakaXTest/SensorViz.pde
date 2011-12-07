/* ----------------------------------------------------------------------------
 *  SensorViz
 * ----------------------------------------------------------------------------
 *  prog: max.rheiner@zhdk.ch / Zhdk / Interaction Design
 *  date: 11/16/2011
 * ----------------------------------------------------------------------------
 */
 

void drawRefObj(float rotX,float rotY,float rotZ)
{
  pushMatrix();
     // sensor rotation
     /*
    rotateZ(-rotZ);
    rotateX(-rotX);
    rotateY(-rotY);
    */
    rotateZ(-rotZ);
    rotateX(-rotY);
    rotateY(-rotX);
    
    
    /*
    rotateZ(-dakaX.euler()[2]);
    rotateX(-dakaX.euler()[1]);
    rotateY(-dakaX.euler()[0]);
    */
    
    drawCoordSys();

    scale(2);
    drawBox();
  popMatrix();
}
 
void drawCoordSys()
{
  pushMatrix();  
    scale(100,100,100);    
    
    // x axis
    stroke(255,110,110);
    line(0,0,0,1,0,0);
    
    // y axis
    stroke(110,255,110);
    line(0,0,0,0,1,0);
    
    // z axis
    stroke(110,110,255);
    line(0,0,0,0,0,1);
    
  popMatrix();
}

void drawBox() 
{
  noStroke();
  beginShape(QUADS);
  
  //Z+ (to the drawing area)
  fill(#0000ff);
  vertex(-30, -5, 20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);
  
  //Z-
  fill(#00ffff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, 5, -20);
  vertex(-30, 5, -20);
  
  //X-
  fill(#ffff00);
  vertex(-30, -5, -20);
  vertex(-30, -5, 20);
  vertex(-30, 5, 20);
  vertex(-30, 5, -20);
  
  //X+
  fill(#ff0000);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(30, 5, -20);
  
  //Y-
  fill(#ff00ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(-30, -5, 20);
  
  //Y+
  fill(#00ff00);
  vertex(-30, 5, -20);
  vertex(30, 5, -20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);
  
  endShape();
}
