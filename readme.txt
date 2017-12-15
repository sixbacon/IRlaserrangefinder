IR rangefinder on ESP8266 sending readings to an 0.96" OLED display. The sketch is written to be used with the Arduino IDE. 
Both the sesnor and display are connected on an I2C interface

You will need the ESP8266 board manager for the Arduino IDE - https://github.com/esp8266/Arduino

You will also need the Pololu library for the VL53L0X sensor, downloadable in the Arduino linrary manager, 
filter for "VL53LOX". This one works with the ESP8266.

For the OLED display you need to download the libary from - https://github.com/squix78/esp8266-oled-ssd1306
I tried a few before I found one that worked.
