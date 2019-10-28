#include <Arduino.h>
#include <U8g2lib.h>
#include "Wire.h"
#include "uRTCLib.h"


uRTCLib rtc(0x68);
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 23, /* data=*/ 19, /* reset=*/ U8X8_PIN_NONE);

#define nextButton 16
#define backButton 18

int page=1;
int selMainMenueItm=1; int selSetTimeItm=1;int selSetAlarmItm=1; int selSetAlarmXItm=1;
bool nxtBtnPressStat=0, bakBtnPressStat=0;
bool reading, prevReading=LOW , reading2, prevReading2=LOW ;
int hr=0,minit=0,seco=0;
int format=12;
int X=1;
int alarmState=LOW,interval=200,counter=1;
unsigned long previousMillis = 0,currentMillis=0,starttime=0,endtime=0;  
 
struct alarmVar
{
  bool status_=0;
  int hr=0;
  int minit=0;
  int seco=0;
} AVar[3];

void setup() {
   Serial.begin(9600);
   pinMode(16, INPUT_PULLUP); 
   pinMode(18, INPUT_PULLUP); 
    pinMode(15, OUTPUT);                                    
   u8g2.begin(); 
   Wire.begin();

    
   
    //rtc.set(0, 42, 16, 6, 2, 5, 15);
    //AVar[1].status_=1; AVar[1].hr=16; AVar[1].minit=42; AVar[1].seco=5;
 }

void loop() {

   
     selector();
     ringAlarm();
     
    u8g2.firstPage();
  do { 
      switch(page)
      { 
       case 1:clockFace(); break;
       case 2:  mainMenue(); break;
       case 21: setTime(); break;
       case 22: setAlarm(); break;
       case 221: setAlarmX(); break;
       case 222: setAlarmX(); break;
       case 223: setAlarmX(); break;
      }
       
   } while ( u8g2.nextPage() );
}

