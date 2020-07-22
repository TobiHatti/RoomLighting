#include <FastLED.h>

#define NUM_LEDS 103
#define DATA_PIN 3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

int shelf = 0;

CRGB leds[NUM_LEDS];

void setup() { 
   delay(1000); 
   FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    for(int i = 0 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0x00bfff;
    shelf++;
    for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0xff7f00;
    shelf++;
    
    for(int i = 1 + (shelf * 17); i<=9 + (shelf * 17) ; i++) leds[i] = 0xff0000;
    for(int i = 9 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0x00bfff;

    
    shelf++;
    for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0xff0000;
    shelf++;
    for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0x00edff;
    shelf++;
    for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0xf2ff00;
    
    FastLED.show(); 
}

void loop() { 

    
  
    
}


