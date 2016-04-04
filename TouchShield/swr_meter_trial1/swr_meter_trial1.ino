
const byte lcdW = 320 ; // The width of the display
const byte lcdH = 240 ; // The height of the display
const byte fontWidth = 10 ;
const byte fontHeigth = 30 ;
const byte totSpan = 9;
const float spanValue[totSpan] =
{
  25000.0f, 50000.0f,100000.0f, 250000.0f,500000.0f, 1000000.0f, 2500000.0f, 5000000.0f,10000000.0f
};

struct settings_t
{
    int spanIdx;
    float freqCenter;
}
settings;

void creaGrid()
{
    byte p0 = lcdH - 2;
    for ( int i = 0 ;i < 9; i++ )  {
        Tft.drawVerticalLine((i*40),fontHeigth,164,RED); //240*320
    }
    for (int i = 1 ;i < 2; i++) {
        Tft.drawHorizontalLine(0,i*82+fontHeigth,319,RED); //240
    }
    Tft.drawCircle(160,164+fontHeigth,5,GREEN);
    if (settings.freqCenter >9999999) {
      Tft.drawFloat(settings.freqCenter/1000000,130,214,2,YELLOW);
    }
    else {
      Tft.drawFloat(settings.freqCenter/1000000,130,214,2,YELLOW);
    }
    float s = spanValue[settings.spanIdx]/8;
    for (int i = 0 ;i < 9; i++) {
        double freq = (settings.freqCenter + (i-4)*s)/1000000;
        Tft.drawFloat(freq,(i*40),185,1,YELLOW);
    }
    if (spanValue[settings.spanIdx] >9999999) {
      Tft.drawFloat(spanValue[settings.spanIdx]/1000000,260,214,2,YELLOW);
    }
    else {
      Tft.drawFloat(spanValue[settings.spanIdx]/1000000,260,214,2,YELLOW);
    }
    Tft.drawRectangle(0,fontHeigth,320,164, GREEN);
    Tft.drawString( "SWR",0,0,2,CYAN );
    Tft.drawFloat( minSwr,40,0,2,GREEN );
    Tft.drawFloat( maxSwr,120, 0,2,RED );
    if ( !modSpan ) {
      Tft.fillRectangle(40,210, 60,20, CYAN);
      Tft.drawString("freq",40, 210 ,2,BLUE);
      Tft.drawString("span",210, 210 ,2,BLUE);
    }
    else {
      Tft.fillRectangle(210,210, 50,20, CYAN);
      Tft.drawString("freq",40, 210 ,2,BLUE);
      Tft.drawString("span",210, 210 ,2,BLUE);
    }
}

void setup() {}

void loop() {}


