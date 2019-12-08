#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define CLIENT_NAME "espMatrix"

const int PIN = 13;

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 32, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

int phase = 0; // 0 red, 1 red yellow, 2 green, 3 yellow
int countdown = 3;
int x = 0;

int16_t start = 8;
int16_t end = 16;

uint8_t red = 255;
uint8_t green = 0;
uint16_t color = matrix.Color(red, green, 0);

void drawBar(uint16_t y, int widthOffset, uint16_t color) {
  uint16_t x0 = widthOffset;
  uint16_t x1 = 7 - widthOffset;

  matrix.drawLine(x0, y, x1, y, color);
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(color);


  matrix.fillScreen(0);
  matrix.show();
}

void loop() {
  matrix.fillScreen(0);

  if (countdown > 0) {
    int mod = millis() % 1000;
    Serial.print(mod < 40 && x > 4);
    Serial.print(" ");
    Serial.print(mod);
    Serial.print(" ");
    Serial.print(x);
    Serial.println();
    if (mod < 100 && x > 4) {
      x = 0;
      countdown--;
    }

    if (phase == 0) {
      start = 8;
      end = 16;
      red = 255;
      green = 0;
    } else if (phase == 1) {
      start = 8;
      end = 24;

      red = 255;
      green = 255;
    } else if (phase == 2) {
      start = 24;
      end = 32;

      red = 0;
      green = 255;
    } else {
      start = 16;
      end = 24;

      red = 255;
      green = 255;
    }
  } else /* cooldown == 0 */ {
    if (phase == 0) {
      start = 8;
      end = 16 + x;

      red = 255;
      green = x * 31;

      if (x > 7) {
        x = 0;
        phase = 1;
        countdown = 1;
      }
    } else if (phase == 1) {
      start = 8 + x * 2;
      end = 24 + x;

      red = 255 - (31 * x);
      green = 255;

      if (x > 7) {
        x = 0;
        phase = 2;
        countdown = 19;
      }
    } else if (phase == 2) {
      start = 24 - x;
      end = 32 - x;

      red = x * 31;
      green = 255;

      if (x > 7) {
        x = 0;
        phase = 3;
        countdown = 1;
      }
    } else {
      start = 16 - x;
      end = 24 - x;

      red = 255;
      green = 255 - (31 * x);

      if (x > 7) {
        phase = 0;
        x = 0;
        countdown = 12;
      }
    }
  }

  color = matrix.Color(red, green, 0);

  if ((phase == 0 || phase == 2) && countdown > 0 && countdown < 20) {
    matrix.setTextColor(color);
    if (countdown > 9) {
      matrix.setCursor(-1, 0);
      matrix.print(countdown / 10);

      matrix.setCursor(3, 0);
      matrix.print(countdown % 10);
    } else {
      matrix.setCursor(2, 0);
      matrix.print(countdown % 10);
    }
  }

  drawBar(start, 2, color);
  drawBar(start + 1, 1, color);

  int length = end - start;
  for (int i = 2; i < length - 2; i++) {
    drawBar(start + i, 0, color);
  }

  drawBar(end - 2, 1, color);
  drawBar(end - 1, 2, color);

  matrix.show();

  // Serial.print(phase);
  // Serial.print(" ");
  // Serial.print(countdown);
  // Serial.print(" ");
  // Serial.print(x);
  // Serial.print(" ");
  // Serial.print(millis());
  // Serial.println();

  x++;
  delay(30);
}
