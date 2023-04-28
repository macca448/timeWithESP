/*
 * This sketch will set your ESP32 Clock using NTP server method "ESP32Time.h" library
 * The "time_t" NTP functions are now part of the ESP32 WiFi library.
 * 
 * In it's current for the sketch has been configured to print a time stamp every second
 * A simple time stamp on all seconds except 00, 10, 20, 30, 40, 50.
 * On seconds 10, 20, 30, 40, 50 a simple tmelements string stamp is performed
 * Then on hour change an NTP update happens. WiFi is only connected when needed.
 * 
 * List of TZ strings here <https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv>
 * Time print string formatting options <https://cplusplus.com/reference/ctime/strftime/>
 * 
 * Date  : 28th April 2023
 * Author: macca448@gmail.com
 * Github: https://github.com/macca448/timeWithESP
 * 
 */

#define TZ_INFO "NZST-12NZDT,M9.5.0,M4.1.0/3"       //NZST/NZDT  UTC +12/+13       

#include <WiFi.h>
#include <ESP32Time.h>                              //Clock Linrary

ESP32Time rtc;                                      //Clock object
time_t now;                                         //So we use the ESP32 time functions

const char* ssid       = "Your_ssid";
const char* password   = "Your_password";
                           
const int32_t DEFAULT_TIME = 1609459200;            //1st Jan 2021 00:00:00
uint8_t syncHour, lastSec, lastMin;                 //For main loop timing statements

void NTPupdate(void){
    int32_t epoch = 0;
    Serial.println("\nConnecting to WiFi.....");
    WiFi.begin(ssid, password);
    WiFi.setAutoReconnect(false);
    while(WiFi.status() != WL_CONNECTED);
    Serial.print("IP address: ");Serial.println(WiFi.localIP());
    Serial.println("WiFi Connected.....");
    Serial.println("Getting NTP Time Update");
    while(epoch < DEFAULT_TIME){          //Wait till the NTP server reponds    
        epoch = time(&now);               //Gets the current Unix UTC/GMT time
    }
    rtc.setTime(epoch);                   //Sets RTC using UNIX Time (EPOCH) and converts it to local time
    Serial.println(rtc.getTime("NTP Update Success!!.....\n%a %d-%B-%Y %T UTC: %z"));
    if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Serial.println(" [WiFi] Disconnected from WiFi and radio off!!\n");
    }
    syncHour = rtc.getHour();               //For the re-sync clock timer
    lastSec = rtc.getSecond();              //starts our clock
    lastMin = rtc.getMinute();              //not used but you might like to?
    return;
}

void clocks(bool simple){                   //Two different ways to make a clock 
  if(simple){
    Serial.printf("\n Simple Clock %02u:%02u:%02u  Date: %02u/%02u/%u", rtc.getHour(), rtc.getMinute(),
                  rtc.getSecond(), rtc.getDay(), rtc.getMonth(), rtc.getYear());
  }else{
    Serial.println(rtc.getTime("\n\t (%Z) Local Time %I:%M (%p)\n"));     //Creates a tm elements Clock
  }
  return;
}

void setup(void) {
    Serial.begin(115200);
    while(!Serial);
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");      //Configures 2 x NTP server address's
    setenv("TZ", TZ_INFO, 1); tzset();                      //Setup your Timezone for true local time
    NTPupdate();
}

void loop(void) {
    if(lastSec != rtc.getSecond()){                         //Simple method for a one second timer 
        if(syncHour != rtc.getHour()){                      //Do NTP re-sync on the hour roll-over
            Serial.println(rtc.getTime("\n%T  Now doing NTP update\n"));
            NTPupdate();
        } 
        uint8_t nowSec = rtc.getSecond();                   
            if(nowSec == 10 || nowSec == 20 || nowSec == 30 || nowSec == 40 || nowSec == 50){
                clocks(0);                                  
            }else{                                          //Chooses which time stamp to use based on second value
                clocks(1);
            }
        lastSec = rtc.getSecond();                          //Keeps the one second clock ticking
    }
}
