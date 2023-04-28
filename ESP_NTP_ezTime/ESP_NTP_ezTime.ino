/*
 * This Sketch uses ezTime library to create a Serial monitor NTP clock
 * It is the bare minimum needed to do local time use NTP
 * The default NTP resync period of ezTime is 30 minutes
 * It prints a simple "Local Time" for seconds 1 to 9
 * On the minute a full default Time Stamp is printed
 * 
 * Provide official timezone names
 * https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
 * 
 */

#include <ezTime.h>               //Library to use our system clock for NTP time
Timezone myTZ;                    //Creates our Timezone object to get "Local Time"
#include "private.h"              //Holds my const char[] ssid & password 
#if defined (ESP32)               //Lets the compiler work with
    #include <WiFi.h>             //ESP32
#elif defined (ESP8266)           //or
    #include <ESP8266WiFi.h>      //ESP8266 without changing any script
#endif
int lastSec;

void getTheTime(bool simple){
    if(simple){
        Serial.printf("Local Time  %02u:%02u:%02u\n", myTZ.hour(), myTZ.minute(), myTZ.second());
    }else{
        Serial.println("\n" + myTZ.dateTime() + "\n");
    }
    return;
}

void setup() {
    Serial.begin(115200);while(!Serial){;}
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print(" Conecting to WiFi ");
    delay(100);
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(". ");
        delay(300);
    }
    Serial.println("\n Now Conected to WiFi and getting Time Update\n");
    waitForSync();                          //Default method for NTP sync every 30min
    myTZ.setLocation(F("Pacific/Auckland"));//Sets our Timezone object
    lastSec = myTZ.second();
}

void loop() {
    events();                       //Needed to run the ezTime setup method
    if(myTZ.second() != lastSec){
        lastSec = myTZ.second();
           if(lastSec == 0 || lastSec == 10 || lastSec == 20 || 
               lastSec == 30 || lastSec == 40 || lastSec == 50){
                getTheTime(0);
            }else{
                getTheTime(1);
            }
    }
}
