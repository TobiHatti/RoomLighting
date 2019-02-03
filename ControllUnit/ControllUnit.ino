/* www.learningbuz.com */
/*Impport following Libraries*/
#include <LCD.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

#define LEFT 11
#define RIGHT 12
#define SUBMIT 13

#define DEVICE_ID 9

#define FIRST_CHILD_DEVICE 4
#define CHILD_DEVICES 6

#define PRESS_COOLDOWN 300

/*
String menuOptions[] = {
    "<Identify Strip>" ,
    "< Select Strip >" , 
    "< Custom Color >" ,
    "< Preset Color >" ,
    "< Save Preset  >" ,
    "< Load Preset  >" ,
    "<Effect Presets>" ,
    "< Strip Length >" ,
    "< Power On/Off >" ,
    "<  Dim Lights  >" ,
    "<   Set Time   >" ,
    "<   Set DOW    >"
};
*/

String menuOptions[] = {
    "<Identify Strip>" ,
    "< Select Strip >" , 
    "< Custom Color >" ,
    "< Default Color>" ,
    "< Power On/Off >" ,
    "<  Dim Lights  >" ,
    "<   Set Time   >" ,
    "<   Set DOW    >"
};

String dayOptions[] = {
    "<    Monday    >" ,
    "<    Tuesday   >" ,
    "<   Wednesday  >",
    "<   Thursday   >",
    "<    Friday    >",
    "<   Saturday   >",
    "<    Sunday    >"
};

String stripOptions[] = {
    "< A: All       >" ,
    "< 1: Shelf     >" ,
    "< 2: Desk      >" ,
    "< 3: Monitors  >" ,
    "< 4: PC-Wall   >" ,
    "< 5: Bed       >" ,
    "< 6: Back-Wall >" ,
    "< 7: Bottles   >" 
};

int identifySelected = 0;
bool identifyActive = false;

int selectedStrip = 0;

byte colorSelectRed = 0;
byte colorSelectGreen = 0;
byte colorSelectBlue = 0;
byte currentEditColor = 0;

byte currentLength = 0;
String lengthMessage;

byte currentDay = 0;
byte currentHour = 0;
byte currentMinute = 0;

byte numberOfMenuOptions = 8;
byte curOption = 0;
int selectedMenuOption = -1;

byte brightness = 100;

byte curMenuMessageIndex = 0;
byte numberOfMenuMessages = 3;
int runTimeMenuCtr = 0;
String menuMessage;

String dimMessage;
String timeMessage;

char timeString[] =  { "00:00:00" };

int timeTransferCtr = 0;

int idleTimeCtr = 0;

bool mainMenuActive = true;
bool powerStateOn = true;

int timeSubmenu = 0;
int tmpHour = -1;
int tmpMinute = -1;
int tmpSecond = -1;

uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };

void setup() 
{

  pinMode(LEFT,INPUT);
  pinMode(RIGHT,INPUT);
  pinMode(SUBMIT,INPUT);

  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  Wire.begin(DEVICE_ID);
  
  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
  lcd.backlight();//To Power ON the back light
  lcd.createChar(0, SpecialChar);

}

