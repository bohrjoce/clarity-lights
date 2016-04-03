#include <FastGPIO.h>
#define APA102_USE_FAST_GPIO
#include <APA102.h>

// Define pins
const uint8_t dataPin = 11;
const uint8_t clockPin = 12;

// Objects and parameters for controlling LED strip
APA102<dataPin, clockPin> ledStrip;
const uint16_t ledCount = 240;
rgb_color new_colors;
rgb_color written_colors[ledCount];
const uint8_t brightness = 15;

// Serial communication buffer
const int numBytes = 3;
byte bytesIn[numBytes];

void setup()
{
    Serial.begin(9600);
    for(uint16_t i = 0; i < ledCount; i++){
	written_colors[i].red = 1;
	written_colors[i].green = 1;
	written_colors[i].blue = 1;
    }
    ledStrip.write(written_colors, ledCount, brightness);
}

void loop()
{
  if(Serial.available() > 0)
  {
    Serial.readBytes((char*)bytesIn, numBytes);
    Serial.write(bytesIn, numBytes);
    
    // Set RGB values for new color
    new_colors.red = bytesIn[0];
    new_colors.green = bytesIn[1];
    new_colors.blue = bytesIn[2];
  }
  
  // Transition colors
  if(new_colors.red != written_colors[0].red
        || new_colors.green != written_colors[0].green
        || new_colors.blue != written_colors[0].blue) {

    for(uint16_t i = 0; i < ledCount; i++) {
      if(written_colors[i].red > new_colors.red)
        written_colors[i].red = written_colors[i].red - 1;
      else if(written_colors[i].red < new_colors.red)
        written_colors[i].red = written_colors[i].red + 1;
      if(written_colors[i].green > new_colors.green)
        written_colors[i].green = written_colors[i].green - 1;
      else if(written_colors[i].green < new_colors.green)
        written_colors[i].green = written_colors[i].green + 1;  
      if(written_colors[i].blue > new_colors.blue)
        written_colors[i].blue = written_colors[i].blue - 1;
      else if(written_colors[i].blue < new_colors.blue)
        written_colors[i].blue = written_colors[i].blue + 1;  
    }
    
    ledStrip.write(written_colors, ledCount, brightness);
    delay(100);
  }
}

