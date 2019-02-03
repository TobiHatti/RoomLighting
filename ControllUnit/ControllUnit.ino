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

#define FIRST_CHILD_DEVICE 4
#define CHILD_DEVICES 6

#define PRESS_COOLDOWN 300

String menuOptions[] = {
    "< Power On/Off >" ,
    "<  Dim Lights  >" ,
    "<   Set Time   >",
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

int currentDay = 0;

int currentHour = 0;
int currentMinute = 0;

int numberOfMenuOptions = 4;
int curOption = 0;
int selectedMenuOption = -1;

int brightness = 100;

int curMenuMessageIndex = 0;
int numberOfMenuMessages = 4;
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

  Wire.begin(2);
  
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
                     
                      NotifyMaster(150);
                      delay(50);
                  }

                  break;
              }  

              case 1:
              {
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

                    NotifyMaster(200 + (brightness / 10));
                    delay(50);
                  }
              
                  if(digitalRead(RIGHT) == HIGH) 
                  {
                    delay(PRESS_COOLDOWN);
                    brightness += 10;
                    if(brightness > 100) brightness = 100;
                    

                    NotifyMaster(200 + (brightness / 10));
                    delay(50);
                  }

                  if(digitalRead(SUBMIT) == HIGH)
                  {
                     selectedMenuOption = -1; 
                     delay(PRESS_COOLDOWN);
                  }

                  break;
              }

              case 2:
              {
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

                             NotifyMaster(tmpSecond);
                             delay(50);
                             NotifyMaster(tmpMinute + 60);
                             delay(50);
                             NotifyMaster(tmpHour + 120);
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

              case 3:
              {
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
                     NotifyMaster(currentDay + 151);
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

void NotifyMaster(int notification)
{
    Wire.beginTransmission(1);
    Wire.write(notification);           
    Wire.endTransmission();   

    for(int i = FIRST_CHILD_DEVICE ; i < CHILD_DEVICES + FIRST_CHILD_DEVICE ; i++)
    {
        Wire.beginTransmission(i);
        Wire.write(notification);
        Wire.endTransmission();
    }
}

