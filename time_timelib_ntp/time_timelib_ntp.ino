/*
 * This sketch has been tested to work on both 
 * ESP32 (V2.0.8) and ESP8266 (V3.1.2)
 * 
 * It provide you with a simple NTP Clock working in the following order
 * > Connect to WiFi 
 * > Set TimeZone and Get local local time (Note: local time is in tm elements format at this point)
 * > Do a "strftime" detailed time stamp 
 * > Convert local time "tmelements" to a "Local Time Unix Epoch" value using "mktime" 
 * > Use to set the Arduino "TimeLib.h" library 
 * > Use "Local Time Unix Epoch" to "setTime()" in "TimeLib.h"
 * > Create a one second and one minute task 
 * > Disconnect and turn off WiFi 
 * > Task #1 Simple Custom Serial Print Time Stamp every second using TimeLib functions
 * > Task #2 On the minute rollover repeat the time sync routine
 * 
 * Under normal use NTP time updates would only be needed a few times a day or even once a day
 * 
 * See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
 * 
 */

#if defined (ESP32)                           // Auto detect ESP Chip type
    #include <WiFi.h>
    #define DEFAULT_TIME 1609459200L          //Epoch for 1st Jan 2021 00:00:00
#elif defined (ESP8266)
    #include <ESP8266WiFi.h>    
#endif

#include <time.h>
#include <TimeLib.h>                          // Arduino Time Library

//TZ example for EST/EDT being UTC -5/-4 (Montreal, New York, Louisville, Indianapolis, Florida)
//#define TZ_INFO  "EST5EDT,M3.2.0,M11.1.0" 

//TZ for NZST/NZDT being UTC +12/+13 (New Zealand)
#define TZ_INFO "NZST-12NZDT,M9.5.0,M4.1.0/3" // Timezone string 
const char* ntpServer = "pool.ntp.org";       // All that's needed to connect to NTP server pools

#include "private.h"
//const char* ssid       = "Your_ssid";
//const char* password   = "Your_password";

uint8_t count = 0, syncMin, lastSec;          // A few variables for time tasks

time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss){
    tmElements_t tmSet;
    tmSet.Year = YYYY;
    tmSet.Month = MM;                         // This function makes our local epoch value
    tmSet.Day = DD;
    tmSet.Hour = hh;
    tmSet.Minute = mm;
    tmSet.Second = ss;
    return makeTime(tmSet);                   //Returns a Local Unix Epoch
}

void getLocalTime(void){                 //Function statement to get tm elements local time
    int yr, mo, dy, hr, mn, sc;
    time_t now = time(nullptr); 
    struct tm *now_tm;
    now = time(NULL);
    #if defined (ESP8266)                   //NTP Sync method for ESP8266
        while(now_tm != localtime(&now)){
            now_tm = localtime(&now);
            delay(500);                     //Small delay needed or we miss the packet
        }
    #elif defined (ESP32)                   //NTP Sync method for ESP32
        int32_t t = 0;
        while(t < DEFAULT_TIME){            //It needs a different wait iteration method
          now_tm = localtime(&now);
          t = time(&now);
          delay(500);
        }        
    #endif
    sc = now_tm->tm_sec;
    mn = now_tm->tm_min;
    hr = now_tm->tm_hour;
    yr = now_tm->tm_year -70;
    mo = now_tm->tm_mon +1;
    dy = now_tm->tm_mday;
    int32_t epoch = tmConvert_t(yr,mo,dy,hr,mn,sc); //Gets Epoch in local time
    setTime(epoch);                                 //Sets the TimeLib library's time
    syncMin = minute();                             //Used for the NTP re-sync duration loop function
    lastSec = second();                             //Starts our loop clock
  return;
}

void doWiFi(void){
  delay(200);
  Serial.printf("\n Connecting to %s ", ssid);
  WiFi.begin(ssid, password);                       //connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      Serial.print(".");
  }
  Serial.println("\n WiFi now Connected\n Syncing to Local NTP Time");
  getLocalTime();                           //Get NTP local time
  WiFi.disconnect(true);                    //disconnect WiFi as it's no longer needed
  WiFi.mode(WIFI_OFF);
  Serial.println("\n Clock Set & WiFi is Disconnected\n");
}

void tmClock(void){
    time_t now = time(NULL); 
    struct tm *now_tm;
    now_tm = localtime(&now);
    #if defined (ESP32)
        Serial.println(now_tm, "\n%A, %B %d %Y %H:%M:%S [%Z]\n");
    #elif defined (ESP8266)      
        char buffer [80];
        strftime (buffer,80,"\n%A, %B %d %Y %H:%M:%S [%Z]\n",now_tm);
        Serial.println(buffer);
    #endif
    return;
}

void myClock(void){
    char buff[50];
    sprintf(buff,"My Custom Clock %02d:%02d:%02d  Date: %02d/%02d/%d", 
            hour(),minute(),second(),day(),month(),year());
    Serial.println(buff);
    return;
}

void setup(void){
  Serial.begin(115200);
  configTime(0, 0, ntpServer);
  setenv("TZ", TZ_INFO, 1);
  tzset();
  doWiFi();
  tmClock();
}

void loop(){
    if(syncMin != minute()){
        doWiFi();
    }
    if(lastSec != second()){
        if(second() == 10 || second() == 20 || second() == 30 || second() == 40 || second() == 50){
            tmClock();
        }else{
            myClock();
        }
        lastSec = second();
    }
}
