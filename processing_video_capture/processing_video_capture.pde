import processing.video.*;
Capture myCapture;

void setup() 
{
  size(1000, 800);
  //background (255);    // Set bg to white
  //noStroke();          //uncomment this line to remove the stroke
  frameRate(10000);       // Set framerate
  smooth();            // Antialiasing
  myCapture = new Capture(this, width, height, 30);
}

void captureEvent(Capture myCapture) {
  myCapture.read();
}

void draw() {
  float pointillize = map(mouseX, 0, width, 2, 20);
  int x = int(random(myCapture.width));
  int y = int(random(myCapture.height));
  color pix = myCapture.get(x, y);
  fill(pix, 255);
  ellipse(x, y, pointillize, pointillize);
 //rect(x, y, pointillize, pointillize); //uncomment this line to use squares
}

// SAVE IMAGE
float saveincr;
void mousePressed() 
{
 saveincr++;
save("image"+saveincr+".jpg");
}