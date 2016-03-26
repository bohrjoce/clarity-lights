#include <FastGPIO.h>
#define APA102_USE_FAST_GPIO
#include <APA102.h>

// Define pins
const uint8_t led = 13;
const uint8_t dataPin = 11;
const uint8_t clockPin = 12;

// Objects and parameters for controlling LED strip
APA102<dataPin, clockPin> ledStrip;
const uint16_t ledCount = 240;
rgb_color colors[ledCount];
const uint8_t brightness = 1;

// Serial communication bytes
byte byteIn, emot;

// Define colors for each emotion
const uint16_t emotCount = 7;
rgb_color emotColors[emotCount] = {
  (rgb_color){242, 0  , 0  }, // Anger
  (rgb_color){100, 100, 100}, // Neutral
  (rgb_color){0  , 224, 0  }, // Disgust
  (rgb_color){216, 0  , 245}, // Fear
  (rgb_color){255, 255, 0  }, // Happy
  (rgb_color){0  , 191, 255}, // Sadness
  (rgb_color){255, 185, 20 }  // Surprise
};


// Blinks an Arduino LED once
void blink() {
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
}

void setup()
{
    Serial.begin(9600);
    pinMode(led, OUTPUT); 
}

void loop()
{
  if(Serial.available() > 0)
  {
    byteIn = Serial.read();
    Serial.write(byteIn);
    emot = byteIn - 48; // ASCII conversion
    
    // Set RGB values for all LEDs
    for(uint16_t i = 0; i < ledCount; i++)
    {
      colors[i] = emotColors[emot-1];
    }
  
    // Write colors to strip
    ledStrip.write(colors, ledCount, brightness);
    
    // Individual actions for each emotion
    switch(emot) {
      case 1: // Anger
        break;
        
      case 2: // Neutral
        break;
        
      case 3: // Disgust
        break;
        
      case 4: // Fear
        break;
        
      case 5: // Happy
        break;
        
      case 6: // Sadness
        break;
        
      case 7: // Surprise
        break;
        
      default: // No data read or invalid byte sent
        break;
    }
  } 
}

