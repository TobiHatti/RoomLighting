#include <Time.h>
#include <Wire.h>
#include <DS1302.h>
#include <FastLED.h>

#define C_RST 2
#define C_DAT 3
#define C_CLK 4

DS1302 rtc(C_RST, C_DAT, C_CLK);

#define NUM_LEDS 150
#define DATA_PIN 4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////////////
#define STRIP_ELEMENT "DESK"
#define IS_MASTER false

#define FIRST_CHILD_DEVICE 8
#define CHILD_DEVICES 1

#define DEVICE_ID 8
////////////////////////////////////////////////////////////////////

// WD ... Weekdays
// WE ... Weekends
// WW ... Whole Week

// MON-FRI
#define WAKETIME_WD "06:30:00"
#define WAKETIME_WE "09:00:00"

#define PAUSETIME1_WD "07:20:00"
#define PAUSETIME1_WE "11:00:00"

#define STARTTIME1_WD "16:00:00"
#define STARTTIME1_WE "17:00:00"

#define ALERTTIME1_WD "06:40:00"
#define ALERTTIME2_WD "07:15:00"
#define ALERTTIME3_WE "12:00:00"
#define ALERTTIME4_WW "18:00:00"
#define ALERTTIME5_WW "20:15:00"

// SUN-THU
#define SLEEPTIME_WD "21:30:00"
#define SLEEPTIME_WE "22:30:00"

void setup() {

    Serial.begin(9600);

    if(IS_MASTER)
    {
        Wire.begin(); // join i2c bus (address optional for master) 
        
        // Set the clock to run-mode, and disable the write protection
        rtc.halt(false);
        rtc.writeProtect(false);
    
        //rtc.setDOW(FRIDAY);         // Set Day-of-Week
        //rtc.setTime(18,44,0);       // Set the time
        //rtc.setDate(18,1, 2019);    // Set the date  
    }
    else
    {
        Wire.begin(DEVICE_ID);        
        Wire.onReceive(EventTrigger); 
    }
    

    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    SetDefaultTheme();
    FastLED.setBrightness(100);
    FastLED.show(); 

}

void loop() 
{
    if(IS_MASTER)
    {
        // EVENTS FOR MONDAY - FRIDAY
        if(DOWIs("Monday") || DOWIs("Tuesday") || DOWIs("Wednesday") || DOWIs("Thursday") || DOWIs("Friday"))
        {
          if(TimeIs(WAKETIME_WD)) LightingRoutine("WakeUp");
          if(TimeIs(PAUSETIME1_WD)) LightingRoutine("PowerDown");
          if(TimeIs(STARTTIME1_WD)) LightingRoutine("StartUp");
    
          if(TimeIs(ALERTTIME1_WD)) LightingRoutine("WakeUpAlert");    
          if(TimeIs(ALERTTIME2_WD)) LightingRoutine("LeaveAlert");    
        }
      
        // EVENTS FOR SUNDAY - THURSDAY
        if(DOWIs("Sunday") || DOWIs("Monday") || DOWIs("Tuesday") || DOWIs("Wednesday") || DOWIs("Thursday"))
        {
          if(TimeIs(SLEEPTIME_WD)) LightingRoutine("Sleep");
        }
      
        // EVENTS FOR SATURDAY - SUNDAY
        if(DOWIs("Saturday") || DOWIs("Sunday"))
        {
          if(TimeIs(WAKETIME_WE)) LightingRoutine("WakeUp");
          if(TimeIs(PAUSETIME1_WE)) LightingRoutine("PowerDown");
          if(TimeIs(STARTTIME1_WE)) LightingRoutine("StartUp");
    
          if(TimeIs(ALERTTIME3_WE)) LightingRoutine("NoonAlert");            
        }
    
        // EVENTS FOR FRIDAY - SATURDAY
        if(DOWIs("Friday") || DOWIs("Saturday"))
        {
          if(TimeIs(SLEEPTIME_WE)) LightingRoutine("Sleep");
          
        }
    
        // EVENTS FOR MONDAY - SUNDAY
        if(TimeIs(ALERTTIME4_WW)) LightingRoutine("EveningAlert");    
        if(TimeIs(ALERTTIME5_WW)) LightingRoutine("PrimeTimeAlert");
    }
       delay(500);
}

//===================================================================================================================
// Date functions
//===================================================================================================================

bool TimeIs(char timeString[])
{
  if(strcmp(rtc.getTimeStr(),timeString) == 0) return true;
  else return false;  
}

bool DateIs(char dateString[])
{
  if(strcmp(rtc.getDateStr(),dateString) == 0) return true;
  else return false;   
}

bool DOWIs(char dowString[])
{
  if(strcmp(rtc.getDOWStr(FORMAT_SHORT),dowString) == 0) return true;
  else return false;     
}

void NotifyChildDevices(int notification)
{
    for(int i = FIRST_CHILD_DEVICE ; i < CHILD_DEVICES + FIRST_CHILD_DEVICE ; i++)
    {
        Wire.beginTransmission(i);
        Wire.write(notification);           
        Wire.endTransmission();   
    }
}

//===================================================================================================================
// RoutineSelector functions
//===================================================================================================================

void LightingRoutine(char routine[])
{
    if(strcmp(routine,"WakeUp") == 0) { NotifyChildDevices(1); WakeUpRoutine(10); }
    else if(strcmp(routine,"Sleep") == 0) { NotifyChildDevices(2); SleepRoutine(10); }
    else if(strcmp(routine,"StartUp") == 0) { NotifyChildDevices(3); StartUpRoutine(10); }
    else if(strcmp(routine,"PowerDown") == 0) { NotifyChildDevices(4); PowerDownRoutine(10); }
    
    else if(strcmp(routine,"WakeUpAlert") == 0) { NotifyChildDevices(5); WakeUpAlert(10); }
    else if(strcmp(routine,"LeaveAlert") == 0) { NotifyChildDevices(6); LeaveAlert(10); }
    else if(strcmp(routine,"NoonAlert") == 0) { NotifyChildDevices(7); NoonAlert(10); }
    else if(strcmp(routine,"EveningAlert") == 0) { NotifyChildDevices(8); EveningAlert(10); }
    else if(strcmp(routine,"PrimeTimeAlert") == 0) { NotifyChildDevices(9); PrimeTimeAlert(10); }
}

void EventTrigger()
{
    int eventValue = Wire.read(); 
    
    if(eventValue == 1) WakeUpRoutine(10);
    if(eventValue == 2) SleepRoutine(10);
    if(eventValue == 3) StartUpRoutine(10);
    if(eventValue == 4) PowerDownRoutine(10);
    
    if(eventValue == 5) WakeUpAlert(10);
    if(eventValue == 6) LeaveAlert(10);
    if(eventValue == 7) NoonAlert(10);
    if(eventValue == 8) EveningAlert(10);
    if(eventValue == 9) PrimeTimeAlert(10); 
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

    if(strcmp(STRIP_ELEMENT,"DESK") == 0)
    {
        for(int i = 0 ; i < NUM_LEDS ; i++) leds[i] = 0x00ffff;  
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


