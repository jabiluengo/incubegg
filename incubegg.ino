#include <DS1307RTC.h>
#include <Wire.h>
#include <VarSpeedServo.h>
//#include <Servo.h>
#include <stdlib.h>
#include "DHT.h"
#include <Time.h>
#include <TimeAlarms.h>
#include <SoftwareSerial.h>
#include <SerialLCD.h>
//#include <NewSoftSerial.h>
#define BUZZER 0
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN 2
#define WATER_SENSOR 8


boolean running = true;
boolean newly_on = true;
boolean tenpup = true;
boolean tenp1firstpass = true;
int sensortime = 10;
int tumbletime = 30;
int wellcometime = 1000;
SerialLCD slcd(5,6);//this is a must, assign soft serial pins
int temp_min = 20;
DHT dht(DHTPIN, DHTTYPE);
//Servo myservoirauli;  // create servo object to control a servo 
//Servo myservohaize;  // create servo object to control a servo
VarSpeedServo Servoirauli;
VarSpeedServo Servovent;

const int relayPin =  4;      // the number of the relay pin
const float tenp1 = 36.5;
const float tenp2 = 37.5;
float tenpbelow = -40;
const int humimin = 50;
const int humimax = 60;
const int humiminlast = 70;
const int humimaxlast = 80;
boolean relayoff = true;
boolean servoright = true;
const int rightangle = 20;
const int lefttangle = 180;
const int servospeed = 20;
const int openanglevent = 60;
const int closeanglevent = 0;
const int servospeedvent = 10;
tmElements_t tm;
int HOLD_DELAY = 3000;    // Sets the hold delay of switch for LED state change
int switchPin  = 7;      // Switch is connected to pin x
unsigned long start_hold;
boolean allow = false;
int sw_state;
int sw_laststate = LOW;
const unsigned long eggtime = 60*60*24*21;
boolean finalcountdown = false;
const unsigned long incubeggdays = 21;
const int lastdays = 3;
int daysleft;

void setup()
{
//  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(WATER_SENSOR, INPUT);
  pinMode(switchPin, INPUT);    // Set the switch pin as input
  dht.begin();
  Alarm.timerRepeat(sensortime, sensorAlarm); 
  Alarm.timerRepeat(tumbletime, tumbleAlarm);
  Servoirauli.attach (3); 
//  myservoirauli.attach(3);  // attaches the servo on pin 9 to the servo object 
//  Servovent.attach (4);
  pinMode(relayPin, OUTPUT);
//  RTC.read(tm);
  pinMode(switchPin, INPUT);    // Set the switch pin as input
  pinMode(BUZZER, OUTPUT);

}

void soundAlarm()
{
	for(uint8_t i = 0;i < 5;i ++)
	{
		digitalWrite(BUZZER, HIGH);
		delay(50);
		digitalWrite(BUZZER, LOW);
		delay(50);
	}
}

boolean missWater()
{
	if(digitalRead(WATER_SENSOR) == LOW)
		return false;
	else return true;
}

// functions to be called when an alarm triggers:
void sensorAlarm(){

    char* tenpbuffer="AAAA";
    char* humibuffer="AAAA";
    slcd.setCursor(0, 0);
    float tenperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    dtostrf(tenperature, 4, 1, tenpbuffer);
    slcd.print("T ");
    slcd.print(tenpbuffer);
    slcd.print("  H ");
    dtostrf(humidity, 4, 1, humibuffer);
    slcd.print(humibuffer);
    if (tenperature > tenpbelow)
      tenpup = true;
    else
      tenpup = false;
    if (tenperature > tenp1 && tenpup){
       digitalWrite(relayPin, LOW);
       relayoff = true;
    }
     if (tenperature < tenp2 && !tenpup){
       digitalWrite(relayPin, HIGH);
       relayoff = false;
    }
    if (daysleft <= lastdays && !finalcountdown){
    slcd.setCursor(0, 1);
    slcd.print("ARRAUTZAK SARERA");
    }
    if (daysleft == 0 && finalcountdown)
    {
    slcd.setCursor(0, 1);
    slcd.print("    AKABO!!");
    finalcountdown = true;
    } 
    if (!finalcountdown)
    {
     slcd.setCursor(0, 1);
     slcd.print(daysleft,DEC);
     slcd.print(" egun falta");
    }
    if (missWater()){
     slcd.setCursor(0, 1);
     slcd.print("   UR FALTA!!   ");
     soundAlarm();
    }
    tenpbelow = tenperature;
}

void tumbleAlarm(){
  if (daysleft > lastdays){  
    slcd.setCursor(0, 1);
    if (servoright) {
    Servoirauli.slowmove(lefttangle,servospeed);
    slcd.print("Irauli gora    ");
    servoright = false;
  delay(5000);                           // waits for the servo to get there 
    }
    else { 
          Servoirauli.slowmove(rightangle,servospeed);
          slcd.print("Irauli behera   ");
          servoright = true;
          delay(5000);     // waits for the servo to get there     
    }
  }
}



void  loop(){ 
    RTC.read(tm);
    daysleft = incubeggdays + 1 - tm.Day;
  if (newly_on){
    slcd.begin();
    slcd.backlight();
    slcd.setCursor(0, 0);
    slcd.print("    Incubegg");
    slcd.setCursor(0, 1);
    slcd.print("     Kaixo!");
    delay(wellcometime);
    slcd.noBacklight();
    newly_on = false;
  }
   Alarm.delay(100);
  sw_state = digitalRead(switchPin);             // read input value
  if (sw_state == HIGH && sw_laststate == LOW){  // for button pressing
    start_hold = millis();                       // mark the time
    allow = true;                                // allow LED state changes
  }
     
  if (allow == true && sw_state == HIGH && sw_laststate == HIGH){  // if button remains pressed
      if ((millis() - start_hold) >= HOLD_DELAY){                   // for longer than x/1000 sec(s)
      RTC.set(0ul);
         slcd.setCursor(0, 1);
         slcd.print("     RESET       ");
         digitalWrite(BUZZER, HIGH);
         delay(50);
         digitalWrite(BUZZER, LOW);
         delay(50);
         allow = false;                                            // prevent multiple state changes
      }
  } 
 
  sw_laststate = sw_state;   

 
}

