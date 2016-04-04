/*
Version: 0.1
This code was written by Grook.net
Please feel free to edit/add any thing to the code but email it back to us for sharing it 
www.grook.net
http://www.youtube.com/GROOKnet
https://www.facebook.com/GROOKnet
*/

import processing.serial.*;
Serial port;
Serial port2;
String data = "";
String Radius = "";
String Theta = "";
int index = 0;
float distance = 0;
float angle = 0;
float pi = 22.0/7; 

int xmax = 500;
int ymax = 500;

void setup() 
{
 size( 500, 500);
 background(255,255,255);
  ellipse( xmax/2, ymax/2, xmax, ymax );
 line( xmax/2, 0, xmax/2, 1000 );
 line( 0, ymax/2, xmax , ymax/2 );
 line( xmax/2 ,ymax/2, xmax, 0 );
 line( xmax/2, ymax/2, 0, 0 );

port = new Serial(this, "/dev/ttyS0", 9600);
port.bufferUntil('.');
}

void draw()

{


}

void serialEvent(Serial port)
{
  data = port.readStringUntil('.');
  data = data.substring(0, data.length() - 1);  
  index = data.indexOf(",");
  Radius = data.substring(0, index);
  Theta = data.substring (index+1 , data.length());
  
   translate(xmax/2, ymax/2 );
   point (0,0);
   
      distance = float(Radius); 
      angle = float(Theta) /180 * pi; 
      fill(30,200,30);
      ellipse(distance * cos(angle) ,  -1 * distance * sin(angle) , 5,5);
}