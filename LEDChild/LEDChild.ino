#include <Wire.h>
#include <FastLED.h>
#include <EEPROM.h>

#define NUM_LEDS 150
#define DATA_PIN 2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////////////

// DESK | MONITOR | PCWALL | BOTTLES | BED | BACKWALL

#define STRIP_ELEMENT "BED"
#define DEVICE_ID 5

////////////////////////////////////////////////////////////////////

int colorRed = 0;
int colorGreen = 0;
int colorBlue = 0;

void setup() {

    Serial.begin(9600);

    Wire.begin(DEVICE_ID);        
    Wire.onReceive(EventTrigger); 


    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    SetDefaultTheme();
    FastLED.setBrightness(100);
    FastLED.show(); 

}

void loop() 
{    
    delay(300);
}

//===================================================================================================================
// RoutineSelector functions
//===================================================================================================================

void EventTrigger()
{
    int eventValue; 

    static int brightness = 100;
    static bool powerStateOn = true;

    while (Wire.available()) 
    { 
        eventValue = Wire.read();  
    
    
        if(eventValue == 1 && powerStateOn) WakeUpRoutine(10);
        else if(eventValue == 2 && powerStateOn) SleepRoutine(10);
        else if(eventValue == 3 && powerStateOn) StartUpRoutine(10);
        else if(eventValue == 4 && powerStateOn) PowerDownRoutine(10);
        
        else if(eventValue == 5 && powerStateOn) WakeUpAlert(10);
        else if(eventValue == 6 && powerStateOn) LeaveAlert(10);
        else if(eventValue == 7 && powerStateOn) NoonAlert(10);
        else if(eventValue == 8 && powerStateOn) EveningAlert(10);
        else if(eventValue == 9 && powerStateOn) PrimeTimeAlert(10); 
    
        else if(eventValue == 150)
        {
            if(powerStateOn) FastLED.setBrightness(0);
            else FastLED.setBrightness(brightness);
    
            FastLED.show(); 
            powerStateOn = !powerStateOn;    
        }
    
        else if(eventValue >=200 && eventValue <=210)
        {
            brightness = (eventValue - 200) * 10; 
            FastLED.setBrightness(brightness);
            FastLED.show();     
        }
    
        if(eventValue == 180)
        {
            colorRed = Wire.read();   
            colorGreen = Wire.read();
            colorBlue = Wire.read(); 

            EEPROM.write(0,colorRed);
            EEPROM.write(1,colorGreen);
            EEPROM.write(2,colorBlue);

            for(int i = 0 ; i < NUM_LEDS ; i++)
            {
                leds[i].r = colorRed; 
                leds[i].g = colorGreen; 
                leds[i].b = colorBlue;  
            }
            FastLED.show();
        }

        if(eventValue == 181)
        {
            SetDefaultTheme();
            FastLED.show();  
        }
    }
}

//===================================================================================================================
// Default Themes
//===================================================================================================================

void SetDefaultTheme()
{
//================================
// Different for every Strip
//================================

    if(strcmp(STRIP_ELEMENT,"SHELF") == 0)
    {
        int shelf = 0;
        
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
    }
    else
    {
        int red = EEPROM.read(0);
        int grn = EEPROM.read(1);
        int blu = EEPROM.read(2);
      
        for(int i = 0 ; i < NUM_LEDS ; i++)
        {
            leds[i].r = red; 
            leds[i].g = grn; 
            leds[i].b = blu;   
        } 
    }
}

//===================================================================================================================
// Routines
//===================================================================================================================

void WakeUpRoutine(int pbSpeed)
{
//================================
// Same for every Strip
//================================
    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0xff9d00;   
    FastLED.setBrightness(0);
    
    for(int i = 0; i < 10 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(10 * pbSpeed);  
    } 
    delay(60000);

    for(int i = 10; i < 20 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(7 * pbSpeed);  
    } 
    delay(60000);

    for(int i = 20; i < 50 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(5 * pbSpeed);  
    } 
}

void SleepRoutine(int pbSpeed)
{
//================================
// Different for every Strip
// Beginning same
//================================
    for(int i = 100; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 2);  
    } 

    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0xae00ff;   
    
    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 2);  
    }
    
