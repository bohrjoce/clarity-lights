#include <FastGPIO.h>
#define APA102_USE_FAST_GPIO
#include <APA102.h>

// Define pins
const uint8_t dataPin = 11;
const uint8_t clockPin = 12;

// Objects and parameters for controlling LED strip
APA102<dataPin, clockPin> ledStrip;
const uint16_t ledCount = 240;
rgb_color colors[ledCount];
const uint8_t brightness = 1;

// Serial communication buffer
const int numBytes = 3;
byte bytesIn[numBytes];

void setup()
{
    Serial.begin(9600);
}

void loop()
{
  if(Serial.available() > 0)
  {
    Serial.readBytes((char*)bytesIn, numBytes);
    Serial.write(bytesIn, numBytes);
    
    // Set RGB values for all LEDs
    for(uint16_t i = 0; i < ledCount; i++)
    {
      colors[i].red = bytesIn[0];
      colors[i].green = bytesIn[1];
      colors[i].blue = bytesIn[2];
    }
  
    // Write colors to strip
    ledStrip.write(colors, ledCount, brightness);
  } 
}

