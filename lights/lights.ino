int led = 13;
byte byte_in, emot;

void setup()
{
    Serial.begin(9600);
    pinMode(led, OUTPUT); 
}

void blink() {
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
}

void loop()
{
  if(Serial.available() > 0)
  {
    byte_in = Serial.read();
    Serial.write(byte_in);
    emot = byte_in - 48; // ASCII conversion
    
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

