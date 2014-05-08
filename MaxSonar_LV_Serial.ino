/*
MaxSonar LV series Serial connection to Teensy3
Since NONE of the scripts I found around worked as expected
(most don't work, some has tons of errors and results are unpredictables)
and I'm using a Teensy3, I wroted this that works and check the serial stream
to ensure that data has consistence and it's in the right order.
MaxSonar has great products but this one, checked for days with Logic Analyzer
has some troubles with serial output. The stream should be:
Rxxx<cr>
But it's not always like this, sometime the 3 bytes number it's 4 or more/less
and so on so every attemp to decode a fixed scheme fails.
This code identify first the R, then check for 3 bytes numbers and when <cr>
has identified it check that the data collected has the format expected.

*/

#include <Adafruit_GFX.h>
#include <OLED_SSD1332.h>
#include <SPI.h>
#include <SoftwareSerial.h>

//#define _SPRINTTT

#define oled_dc     9
#define oled_cs     10
#define oled_rst    14
#define buzzpin     6
#define maxbotix_en 3//Hi:Proximity, LOW:Stop      [in]
#define maxbotix_px 4//Low:No Obj, Hi:Obj Detected [out]

#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF
#define TRANSPARENT     -1

#define _MINDIST      15.24
#define _MAXDIST      600

boolean sonarEnabled = false;
volatile boolean sonarTrigger = false;


IntervalTimer sonarTimer;
OLED_SSD1332 oled = OLED_SSD1332(oled_cs,oled_dc,oled_rst);
/*
I'm using a MAX232 chip to invert RS232 data to TTL. If you
directly connect it without convert chip you need to set true
instead false
*/
SoftwareSerial maxsensor(7,8,false); // RX, TX

void setup() {
#if defined (_SPRINTTT)
  Serial.begin(9600);
  /*
  while (!Serial) { 
   ; 
   }
   */
#endif
  maxsensor.begin(9600);
  oled.begin();
  oled.setBitrate(24000000);
  oled.setCursor(0,0);
  pinMode(maxbotix_en,OUTPUT);
  enableSonar(true);
  sonarTimer.begin(sonarCallback, 10000L);
  oled.clearScreen();
  oled.setCursor(0,10);
  oled.print(" started ");
}


void loop() {
  if (sonarTrigger){
    noInterrupts();
    measureDistance();
    sonarTrigger = false;
    interrupts();
  }
}

void enableSonar(boolean val){
  if (val == true){
    if (!sonarEnabled){
      sonarEnabled = true;
      digitalWrite(maxbotix_en,HIGH);
      delay(500);
    }
  } 
  else {
    if (sonarEnabled){
      sonarEnabled = false;
      digitalWrite(maxbotix_en,LOW);
      delay(500);
    }
  }
}


void measureDistance(){
  if (sonarEnabled){
    bool dataReady = false;
    if (maxsensor.available()){
      maxsensor.flush();
      int idx = 0;
      int outs = 0;
      char collectData[4] = {
        0,0,0,0                                  };
      bool collectStarted = false;
      while (maxsensor.available()) { 
        char tempByte = maxsensor.read();
        if (tempByte == 0x52) collectStarted = true;
        if (collectStarted){    
          if (tempByte == 0x0D) {//carriage
            collectStarted = false;
            if (idx == 3){ // as expected
              dataReady = true;
              collectData[idx] = 0x00;//needed for atoi
            } 
            else {//something wrong
              dataReady = false;
            }
            //exit while loop
            break;
          } 
          else if (tempByte == 0x52) {
            idx = 0;//to prevent index jump
          }
          if (tempByte > 0x2F && tempByte < 0x3A){
            // data it's numeric
            collectData[idx] = tempByte;
            idx++;
          }
        }
      }//while
      if (dataReady){
        oled.clearScreen();
        oled.setCursor(0,10);
        outs = atoi(collectData);
        if (outs == 0 || outs == 255){
          oled.print("-out of range-");
        } 
        else {
          float mout = outs * 2.54;//convert in cm
          if (mout > 100){//check for max range
            if (mout >= _MAXDIST){
              oled.print("-too far!-");
            } 
            else {
              mout = mout/100;
              oled.print(mout,1);
              oled.print(" Meters");
            }
          } 
          else {
            if (mout <= _MINDIST){//check for min range
              oled.print("-too close!-");
            } 
            else {
              oled.print(mout,2);
              oled.print(" Cm");
            }
          }
        }
      }
    } 
    //delay(10);//if less will not work
  }
}




void sonarCallback() {
  sonarTrigger = true;
}









