// All the mcufriend.com UNO shields have the same pinout.
// i.e. control pins A0-A4.  Data D2-D9.  microSD D10-D13.
// Touchscreens are normally A1, A2, D7, D6 but the order varies
//
// This demo should work with most Adafruit TFT libraries
// If you are not using a shield,  use a full Adafruit constructor()
// e.g. Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#include <SPI.h>          // f.k. for Arduino-1.5.2
#include <Adafruit_GFX.h> // Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;   

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

void setup(void);
void loop(void);
unsigned long testFillScreen();
unsigned long testText();
unsigned long testLines(uint16_t color);
unsigned long testFastLines(uint16_t color1, uint16_t color2);
unsigned long testRects(uint16_t color);
unsigned long testFilledRects(uint16_t color1, uint16_t color2);
unsigned long testFilledCircles(uint8_t radius, uint16_t color);
unsigned long testCircles(uint8_t radius, uint16_t color);
unsigned long testTriangles();
unsigned long testFilledTriangles();
unsigned long testRoundRects();
unsigned long testFilledRoundRects();
void progmemPrint(const char *str);
void progmemPrintln(const char *str);

void runtests(void);

uint16_t g_identifier;

void setup(void) {
    static uint16_t identifier;
    tft.reset();                 //we can't read ID on 9341 until begin()
    g_identifier = tft.readID(); //
    if (g_identifier == 0) g_identifier = 0x9325;
    tft.begin(g_identifier);
//    tft.begin();
}

void loop(void) {
    uint8_t aspect;
    runtests();
    delay(2000);
    if (tft.height() > 64) {
        for (aspect = 0; aspect < 4; aspect++) {
            tft.setRotation(aspect);
            testText();
            delay(1000);
            uint16_t maxscroll;
            if (tft.getRotation() & 1) maxscroll = tft.width();
            else maxscroll = tft.height();
            for (uint16_t i = 0; i < maxscroll; i++) {
                tft.vertScroll(0, maxscroll, i);
                delay(10);
            }
            tft.vertScroll(0, maxscroll, 0);
            delay(1000);
        }
    }
    tft.setRotation(0);
}

typedef struct {
    PGM_P msg;
    uint32_t ms;
} TEST;
TEST result[20];

#define RUNTEST(n, str, test) { result[n].msg = PSTR(str); result[n].ms = test; delay(500); }

void runtests(void)
{
    uint8_t i;
    uint32_t total;
    RUNTEST(0, "FillScreen               ", testFillScreen());
    RUNTEST(1, "Text                     ", testText());
    RUNTEST(2, "Lines                    ", testLines(CYAN));
    RUNTEST(3, "Horiz/Vert Lines         ", testFastLines(RED, BLUE));
    RUNTEST(4, "Rectangles (outline)     ", testRects(GREEN));
    RUNTEST(5, "Rectangles (filled)      ", testFilledRects(YELLOW, MAGENTA));
    RUNTEST(6, "Circles (filled)         ", testFilledCircles(10, MAGENTA));
    RUNTEST(7, "Circles (outline)        ", testCircles(10, WHITE));
    RUNTEST(8, "Triangles (outline)      ", testTriangles());
    RUNTEST(9, "Triangles (filled)       ", testFilledTriangles());
    RUNTEST(10, "Rounded rects (outline)  ", testRoundRects());
    RUNTEST(11, "Rounded rects (filled)   ", testFilledRoundRects());

    tft.setRotation(0);
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.println("MCUFRIEND UNO shield");

    tft.setTextSize(1);
    total = 0;
    for (i = 0; i < 12; i++) {
        PGM_P str = result[i].msg;
        char c;
        tft.print(i);
        tft.print(": ");
        while (c = pgm_read_byte(str++)) tft.print(c);
        tft.println(result[i].ms);
        total += result[i].ms;
    }
    tft.setTextSize(2);
    g_identifier = tft.readID();
    tft.print("ID: 0x");
    tft.println(tft.readID(), HEX);
    tft.print("Total: ");
    tft.print(total);
    tft.println("us");

    delay(10000);
}

unsigned long testFillScreen() {
    unsigned long start = micros();
    tft.fillScreen(BLACK);
    tft.fillScreen(RED);
    tft.fillScreen(GREEN);
    tft.fillScreen(BLUE);
    tft.fillScreen(BLACK);
    return micros() - start;
}