void mainMenue()
{
  
  switch(selMainMenueItm)
 {
  case 1:drawMainMenue(0,1,1,1,2); break;
  case 2:drawMainMenue(1,0,1,1,17); break;
  case 3:drawMainMenue(1,1,0,1,32); break;
  case 4:drawMainMenue(1,1,1,0,47); break;
 }

}
int drawMainMenue(int b1,int b2,int b3,int b4,int boxYCoordinate)
{
 
 
    u8g2.setFont(u8g2_font_9x15_mf);
    u8g2.setFontMode(1);
    u8g2.setDrawColor(1);
    u8g2.drawBox(0,boxYCoordinate,128,13);
  
    u8g2.setDrawColor(b1);
    u8g2.drawStr(0,13,"BACK");
   
    u8g2.setDrawColor(b2);
    u8g2.drawStr(0,28,"SET TIME");
  
    u8g2.setDrawColor(b3);
    u8g2.drawStr(0,43,"SET ALARM");
    
    u8g2.setDrawColor(b4);
    u8g2.drawStr(0,58,"SET FACE");
    

   }

   void clockFace()
   {
      rtc.refresh();
      hr=rtc.hour();
      minit=rtc.minute();
      seco=rtc.second();

     
//*****************************12/24 HR conversions**************************************************************      
      u8g2.setFont(u8g2_font_freedoomr10_mu);
       int h = u8g2.getFontAscent()-u8g2.getFontDescent();
           h=h/2;
       int tHR;    
      if(format==12)
      {
        if(hr>=0&&hr<=11)
        { u8g2.setCursor(115-(u8g2.getStrWidth("AM")/2),17+h); u8g2.print("AM");
           tHR=hr; }
        else if (hr>=12&&hr<=23) 
        {u8g2.setCursor(115-(u8g2.getStrWidth("PM")/2),17+h); u8g2.print("PM");
          if(hr==12)
          tHR=hr;
          else 
          tHR=hr-12;
          }
          
      }
//*****************************tempereture**********************************************************************

     // u8g2.setCursor(115-(u8g2.getStrWidth("00")/2),35+h); u8g2.print(rtc.temp());
     //u8g2.drawFrame(104,26,u8g2.getStrWidth("00")+4,(h*2)+4);
      
//*****************************converting read time from rtc to a string format HH:MM*********************************
      char TIME[6];

      Serial.println(tHR);
      if(tHR>=10)
      {
        int tHR1,tHR2;
        tHR1=tHR/10; tHR2=tHR%10;
        TIME[0]=(char)tHR1+48;  TIME[1]=(char)tHR2+48;
       }
      else
      {
        TIME[0]='0';  TIME[1]=tHR+48;
      }
      
      if(minit>=10)
      { 
        int minit1,minit2;
        minit1=minit/10; minit2=minit%10;
        TIME[3]=(char)minit1+48;  TIME[4]=(char)minit2+48;
       }
      else
      {
        TIME[3]='0';  TIME[4]=minit+48;
      }
      TIME[2]=':';TIME[5]='\0';
      
      
      u8g2.setFont(u8g2_font_freedoomr25_mn);
        h = u8g2.getFontAscent()-u8g2.getFontDescent();
           h=h/2;
       
       u8g2.setCursor(19-(u8g2.getStrWidth("00")/2),24+h); u8g2.print(TIME);

//******************************drawing the seconds box**********************************************************
        u8g2.setFont(u8g2_font_freedoomr10_mu);
       
        u8g2.drawFrame(1,45,101,11);
        u8g2.drawBox(1,46,(seco++*1.7),9);
        
        u8g2.setCursor(107,57); u8g2.print(seco);
   }


   void setTime()
   {
      int width=128,height=64;
       
      
       u8g2.setFont(u8g2_font_9x15_mf);
       int h = u8g2.getFontAscent()-u8g2.getFontDescent();
           h=h/2;

       u8g2.setDrawColor(1);
       switch(selSetTimeItm)
      {
        case 1: u8g2.drawDisc(32,36,10); break;
        case 2: u8g2.drawDisc(64,36,10); break;
        case 3: u8g2.drawDisc(96,36,10); break;
        
        case 4: u8g2.drawBox(0,48,64,16); break;
        case 5: u8g2.drawBox(64,48,64,16); break;
      }
 
      u8g2.setDrawColor(2);

       u8g2.setCursor(64-(u8g2.getStrWidth("SET TIME")/2),8+h); u8g2.print("SET TIME");
       u8g2.drawLine(0,16,128,16);
      
      u8g2.setCursor(32-(u8g2.getStrWidth("HH")/2),36+h); u8g2.print(hr);
      u8g2.setCursor(64-(u8g2.getStrWidth("MM")/2),36+h); u8g2.print(minit);
      u8g2.setCursor(96-(u8g2.getStrWidth("SS")/2),36+h); u8g2.print(seco);

    
     
      u8g2.drawStr(32-(u8g2.getStrWidth("OK")/2),56+h,"OK");
      u8g2.drawStr(96-(u8g2.getStrWidth("CANCEL")/2),56+h,"CANCEL");

      //int i;
      //for(i=0;i<=129;i+=16)
      //{u8g2.drawLine(i,0,i,64);
      //u8g2.drawLine(0,i,128,i);}

     
        u8g2.setDrawColor(1);
  
   }
   void setAlarm()
   {
     
     u8g2.setFont(u8g2_font_9x15_m_symbols);
     int h = u8g2.getFontAscent()-u8g2.getFontDescent();
     h=h/2;

     u8g2.setDrawColor(1);
     switch(selSetAlarmItm)
     {
      case 1:u8g2.drawBox(0,0,128,16); break;
      case 2:u8g2.drawBox(0,16,128,16); break;
      case 3:u8g2.drawBox(0,32,128,16); break;
      case 4:u8g2.drawBox(0,48,128,16); break;
     }

     
     
     u8g2.setDrawColor(2);
     u8g2.setCursor(0,8+h); u8g2.print("Alarm 1");      
     u8g2.setCursor(0,24+h); u8g2.print("Alarm 2");   
      u8g2.setCursor(0,40+h); u8g2.print("Alarm 3");    
     u8g2.setCursor(0,56+h); u8g2.print("BACK");
///////////////////////Drawing ✔ or ❌  to see status of alarm/////////////////////////////
    
    //u8g2.setFont(u8g2_font_m2icon_9_tf);
    
     for(int i=0,j=8 ;i<3&&j<=40 ;i++,j+=16)
     {
        
        if(AVar[i].status_==1) 
        u8g2.drawGlyph(96,j+h-2, 0x2714);
        else
        u8g2.drawGlyph(96,j+h-2, 0x2715);

     }

     u8g2.setDrawColor(1);
   }

   void setAlarmX()
   {
     
       
      
       u8g2.setFont(u8g2_font_9x15_mf);
       int h = u8g2.getFontAscent()-u8g2.getFontDescent();
           h=h/2;

       u8g2.setDrawColor(1);
       switch(selSetAlarmXItm)
      {
        case 1: u8g2.drawDisc(32,36,10); break;
        case 2: u8g2.drawDisc(64,36,10); break;
        case 3: u8g2.drawDisc(96,36,10); break;
        
        case 4: u8g2.drawBox(0,48,64,16); break;
        case 5: u8g2.drawBox(64,48,64,16); break;
      }
 
      u8g2.setDrawColor(2);

       String Name="SET ALARM";
       Name=Name+(X+1);
       
       u8g2.setCursor(64-(u8g2.getStrWidth("SET ALARM8")/2),8+h); u8g2.print(Name);
       u8g2.drawLine(0,16,128,16);
      
      u8g2.setCursor(32-(u8g2.getStrWidth("HH")/2),36+h); u8g2.print(AVar[X].hr);
      u8g2.setCursor(64-(u8g2.getStrWidth("MM")/2),36+h); u8g2.print(AVar[X].minit);
      u8g2.setCursor(96-(u8g2.getStrWidth("SS")/2),36+h); u8g2.print(AVar[X].seco);

    
     
      u8g2.drawStr(32-(u8g2.getStrWidth("SET")/2),56+h,"SET");
      u8g2.drawStr(96-(u8g2.getStrWidth("DELETE")/2),56+h,"DELETE");

      //int i;
      //for(i=0;i<=129;i+=16)
      //{u8g2.drawLine(i,0,i,64);
      //u8g2.drawLine(0,i,128,i);}

     
        u8g2.setDrawColor(1);
  
   }
   


  void button1press()
  {
    reading=digitalRead(nextButton);
    delay(10);
      if(reading!=prevReading)
      prevReading=reading;

        if(reading==0)
        nxtBtnPressStat=1;
        else
        nxtBtnPressStat=0;
    }  

     void button2press()
  {
    reading2=digitalRead(backButton);
    delay(10);
      if(reading2!=prevReading2)
      prevReading2=reading2;

        if(reading2==0)
        bakBtnPressStat=1;
        else
        bakBtnPressStat=0;
    }  

