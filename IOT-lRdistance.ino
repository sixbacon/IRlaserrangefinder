/* This demonstrates the IR distance sensor the , mounted on module.
 * Laser rangefinder library from https://github.com/pololu/vl53l0x-arduino
 * The library for the oled display is available at https://github.com/squix78/esp8266-oled-ssd1306
 * Both the sensor module and the display are connected to the NodeMCU via the I2C bus 
 * To be able to shutdown the laser module,GPIO14/D5 is connected to the XSHUT connection on the module
 * 13/12/17
 */

#include "SSD1306.h"                             // library for display
#include <Wire.h>                                // I2C library
#include <VL53L0X.h>                             // laser ranefinder library

//I2C connections
#define SDA 0                                    // GPIO0 / D3
#define SCL 2                                    // GPIO2 / D4

//laser rangefinder
VL53L0X sensor;                                  // create an instance of a laser rangefinder

#define maxDistance 2000                         // maximum distance for display
#define sensorXSHUT 14                           // sensor shut down pin connected to D5/GPIO14                                                
const boolean ON = HIGH;                         
const boolean OFF = LOW;

void switchLaserRangefinder(boolean state){       
// this is only used to switch the display on in this sketch but for low power applications it can be
// used to switch the VL53L0X OFF 
  pinMode(sensorXSHUT, OUTPUT);
  digitalWrite(sensorXSHUT, state);
  if (state == ON) {
    Serial.println("Laser Rangefinder powered up");  
  }
  else {
    Serial.println("Laser Rangefinder powered down");
  }
  delay(300);
}

void setupLaserRangefinder() {                   // configuration taken from Pololu example
  switchLaserRangefinder(ON);
  sensor.init();
  sensor.setTimeout(500);
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
  Serial.println("Laser range finder configured");  
}

//OLED display
#define I2Caddress 0x3c                          // address of the display

SSD1306 display(I2Caddress, SDA, SCL);           // create a instance of an OLED display

void setupOLEDdisplay() {
  display.init();                                // initialise the oled display
  display.flipScreenVertically();                // gives orientation the same as the pin labels
  display.setFont(ArialMT_Plain_10);             // set the font type and size  
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting");
  delay(20);
  Wire.begin(SDA, SCL);                          // setup the I2C interface
  setupLaserRangefinder();
  delay(200);
  setupOLEDdisplay(); 
}
// end of setup


// prepare the graphics display to show a bar representing the distance the target is from the sensor
// function to fill in the bar up to the current reading (value)
int width=80;                                    // size of bar in display units
int height=20;                                   // height of bar in display units


void drawBar(int x, int y, int w, int h, int value) {
  float posReal;                                 // needed to calculate the width of the bar to fill in
  int posInt;                                    // converts the real width to an integer one
  
  display.drawRect(x, y, w, h);                  // draw the outline box
  posReal= 1.0*value*width/maxDistance;          // 1.0* needed to force real arithmetic
  posInt=posReal;
  display.fillRect(x, y, posInt, h);             // fill int the bar to the value indicated
  display.drawString(x+w+2,y,String(value));     // print the value at the end of the bar
}

// variable for main loop
long reading;

void loop() {  
  display.clear();                               // remove the last image displayed
  // start assembling the image 
  // x goes from 0 - 127, left to right 
  // y goes from 0 - 63, top to bottom
  // insert the title
  display.drawString(0,0,"ESP Laser Rangefinder");
  display.drawString(0,18, "Distance in mm");    // add the bar name
  do {                                           // repeat until valid reading obtained
    reading = sensor.readRangeSingleMillimeters();
  } while (reading > 5000);                     // occaisonally 8190 = 0x1FFE is returned, no documentation on error codes
                                                // though the fact that they exist is mentioned in datasheet
  Serial.print("Reading / mm - ");
  Serial.println(reading);
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  drawBar(0,34,width,height,reading);            // add the bar
  display.display();                             // transfer the assembled image to the display
  delay(500);
}