unsigned long testText() {
    unsigned long start;
    tft.fillScreen(BLACK);
    start = micros();
    tft.setCursor(0, 0);
    tft.setTextColor(WHITE);  tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(YELLOW); tft.setTextSize(2);
    tft.println(1234.56);
    tft.setTextColor(RED);    tft.setTextSize(3);
    tft.println(0xDEADBEEF, HEX);
    //	tft.println(g_identifier, HEX);
    tft.println();
    tft.setTextColor(GREEN);
    tft.setTextSize(5);
    tft.println("Groop");
    tft.setTextSize(2);
    tft.println("I implore thee,");
    tft.setTextSize(1);
    tft.println("my foonting turlingdromes.");
    tft.println("And hooptiously drangle me");
    tft.println("with crinkly bindlewurdles,");
    tft.println("Or I will rend thee");
    tft.println("in the gobberwarts");
    tft.println("with my blurglecruncheon,");
    tft.println("see if I don't!");
    return micros() - start;
}

unsigned long testLines(uint16_t color) {
    unsigned long start, t;
    int           x1, y1, x2, y2,
                  w = tft.width(),
                  h = tft.height();

    tft.fillScreen(BLACK);

    x1 = y1 = 0;
    y2    = h - 1;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = w - 1;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    t     = micros() - start; // fillScreen doesn't count against timing

    tft.fillScreen(BLACK);

    x1    = w - 1;
    y1    = 0;
    y2    = h - 1;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = 0;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    t    += micros() - start;

    tft.fillScreen(BLACK);

    x1    = 0;
    y1    = h - 1;
    y2    = 0;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = w - 1;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    t    += micros() - start;

    tft.fillScreen(BLACK);

    x1    = w - 1;
    y1    = h - 1;
    y2    = 0;
    start = micros();
    for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
    x2    = 0;
    for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

    return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
    unsigned long start;
    int           x, y, w = tft.width(), h = tft.height();

    tft.fillScreen(BLACK);
    start = micros();
    for (y = 0; y < h; y += 5) tft.drawFastHLine(0, y, w, color1);
    for (x = 0; x < w; x += 5) tft.drawFastVLine(x, 0, h, color2);

    return micros() - start;
}

unsigned long testRects(uint16_t color) {
    unsigned long start;
    int           n, i, i2,
                  cx = tft.width()  / 2,
                  cy = tft.height() / 2;

    tft.fillScreen(BLACK);
    n     = min(tft.width(), tft.height());
    start = micros();
    for (i = 2; i < n; i += 6) {
        i2 = i / 2;
        tft.drawRect(cx - i2, cy - i2, i, i, color);
    }

    return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
    unsigned long start, t = 0;
    int           n, i, i2,
                  cx = tft.width()  / 2 - 1,
                  cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    n = min(tft.width(), tft.height());
    for (i = n; i > 0; i -= 6) {
        i2    = i / 2;
        start = micros();
        tft.fillRect(cx - i2, cy - i2, i, i, color1);
        t    += micros() - start;
        // Outlines are not included in timing results
        tft.drawRect(cx - i2, cy - i2, i, i, color2);
    }

    return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
    unsigned long start;
    int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

    tft.fillScreen(BLACK);
    start = micros();
    for (x = radius; x < w; x += r2) {
        for (y = radius; y < h; y += r2) {
            tft.fillCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
    unsigned long start;
    int           x, y, r2 = radius * 2,
                        w = tft.width()  + radius,
                        h = tft.height() + radius;

    // Screen is not cleared for this one -- this is
    // intentional and does not affect the reported time.
    start = micros();
    for (x = 0; x < w; x += r2) {
        for (y = 0; y < h; y += r2) {
            tft.drawCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long testTriangles() {
    unsigned long start;
    int           n, i, cx = tft.width()  / 2 - 1,
                        cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    n     = min(cx, cy);
    start = micros();
    for (i = 0; i < n; i += 5) {
        tft.drawTriangle(
            cx    , cy - i, // peak
            cx - i, cy + i, // bottom left
            cx + i, cy + i, // bottom right
            tft.color565(0, 0, i));
    }

    return micros() - start;
}

unsigned long testFilledTriangles() {
    unsigned long start, t = 0;
    int           i, cx = tft.width()  / 2 - 1,
                     cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    start = micros();
    for (i = min(cx, cy); i > 10; i -= 5) {
        start = micros();
        tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(0, i, i));
        t += micros() - start;
        tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                         tft.color565(i, i, 0));
    }

    return t;
}

unsigned long testRoundRects() {
    unsigned long start;
    int           w, i, i2,
                  cx = tft.width()  / 2 - 1,
                  cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    w     = min(tft.width(), tft.height());
    start = micros();
    for (i = 0; i < w; i += 6) {
        i2 = i / 2;
        tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
    }

    return micros() - start;
}

unsigned long testFilledRoundRects() {
    unsigned long start;
    int           i, i2,
                  cx = tft.width()  / 2 - 1,
                  cy = tft.height() / 2 - 1;

    tft.fillScreen(BLACK);
    start = micros();
    for (i = min(tft.width(), tft.height()); i > 20; i -= 6) {
        i2 = i / 2;
        tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
    }

    return micros() - start;
}


