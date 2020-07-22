#include <FastLED.h>

bool isActive = false;

#define NUM_LEDS 210
#define DATA_PIN 2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(8,INPUT);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  
  
  FastLED.setBrightness(100);
  FastLED.show(); 
}

void loop() {
  // put your main code here, to run repeatedly:

  if(isActive && digitalRead(8) == LOW) 
  {
      isActive = false;

      // Turn Light off
      
      Serial.println("Light off");  
      delay(500);
  }

  if(!isActive && digitalRead(8) == HIGH) 
  {
      isActive = true;
      Serial.println("Light on");  
      delay(500);
  }

  for(int i = 0 ; i < NUM_LEDS ; i++) 
  {
      leds[i-1] = 0x00ff00;   
      leds[i] = 0x00ff00;     
  }
  
}
