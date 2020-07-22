#include <FastLED.h>

#define NUM_LEDS 300
#define DATA_PIN 3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB


CRGB leds[NUM_LEDS];

void setup() { 
   delay(1000); 
   FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    for(int i = 0; i<=NUM_LEDS ; i++) leds[i] = 0xffffff;
    
    FastLED.show(); 
}

void loop() { 

    
  
    
}
