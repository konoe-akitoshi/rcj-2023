 //------------------------------------------
// Tested Module    : Arduino NANO / Atmega328
// Drive IC         : OLED(SSD1306)55
// INTERFACE        : I2C
// VDD              : 2.7V-5.5V
//------------------------------------------
//
// LineSnser Tunenig program Ver. 1.0
// Dec.08.2022 By A.Saeki
//

#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000
};

const int Ledthreshold = 510;

const int HEARTBEAT = 6; // Built in LED -> D10

/* Set the delay between fresh samples */

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                    ADO=LOW  > Address = 0x28
//                                    ADO=Open > Address = 0x29
//                                   id, address
//Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/

void setup(void)
{
  pinMode(HEARTBEAT, OUTPUT);
  digitalWrite(HEARTBEAT, HIGH);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while (1) {
      digitalWrite(HEARTBEAT, LOW);
      delay(100);
      Serial.println("Ooops, no SD1302... Check your wiring or I2C ADDR!");
      digitalWrite(HEARTBEAT, HIGH);
      delay(100);
    }
  }

  delay(500);
  digitalWrite(HEARTBEAT, LOW);
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(500); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  digitalWrite(HEARTBEAT, HIGH);
  delay(500);
  digitalWrite(HEARTBEAT, LOW);

}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
  int line1, line2, line3, line4, line5;
  // Possible vector values can be:

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.clearDisplay();
  display.setCursor( 40, 0);     // Start at top-left corner by dot(x,y)
  display.print("Line Senser");

  line1 = analogRead(A0); // front
  line2 = analogRead(A1); // right
  line3 = analogRead(A2); // back
  line4 = analogRead(A3); // left
  line5 = analogRead(A6); // center

  display.setCursor(44, 20);  // front:1
  if (line1 < Ledthreshold) {
    display.setTextColor(BLACK, WHITE); // Draw Reverse text
  } else {
    display.setTextColor(WHITE); // Draw white text
  }
  display.print("1");
  display.setTextColor(WHITE); // Draw white text
  display.print(" ");
  display.print(line1, DEC);

  display.setCursor(88, 36);  // right:2
  if (line2 < Ledthreshold) {
    display.setTextColor(BLACK, WHITE); // Draw Reverse text
  } else {
    display.setTextColor(WHITE); // Draw white text
  }
  display.print("2");
  display.setTextColor(WHITE); // Draw white text
  display.print(" ");
  display.print(line2, DEC);

  display.setCursor(44, 56);  // back:3
  if (line3 < Ledthreshold) {
    display.setTextColor(BLACK, WHITE); // Draw Reverse text
  } else {
    display.setTextColor(WHITE); // Draw white text
  }
  display.print("3");
  display.setTextColor(WHITE); // Draw white text
  display.print(" ");
  display.print(line3, DEC);

  display.setCursor(00, 36);  // left:4
  if (line4 < Ledthreshold) {
    display.setTextColor(BLACK, WHITE); // Draw Reverse text
  } else {
    display.setTextColor(WHITE); // Draw white text
  }
  display.print("4");
  display.setTextColor(WHITE); // Draw white text
  display.print(" ");
  display.print(line4, DEC);

  display.setCursor(44, 36);  // center:5
  if (line5 < Ledthreshold) {
    display.setTextColor(BLACK, WHITE); // Draw Reverse text
  } else {
    display.setTextColor(WHITE); // Draw white text
  }
  display.print("5");
  display.setTextColor(WHITE); // Draw white text
  display.print(" ");
  display.print(line5, DEC);

  display.display();

  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  delay(200);
}
