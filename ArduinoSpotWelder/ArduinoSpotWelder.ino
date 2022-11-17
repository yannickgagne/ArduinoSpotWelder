/*
  Spot Welder Timer Example

  Check out the instructable here - http://www.instructables.com/id/DIY-Spot-Welder-From-Microwave/

  >>> Before compiling: Please install the u8glib library from below!

  Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/

  Jack Davies, http://jackdaviesdesign.com <<< >>> http://undergroundengineering.co.uk
*/

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);	// I2C / TWI

int potPin = 0;   // analog pin used to connect the potentiometer
int potVal;       // variable to read the value from the analog pin

int inPin = 2;
int triggerSwitch = 0;
int ssrPin = 3;
int stringWidth = 0;
unsigned long lastMs = 0;

void draw(void) {
  // graphic commands to redraw the complete screen
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 12, 10, "YG Spot Welder"); // rename to personalise ;)
  u8g.setScale2x2();
  u8g.setPrintPos(20, 20);
  u8g.print(potVal);
  u8g.undoScale();
  u8g.drawStr( 20, 60, "Milliseconds");

}

void triggerCallback(void) {
  triggerSwitch = HIGH;
}

int smoothAnalogRead(int pin) {
  int i;
  int value = 0;
  int numReadings = 20;

  for (i = 0; i < numReadings; i++){
    // Read sensor data.
    value = value + analogRead(pin);

    // 1ms pause adds more stability between reads.
    delay(1);
  }

  // Take an average of all the readings.
  value = value / numReadings;

  return value;
}

void setup(void) {
  delay(2500); //sanity wait
  
  // flip screen, if required
  // u8g.setRot180();

  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }

  Serial.begin(9600);
  pinMode(inPin, INPUT);
  pinMode(ssrPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(inPin), triggerCallback, FALLING);
  Serial.println(">>> MCU Ready <<<");
}

void loop(void) {
  if((millis() - lastMs) > 500) {
    // picture loop
    u8g.firstPage();
    do {
      draw();
    } while ( u8g.nextPage() );

    //Read pot value and map it
    potVal = smoothAnalogRead(potPin);          // reads the value of the potentiometer (value between 0 and 1023)
    potVal = map(potVal, 0, 1023, 10, 750);     // scale it to use it get the right time (value between 10 and 750)
  }
  //triggerSwitch = digitalRead(inPin);

  if (triggerSwitch == HIGH) {
    //Serial.print("SSR Start\r\n");
    digitalWrite(ssrPin, HIGH);
    delay(potVal);
    digitalWrite(ssrPin, LOW);
    //Serial.print("SSR Stop\r\n");
    delay(1000);
    triggerSwitch = LOW;
  }

}