void loop() 
{
    if(idleTimeCtr > 1000)
    {
        idleTimeCtr = 0; 
        mainMenuActive = true; 
    }


    if(digitalRead(SUBMIT) == HIGH || digitalRead(LEFT) == HIGH || digitalRead(RIGHT) == HIGH) idleTimeCtr = 0;   
  
    if(mainMenuActive)
    {
        if(runTimeMenuCtr > 80)
        {
            Wire.requestFrom(1, 8);

            timeTransferCtr = 0;
            while (Wire.available()) { // slave may send less than requested
                timeString[timeTransferCtr] = Wire.read();
                timeTransferCtr++;
            }
            
            runTimeMenuCtr=0;
            curMenuMessageIndex++;
            if(curMenuMessageIndex > numberOfMenuMessages-1) curMenuMessageIndex = 0;
              
        }

        lcd.setCursor(0,0);

        switch(curMenuMessageIndex)
        {
            case 0: 
            {
                menuMessage = "Time: "; 
                menuMessage.concat(timeString); 
                menuMessage.concat("         "); 
                break;
            }
            case 1: 
            {
              if(powerStateOn) menuMessage = "Power: ON          ";
              else menuMessage = "Power: OFF      "; 
              break;
            }
            case 2: menuMessage = "Brightness: "; menuMessage.concat(brightness) ; menuMessage.concat("%   "); break;      
        }

        lcd.print(menuMessage);   
        
        lcd.setCursor(0,1);
        lcd.print("> Press [ENTER] "); 

        if(digitalRead(SUBMIT) == HIGH) 
        { 
            mainMenuActive = false; 
            delay(PRESS_COOLDOWN); 
        }
    }
    else
    {
      if(selectedMenuOption == -1)
      {
          lcd.setCursor(0,0);
          lcd.print("-= MAIN  MENU =-");       
  
          lcd.setCursor(0,1);
          lcd.print(menuOptions[curOption]); 
          
          if(digitalRead(RIGHT) == HIGH) 
          {
            curOption++;
            if(curOption > numberOfMenuOptions-1) curOption = 0;
            delay(PRESS_COOLDOWN);
          }
      
          if(digitalRead(LEFT) == HIGH) 
          {
            curOption--;
            if(curOption < 0) curOption = numberOfMenuOptions-1;
            delay(PRESS_COOLDOWN);
          }
  
          if(digitalRead(SUBMIT) == HIGH)
          {
             selectedMenuOption = curOption;
             delay(PRESS_COOLDOWN);  
          }  
      }
      else
      {
          switch(selectedMenuOption)
          {
              case 0: 
              {
                  // Identify Strip  

                  lcd.setCursor(0,0);
                  lcd.print("-=  Identify  =-");      

                  if(identifyActive)
                  {

                      lcd.setCursor(0,1);
                      lcd.print("Identifying...  ");    
                      
                      TransferColor(identifySelected,255,255,255);
                      delay(200);
                      TransferColor(identifySelected,0,0,0);
                      delay(200);
                      
                      if(digitalRead(SUBMIT) == HIGH)
                      {
                          NotifyDevice(identifySelected,181);
                          identifyActive = false;
                          selectedMenuOption = -1;
                          delay(PRESS_COOLDOWN);  
                      }   
                  }
                  else
                  {
                      lcd.setCursor(0,1);
                      lcd.print(stripOptions[identifySelected]);   
    
                      if(digitalRead(RIGHT) == HIGH) 
                      {
                        identifySelected++;
                        if(identifySelected > 7) identifySelected = 0;
                        delay(PRESS_COOLDOWN);
                      }
                  
                      if(digitalRead(LEFT) == HIGH) 
                      {
                        identifySelected--;
                        if(identifySelected < 0) identifySelected = 7;
                        delay(PRESS_COOLDOWN);
                      }
              
                      if(digitalRead(SUBMIT) == HIGH)
                      {
                         identifyActive = true;
                         delay(PRESS_COOLDOWN);  
                      }   
                  }
                  
                  break;
              }

              case 1:
              {
                  // Select Strip

                  lcd.setCursor(0,0);
                  lcd.print("-=  Select    =-");      


                  lcd.setCursor(0,1);
                  lcd.print(stripOptions[selectedStrip]);   

                  if(digitalRead(RIGHT) == HIGH) 
                  {
                    selectedStrip++;
                    if(selectedStrip > 7) selectedStrip = 0;
                    delay(PRESS_COOLDOWN);
                  }
              
                  if(digitalRead(LEFT) == HIGH) 
                  {
                    selectedStrip--;
                    if(selectedStrip < 0) selectedStrip = 7;
                    delay(PRESS_COOLDOWN);
                  }
          
                  if(digitalRead(SUBMIT) == HIGH)
                  {
                      selectedMenuOption = -1;

                      lcd.setCursor(0,1);
                      lcd.print("Strip Selected! ");   
                      delay(500);
                      delay(PRESS_COOLDOWN);  
                  }   
                  
                  break;
              }

              case 2:
              {
                  // Set Custom Color

                  lcd.setCursor(0,1); 
                  lcd.print("                "); 
                  
                  switch(currentEditColor)
                  {
                      case 0:
                      { 
                          lcd.setCursor(0,0); 
                          lcd.print(" R              "); 
                          lcd.setCursor(0,1);
                          lcd.print(colorSelectRed);
    
                          if(digitalRead(RIGHT) == HIGH) 
                          {
                            colorSelectRed++;
                            if(colorSelectRed > 254) colorSelectRed = 0;
                            TransferColor(selectedStrip,colorSelectRed,colorSelectGreen,colorSelectBlue);
                            delay(10);
                          }
                      
                          if(digitalRead(LEFT) == HIGH) 
                          {
                            colorSelectRed--;
                            if(colorSelectRed <= 0) colorSelectRed = 254;
                            TransferColor(selectedStrip,colorSelectRed,colorSelectGreen,colorSelectBlue);
                            delay(10);
                          }
                  
                          if(digitalRead(SUBMIT) == HIGH)
                          {
                              currentEditColor++;
                              delay(PRESS_COOLDOWN);  
                          } 
                          
                          break;
                      }

                      case 1:
                      { 
                          lcd.setCursor(0,0); 
                          lcd.print("       G        "); 
                          lcd.setCursor(6,1);
                          lcd.print(colorSelectGreen);
    
                          if(digitalRead(RIGHT) == HIGH) 
                          {
                            colorSelectGreen++;
                            if(colorSelectGreen > 254) colorSelectGreen = 0;
                            TransferColor(selectedStrip,colorSelectRed,colorSelectGreen,colorSelectBlue);
                            delay(10);
                          }
                      
                          if(digitalRead(LEFT) == HIGH) 
                          {
                            colorSelectGreen--;
                            if(colorSelectGreen <= 0) colorSelectGreen = 254;
                            TransferColor(selectedStrip,colorSelectRed,colorSelectGreen,colorSelectBlue);
                            delay(10);
                          }
                  
                          if(digitalRead(SUBMIT) == HIGH)
                          {
                              currentEditColor++;
                              delay(PRESS_COOLDOWN);  
                          } 
                          
                          break;
                      }

                      case 2:
                      { 
                          lcd.setCursor(0,0); 
                          lcd.print("             B  "); 
                          lcd.setCursor(12,1);
                          lcd.print(colorSelectBlue);
    
                          if(digitalRead(RIGHT) == HIGH) 
                          {
                            colorSelectBlue++;
                            if(colorSelectBlue > 254) colorSelectBlue = 0;
                            TransferColor(selectedStrip,colorSelectRed,colorSelectGreen,colorSelectBlue);
                            delay(10);
                          }
                      
                          if(digitalRead(LEFT) == HIGH) 
                          {
                            colorSelectBlue--;
                            if(colorSelectBlue <= 0) colorSelectBlue = 254;
                            TransferColor(selectedStrip,colorSelectRed,colorSelectGreen,colorSelectBlue);
                            delay(10);
                          }
                  
                          if(digitalRead(SUBMIT) == HIGH)
                          {
                              currentEditColor = 0;
                              selectedMenuOption = -1;
                              delay(PRESS_COOLDOWN);  
                          } 
                          
                          break;
                      }
                  }

                  break;
              }

              case 3:
              {
                  // Set Default colorscheme
                  
                  lcd.setCursor(0,0);
                  lcd.print("[= Set Default=]");       

                  lcd.setCursor(0,1);
                  lcd.print("[ENTER] Default ");  

                  if(digitalRead(SUBMIT) == HIGH)
                  {
                      selectedMenuOption = -1;
                      NotifyDevice(selectedStrip, 181);
                      delay(PRESS_COOLDOWN);
                  }

                  break;      
              }
              
              case 4:
              {
                  // Turn lights on/off
                  
                  lcd.setCursor(0,0);
                  lcd.print("[=Power On/Off=]");       

                  lcd.setCursor(0,1);
                  if(powerStateOn) lcd.print("[ENTER] Turn Off");  
                  else lcd.print("[ENTER] Turn On ");  

                  if(digitalRead(SUBMIT) == HIGH)
                  {
                      selectedMenuOption = -1;
                      delay(PRESS_COOLDOWN); 
                      powerStateOn = !powerStateOn;
                     
                      NotifyDevice(selectedStrip, 150);
                      delay(50);
                  }

                  break;
              }  

              case 5:
              {
                  // Dim Lights
                  
                  lcd.setCursor(0,0);
                  lcd.print("[= Dim Lights =]");     

                  lcd.setCursor(0,1);
                  dimMessage = "< [-] ";
                  if(brightness != 100) dimMessage.concat(" ");
                  if(brightness == 0) dimMessage.concat(" ");
                  dimMessage.concat(brightness);
                  dimMessage.concat("%");
                  dimMessage.concat(" [+] >"); 
                    
                  lcd.print(dimMessage);  

                  if(digitalRead(LEFT) == HIGH) 
                  {
                    delay(PRESS_COOLDOWN);
                    brightness -= 10;
                    if(brightness < 0) brightness = 0;

                    NotifyDevice(selectedStrip, 200 + (brightness / 10));
                    delay(50);
                  }
              
                  if(digitalRead(RIGHT) == HIGH) 
                  {
                    delay(PRESS_COOLDOWN);
                    brightness += 10;
                    if(brightness > 100) brightness = 100;
                    
                    NotifyDevice(selectedStrip, 200 + (brightness / 10));
                    delay(50);
                  }

                  if(digitalRead(SUBMIT) == HIGH)
                  {
                     selectedMenuOption = -1; 
                     delay(PRESS_COOLDOWN);
                  }

                  break;
              }

              case 6:
              {
                  // Set Time

                  if(tmpHour == -1) tmpHour = currentHour; 
                  if(tmpMinute == -1) tmpMinute = currentMinute;   
                  if(tmpSecond == -1) tmpSecond = 0;   

                  switch(timeSubmenu)
                  {
                      case 0:
                      {
                          lcd.setCursor(0,0);
                          timeMessage = "    ";
                          timeMessage.concat(char(0));
                          timeMessage.concat("/          ");
                          lcd.print(timeMessage);  

                          if(digitalRead(SUBMIT) == HIGH)
                          {
                             timeSubmenu++; 
                             delay(PRESS_COOLDOWN);
                          }   

                          if(digitalRead(LEFT) == HIGH) 
                          {
                            delay(PRESS_COOLDOWN);
                            tmpHour --;
                            if(tmpHour < 0) tmpHour = 23;
                          }
                      
                          if(digitalRead(RIGHT) == HIGH) 
                          {
                            delay(PRESS_COOLDOWN);
                            tmpHour ++;
                            if(tmpHour > 23) tmpHour = 0;
                          }
                        
                          break; 
                      }  

                      case 1:
                      {
                          lcd.setCursor(0,0);
                          timeMessage = "       ";
                          timeMessage.concat(char(0));
                          timeMessage.concat("/       ");
                          lcd.print(timeMessage);  

                          if(digitalRead(SUBMIT) == HIGH)
                          {
                             timeSubmenu++; 
                             delay(PRESS_COOLDOWN);
                          } 

                          if(digitalRead(LEFT) == HIGH) 
                          {
                            delay(PRESS_COOLDOWN);
                            tmpMinute --;
                            if(tmpMinute < 0) tmpMinute = 59;
                          }
                      
                          if(digitalRead(RIGHT) == HIGH) 
                          {
                            delay(PRESS_COOLDOWN);
                            tmpMinute ++;
                            if(tmpMinute > 59) tmpMinute = 0;
                          }
                          
                          break; 
                      }

                      case 2:
                      {
                          lcd.setCursor(0,0);
                          timeMessage = "          ";
                          timeMessage.concat(char(0));
                          timeMessage.concat("/    ");
                          lcd.print(timeMessage); 

                          if(digitalRead(SUBMIT) == HIGH)
                          {
                             timeSubmenu = 0; 
                             selectedMenuOption = -1; 
                             delay(PRESS_COOLDOWN);

                             // 000...059 Seconds
                             // 060...119 Minutes
                             // 120...143 Hours

                             NotifyDevice(1,tmpSecond); 
                             delay(50);
                             NotifyDevice(1,tmpMinute + 60);
                             delay(50);
                             NotifyDevice(1,tmpHour + 120);
                             delay(50);
                          }  

                          if(digitalRead(LEFT) == HIGH) 
                          {
                            delay(PRESS_COOLDOWN);
                            tmpSecond --;
                            if(tmpSecond < 0) tmpSecond = 59;
                          }
                      
                          if(digitalRead(RIGHT) == HIGH) 
                          {
                            delay(PRESS_COOLDOWN);
                            tmpSecond ++;
                            if(tmpSecond > 59) tmpSecond = 0;
                          }
                            
                          break;  
                      }
                  }

                  lcd.setCursor(0,1);
                  timeMessage = "<-  ";
                  if(tmpHour < 10) timeMessage.concat("0");
                  timeMessage.concat(tmpHour);
                  timeMessage.concat(":");
                  if(tmpMinute < 10) timeMessage.concat("0");
                  timeMessage.concat(tmpMinute);
                  timeMessage.concat(":");
                  if(tmpSecond < 10) timeMessage.concat("0");
                  timeMessage.concat(tmpSecond);
                  timeMessage.concat("  +>");
                  lcd.print(timeMessage);   
                  break;
              }

              case 7:
              {
                  // Set Day of Week
                  
                  lcd.setCursor(0,0);
                  lcd.print("[=  Set  DOW  =]");   


                  lcd.setCursor(0,1);
                  lcd.print(dayOptions[currentDay]);   


                  if(digitalRead(LEFT) == HIGH) 
                  {
                    delay(PRESS_COOLDOWN);
                    currentDay --;
                    if(currentDay < 0) currentDay = 6;
                  }
              
                  if(digitalRead(RIGHT) == HIGH) 
                  {
                    delay(PRESS_COOLDOWN);
                    currentDay ++;
                    if(currentDay > 6) currentDay = 0;
                  }

                  if(digitalRead(SUBMIT) == HIGH)
                  {
                     selectedMenuOption = -1; 
                     NotifyDevice(1,currentDay + 151);
                     delay(PRESS_COOLDOWN);
                  } 
                  
                  break;  
              }
          }  
      }
        
    }
    
    if(idleTimeCtr > 999999) idleTimeCtr = 0;
    if(runTimeMenuCtr > 999999) runTimeMenuCtr = 0;
     
    idleTimeCtr++;
    runTimeMenuCtr++;
    delay(10);
}

void NotifyDevice(int deviceID, int notification)
{
    if(deviceID == 0)
    {
        for(int i = 1 ; i <= 7 ; i++)
        {
            Wire.beginTransmission(i);
            Wire.write(notification);
            Wire.endTransmission();
        } 
    }
    else
    {
        Wire.beginTransmission(deviceID);
        Wire.write(notification);           
        Wire.endTransmission();    
    }  
}

void TransferColor(byte deviceID, byte red, byte green ,byte blue)
{
    if(deviceID == 0)
    {
        for(int i = 1 ; i <= 7 ; i++)
        {
            Wire.beginTransmission(i);
            Wire.write(180); 
            Wire.write(red); 
            Wire.write(green);    
            Wire.write(blue);              
            Wire.endTransmission();  

            delay(50);
        } 
    }
    else
    {
        Wire.beginTransmission(deviceID);
        Wire.write(180); 
        Wire.write(red); 
        Wire.write(green);    
        Wire.write(blue);              
        Wire.endTransmission();    
    } 
}

void TransferLength(byte deviceID, byte stripLength)
{
    Wire.beginTransmission(deviceID);
    Wire.write(182);
    Wire.write(stripLength); 
    Wire.endTransmission();   
}

