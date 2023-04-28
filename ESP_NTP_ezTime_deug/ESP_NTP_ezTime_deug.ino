/*
 * Bare bones minimal ezTime sketch using NTP sync of 60 seconds
 * Debug info on Sync
 * Two main loop library defined clock stamps
 * 
 * Provide official timezone names for "myTZ.setLocation(F("TZ_Name"));" 
 * https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
 * 
 */

#include <ezTime.h>               //Library to use our system clock for NTP time

#if defined (ESP32)               //Lets the compiler work with
    #include <WiFi.h>             //ESP32
#elif defined (ESP8266)        //or
    #include <ESP8266WiFi.h>      //ESP8266 without changing any script
#endif

Timezone myTZ;                    //Creates our Timezone object to get "Local Time"

const char* ssid       = "Your_ssid";
const char* password   = "Your_password";

int lastSec, count = 0;

void setup() {
    Serial.begin(115200);while(!Serial){;}
    #if defined (ESP32)
      Serial.println("\nConnecting ESP32 to WiFi\n");
    #elif defined (ESP8266)
      Serial.println("\nConnecting ESP8266 to WiFi");
    #endif
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(". ");
        delay(300);
    }
    Serial.println("\n");
    setInterval(60);                        //Overrides default NTP update to 60 seconds
    setDebug(INFO);                         //Provides some key timing debug data on update
    waitForSync();                          //Default NTP method - must run loop "events();"
    myTZ.setLocation(F("Pacific/Auckland"));
    Serial.print(F("Local (GeoIP):   "));
  if (myTZ.setLocation()) {
    Serial.println("\n" + myTZ.dateTime());
  } else {
    Serial.println(errorString());
  }
  Serial.print("\nNTP Sync done.......\nStarting System Clock in: ");
  int i = 6;
  while(i > 0){i--;Serial.print(i);Serial.print(" ");delay(1000);}
  Serial.println("\n");
}

void loop() {
    events();                       //Needed to run the NTP setup method
    if(lastSec != myTZ.second()){
        lastSec = myTZ.second();
        count++; count %= 10;
            if(count < 9){          //Simple Clock Display with two Stamp formats
                Serial.println("RFC822 Stamp:\t" + myTZ.dateTime(RFC822));
            }else{ 
                Serial.println("\nDefault Stamp:\t" + myTZ.dateTime() + "\n");
            }
    }
}
