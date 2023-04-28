/*
 * NTP time in it's simplest form using <time.h> C# library
 * In essence this sketch should work on any network connected device
 * by changing the scope to suit your method
 * 
 * Sketch was tested on both ESP32 and ESP8266
 * Testing used Arduino IDE v1.8.19
 * using board libraries ESP32 v2.0.8 and ESP8266 3.1.2
 * 
 * Sketch is configured to start and connect to your WiFi
 * Get NTP local time (You must configure the TZ string for your location)
 * Once update complete disconnect from Wifi and turn radio off
 * Run a simple Serial Monitor clock (Turn on your "Show timestamp" to see the accuracy)
 * after 5 minutes re-start WiFi and re-sync
 * 
 * List of TZ strings here <https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv>
 * Time print string formatting options <https://cplusplus.com/reference/ctime/strftime/>
 * 
 * Date  : 28th April 2023
 * Author: macca448@gmail.com
 * Github: https://github.com/macca448/timeWithESP
 * 
 */

#define TZ_INFO "NZST-12NZDT,M9.5.0,M4.1.0/3"  //NZST/NZDT  UTC +12/+13       

#if defined (ARDUINO_ARCH_ESP32)                           // Auto detect ESP Chip type
    #include <WiFi.h>    
#elif defined (ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>    
#endif

#include <time.h>                                   //C# time library
/* Globals */
time_t now;                         // this is the epoch
tm now_tm;                            // the structure tm holds time information in a more convenient way

#include "private.h"                                //SSID and Password
//const char* ssid       = "Your_ssid";
//const char* password   = "Your_password";

const int32_t DEFAULT_TIME = 1609459200;  //1st Jan 2021 00:00:00                           
uint8_t lastSec, lastMin, count;         

void updateTime(void){
    uint8_t min = 0, sec = 0;
    time_t now = time(NULL); 
    struct tm *now_tm;
    now_tm = localtime(&now);
    sec = now_tm->tm_sec;
    if(lastSec != sec){
        char buffer [60];
        strftime (buffer,60,"Current Time %I:%M:%S [%p]",now_tm);
        Serial.println(buffer);
        lastSec = sec;
    }
    min = now_tm->tm_min;
    if(lastMin != min){
        lastMin = min;
        count++;
            if(count >= 5){
                NTPupdate();
            }
    }  
}

void NTPupdate(void){
    delay(500);
    Serial.printf("\nConnecting to WiFi %s ", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setAutoReconnect(false);
    while(WiFi.status() != WL_CONNECTED){Serial.print(".");delay(250);}
    Serial.print("\nIP address: ");Serial.println(WiFi.localIP());
    Serial.println("\nWiFi Connected.....");
    Serial.println("Getting NTP Time Update");
    time_t now = time(nullptr); 
    struct tm *now_tm;
    now = time(NULL);
    int32_t t = 0;
    while(t < DEFAULT_TIME){
      #if defined (ARDUINO_ARCH_ESP8266)  
        now_tm = localtime(&now);
      #endif
        t = time(&now);
        delay(500);                     //Small delay needed or we miss the packet
    }
    #if defined (ARDUINO_ARCH_ESP32)
        now_tm = localtime(&now);
    #endif 
    delay(1000);   
    char buffer [80];
    strftime (buffer,80," NTP Sync Stamp %A, %B %d %Y %H:%M:%S [%Z]\n",now_tm);
    Serial.println(buffer);
    lastSec = now_tm->tm_sec;
    lastMin = now_tm->tm_min;
    if(WiFi.status() == WL_CONNECTED){
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Serial.println(" [WiFi] Disconnected from WiFi and radio off!!\n");
    }
    count = 0;
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
    updateTime();
}
