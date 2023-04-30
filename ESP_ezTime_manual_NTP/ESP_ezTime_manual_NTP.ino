/*
 * This Sketch uses ezTime library to create a Serial monitor NTP clock
 * It is the bare minimum needed to do local time
 * It uses a manual connection method (no events()) and resync's every minute
 * It prints a simple "Local Time" for seconds 1 to 9
 * then prints a tmelements strftime time string on 10, 20, 30, 40 and 50
 * 
 * Provide official timezone names
 * https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
 * 
 * For Timezone codes for your region
 * See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv 
 * 
 * Time print string formatting options 
 * <https://cplusplus.com/reference/ctime/strftime/>
 * 
 * Date  : 28th April 2023
 * Author: macca448@gmail.com
 * Github: https://github.com/macca448/timeWithESP
 * 
 */


#define TZ_INFO "NZST-12NZDT,M9.5.0,M4.1.0/3"       //NZST/NZDT  UTC +12/+13  
  
#include <ezTime.h>                                 //Library to use our system clock for NTP time
Timezone myTZ;                                      //Creates our Timezone object to get "Local Time"

#if defined (ESP32)               //Lets the compiler work with
    #include <WiFi.h>             //ESP32
#elif defined (ESP8266)           //or
    #include <ESP8266WiFi.h>      //ESP8266 without changing any script
#endif

const char* ssid       = "Your_ssid";
const char* password   = "Your_password";

uint8_t lastSec, lastMin;

void getTheTime(bool simple){
    if(simple){
        Serial.printf("Local Time  %02u:%02u:%02u\n", myTZ.hour(), myTZ.minute(), myTZ.second());
    }else{
        Serial.println("\n" + myTZ.dateTime() + "\n");
    }
    return;
}

void doNTP(void){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print(" Conecting to WiFi ");
    delay(500);
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(". ");
        delay(300);
    }
    Serial.println("\n Now Conected to WiFi and getting Time Update\n");
    waitForSync();
    Serial.println("NTP Update Sucess " + myTZ.dateTime() + "\n");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi Disconnected - returning to Loop");
    lastMin = myTZ.minute();
    lastSec = myTZ.second();
    return;
}

void setup() {
    Serial.begin(115200);while(!Serial){;}
    myTZ.setPosix(TZ_INFO);
    doNTP();
}

void loop() {

    if(lastMin != myTZ.minute()){
        doNTP();
    }
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