void ringAlarm()
{
  if(page!=21)
  {rtc.refresh();
  hr=rtc.hour();
  minit=rtc.minute();
  seco=rtc.second();
  }
  for(int i=0;i<3;i++)
  {
    if(AVar[i].status_==1)
    {
      if((hr==AVar[i].hr)&&(minit==AVar[i].minit)&&(seco==AVar[i].seco))
      {
        starttime=millis();
        endtime=starttime+((1000*30)-1);
       
      }
    }
    if(starttime<endtime)
    {
      starttime=millis();
      currentMillis = millis();
      
      if (currentMillis - previousMillis >= interval) 
      {
        previousMillis = currentMillis;
       counter++;
       Serial.println(counter);

        if(counter<9)
        {
        if (alarmState == LOW) 
        {alarmState = HIGH;}
        else 
        {alarmState = LOW; }
        }

        if(counter>=9&&counter<=13)
        {
          alarmState=LOW;
          if(counter==13)
          counter=1;
        }
      }

      digitalWrite(15,alarmState);
       
       button2press();
        if(bakBtnPressStat==1)
         {
           bakBtnPressStat==0; endtime=starttime+1;
         }
    }
    else
    digitalWrite(15,LOW);
  }
}

void selector()
{
 if(page==2)
 {
       button1press();
  
       if(nxtBtnPressStat)
       {
        selMainMenueItm++;
        if( selMainMenueItm > 4)
        selMainMenueItm=1;
       }
 
       switch(selMainMenueItm)
       {
         case 1: button2press();
                 if(bakBtnPressStat==1)
                 {page=1;nxtBtnPressStat=0;}
                 break;
     
         case 2: button2press();
                 if(bakBtnPressStat==1)
                 {page=21;nxtBtnPressStat=0;}
                 break;

         case 3: button2press();
                 if(bakBtnPressStat==1)
                 {page=22;nxtBtnPressStat=0;}
                 break;
         
     
       }
}

  else if(page==1)
   {
     button1press();
    if(nxtBtnPressStat==1)
     { page=2;nxtBtnPressStat=0;}
   }

   else if(page==21)
   {
      button1press();
      if(nxtBtnPressStat)
      { 
        selSetTimeItm++;
        if( selSetTimeItm > 5)
        selSetTimeItm=1;
      }
      
      switch(selSetTimeItm)
      {
        case 1: button2press();
                if(bakBtnPressStat==1)
                  {
                    if(hr<24)
                    hr++;
                    else
                    hr=0;
                    nxtBtnPressStat=0;
                  }break;
                  
         case 2: button2press();
                if(bakBtnPressStat==1)
                  {
                    if(minit<60)
                    minit++;
                    else
                    minit=0;
                    nxtBtnPressStat=0;
                  }break;          
          case 3: button2press();
                if(bakBtnPressStat==1)
                  {
                    if(seco<60)
                    seco++;
                    else
                    seco=1;
                    nxtBtnPressStat=0;
                  }break;
                  
           case 4: button2press();
                if(bakBtnPressStat==1)
                  {  rtc.set(seco,minit,hr, 6, 2, 5, 15);
                     page=2;nxtBtnPressStat=0;
                    }break;                 
        case 5: button2press();
                if(bakBtnPressStat==1)
               {page=2;bakBtnPressStat=0;} break;
      }
   }

   else if(page==22)
   {
         
         
         button1press();
         if(nxtBtnPressStat==1)
         {
          selSetAlarmItm++;
          if(selSetAlarmItm > 4)
          {selSetAlarmItm=1;}
            X=0;
           nxtBtnPressStat==0;
         }

      switch(selSetAlarmItm)
        {
         case 1: button2press();
                 if(bakBtnPressStat==1)
                 {page=221;
                  X=0;
                  nxtBtnPressStat=0;}
                  break;
         case 2: button2press();
                 if(bakBtnPressStat==1)
                 {page=222;
                  X=1;
                  nxtBtnPressStat=0;}
                  break;
         case 3: button2press();
                 if(bakBtnPressStat==1)
                 {page=223;
                  X=2;
                  nxtBtnPressStat=0;}
                  break;         
                  
         case 4: button2press();
                  if(bakBtnPressStat==1)
                  {page=2; bakBtnPressStat==0;}
                  break;
        }
   }

  else if(page==221||page==222||page==223)
  {
      
       
      button1press();
      if(nxtBtnPressStat)
      { 
        selSetAlarmXItm++;
        if( selSetAlarmXItm > 5)
        selSetAlarmXItm=1;
      }
      
      switch(selSetAlarmXItm)
      {
        case 1: button2press();
                if(bakBtnPressStat==1)
                  {
                    if(AVar[X].hr<24)
                    AVar[X].hr++;
                    else
                    AVar[X].hr=0;
                    nxtBtnPressStat=0;
                  }break;
                  
         case 2: button2press();
                if(bakBtnPressStat==1)
                  {
                    if(AVar[X].minit<60)
                    AVar[X].minit++;
                    else
                    AVar[X].minit=0;
                    nxtBtnPressStat=0;
                  }break;          
          case 3: button2press();
                if(bakBtnPressStat==1)
                  {
                    if(AVar[X].seco<60)
                    AVar[X].seco++;
                    else
                    AVar[X].seco=0;
                    nxtBtnPressStat=0;
                  }break;
                  
           case 4: button2press();
                if(bakBtnPressStat==1)
                  {  AVar[X].status_=1;
                     page=22;nxtBtnPressStat=0;
                    }break;
                  
         case 5: button2press();
                  if(bakBtnPressStat==1)
                  { AVar[X].status_=0;
                    page=22; bakBtnPressStat==0;
                    }break;
      }
  }

}
  