// Start of Individual Parts  

    if(strcmp(STRIP_ELEMENT,"SHELF") == 0)
    {
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

    if(strcmp(STRIP_ELEMENT,"DESK") == 0)
    {
        delay(1000);
         
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

    if(strcmp(STRIP_ELEMENT,"MONITOR") == 0)
    {
        delay(2000);
         
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

    if(strcmp(STRIP_ELEMENT,"PCWALL") == 0)
    {
        delay(3000);
         
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

    if(strcmp(STRIP_ELEMENT,"BOTTLES") == 0)
    {
        delay(5000);
         
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

    if(strcmp(STRIP_ELEMENT,"BED") == 0)
    {
        delay(4000);
         
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

    if(strcmp(STRIP_ELEMENT,"BACKWALL") == 0)
    {
        delay(6000);
         
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            for(int j = 0; j < 8; j++)
            {
                leds[i] /= 2;
                FastLED.show();    
                delay(pbSpeed/3);
            }   
        }  
    }

}

void StartUpRoutine(int pbSpeed)
{
//================================
// Same for every Strip
//================================

    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 2);  
    }     
}

void PowerDownRoutine(int pbSpeed)
{
//================================
// Same for every Strip
//================================

    for(int i = 100; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 2);  
    }  
}

void WakeUpAlert(int pbSpeed)
{
//================================
// Same for every Strip
//================================

    for(int i = 50; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed);  
    } 

    for(int j = 0 ; j < 5 ; j++)
    {
        for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0xffff00;   
    
        for(int i = 0; i < 100 ; i++)
        {
            FastLED.setBrightness(i);
            FastLED.show(); 
            delay(pbSpeed * 2);  
        }
        delay(600);
        for(int i = 100; i > 0 ; i--)
        {
            FastLED.setBrightness(i);
            FastLED.show(); 
            delay(pbSpeed);  
        } 
    
        for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0x00ffff;
    
        for(int i = 0; i < 100 ; i++)
        {
            FastLED.setBrightness(i);
            FastLED.show(); 
            delay(pbSpeed / 3);  
        }
        delay(10);
        for(int i = 100; i > 0 ; i--)
        {
            FastLED.setBrightness(i);
            FastLED.show(); 
            delay(pbSpeed / 2);  
        } 
    }

    delay(1500);
    
    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0xffff00;  

    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 2);  
    }
    
}

void LeaveAlert(int pbSpeed)
{
//================================
// Same for every Strip
//================================

    for(int i = 100; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed / 2);  
    }   

    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0x10ff00;  

    for(int j = 0 ; j < 2 ; j++)
    {
        for(int i = 0; i < 100 ; i++)
        {
            FastLED.setBrightness(i);
            FastLED.show(); 
            delay(pbSpeed / 2);  
        }
        delay(pbSpeed * 4);
        FastLED.setBrightness(0);
        FastLED.show(); 
        delay(pbSpeed * 4);
        FastLED.setBrightness(100);
        FastLED.show(); 
        delay(pbSpeed * 4);
        FastLED.setBrightness(0);
        FastLED.show(); 
        delay(pbSpeed * 4);
        FastLED.setBrightness(100);
        FastLED.show(); 
    
        for(int i = 100; i > 0 ; i--)
        {
            FastLED.setBrightness(i);
            FastLED.show(); 
            delay(pbSpeed / 2);  
        } 
    } 

    delay(1500);

    SetDefaultTheme();
    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 3);  
    }
}

void NoonAlert(int pbSpeed)
{
//================================
// Same for every Strip
//================================

    for(int i = 100; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed / 2);  
    }   

    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0x000000;
    FastLED.setBrightness(100);  

    for(int k = 0 ; k < 2 ; k++)
    {
        for(int i = 0; i < NUM_LEDS ; i++)
        {
            leds[i] = 0x00ffff;
            FastLED.show();    
            delay(pbSpeed/4);  
        }  
        
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            leds[i] = 0x000000;
            FastLED.show();    
            delay(pbSpeed/4);
        }    
    }
    delay(1500);

    SetDefaultTheme();
    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 3);  
    }
}

void EveningAlert(int pbSpeed)
{
//================================
// Same for every Strip
//================================

    for(int i = 100; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed / 2);  
    }   

    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0x000000;
    FastLED.setBrightness(100);  

    for(int k = 0 ; k < 2 ; k++)
    {
        for(int i = 0; i < NUM_LEDS ; i++)
        {
            leds[i] = 0x48ff00;
            FastLED.show();    
            delay(pbSpeed/4);  
        }  
        
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            leds[i] = 0x000000;
            FastLED.show();    
            delay(pbSpeed/4);
        }    
    }
    delay(1500);

    SetDefaultTheme();
    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 3);  
    }
  
}

void PrimeTimeAlert(int pbSpeed)
{
//================================
// Same for every Strip
//================================
    
    
    for(int i = 100; i > 0 ; i--)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed / 2);  
    }   

    for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0x000000;
    FastLED.setBrightness(100);  

    for(int k = 0 ; k < 2 ; k++)
    {
        for(int i = 0; i < NUM_LEDS ; i++)
        {
            leds[i] = 0x4ffc700;
            FastLED.show();    
            delay(pbSpeed/4);  
        }  
        
        for(int i = NUM_LEDS; i >= 0 ; i--)
        {
            leds[i] = 0x000000;
            FastLED.show();    
            delay(pbSpeed/4);
        }    
    }
    delay(1500);

    SetDefaultTheme();
    for(int i = 0; i < 100 ; i++)
    {
        FastLED.setBrightness(i);
        FastLED.show(); 
        delay(pbSpeed * 3);  
    }
  
}
