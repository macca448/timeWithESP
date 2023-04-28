## Time Keeping on an ESP Device

Some Example Sketch's showing a few different methods to keep time using your system's clock to produce Human Readable Local Time.
1. All testing was done using Arduino IDE v1.8.19 using ESP32 v2.0.8 and ESP8266 v3.1.2 board profiles
2. All sketch's produce local time with Daylight offset if your time zone supports it
3. All example sketch's use the clock that is created to do main loop clock 
4. The `time.h` library should be installed by default and any other library mentioned can be installed via the Arduino IDE
5. Every Sketch by default requires WiFi functionality
6. The generic sketchs should actually work on any network aware Arduino controller
7. Link for `Olson` Timezone names for the `ezTime` library [here](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)
8. Link for `POSIX` Timezone string database [here ](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)

![POSIX explained](https://github.com/macca448/timeWithESP/blob/main/assets/posix_detail.jpg)

#### The basics of how time is created on your controller
Our controllers and hardware RTC modules have ossicilators that are used to create constant time periods and these are used to create the `millis()` and `micros()` functions in our Arduino sketch's. 

The `time.h` library looks at the current controllers constant running clocks and sync's it to the `Unix Epoch` that you pass to it. That means when you do an `NTP` time sync the `time.h` library does a calculation to convert the `Epoch` value which you pass being seconds since `January 1st, 1970 at 00:00:00 UTC` then calculates how to make your current `millis()` value into a human readable clock for both time and date. If you use `Timezone` and `localtime()` then you will also not only get true local time but your clock will be aware of `Daylight`   time offset changes.

#### Sketch's in this archive

| SKETCH                | Description                                                                                               | Chip     | Time Libraries Used |
|-----------------------|-----------------------------------------------------------------------------------------------------------|:--------:|-------------------------|
|                       |                                                                                                           |          |                         |
| ESP_time_32           | The simplest form of keeping time with the `C#` time library using `tmelements_t`                         | Both     | `time.h`                |
| time_timelib_ntp      | Converts `tmelements` to `local time epoch` so we can use `uint8_t` time variables anywhere in the sketch | Both     | `time.h` `TimeLib.h`    |
| ESP_NTP_ezTime        | Let's `ezTime` do the heavy lifting for NTP re-sync                                                       | Both     | `ezTime.h`              |
| ESP_NTP_ezTime_deug   | Add's NTP connection `debug` info to the `ezTime` basic sketch                                            | Both     | `ezTime.h`              |
| ESP_ezTime_manual_NTP | Using the `ezTime` library and manually doing your NTP update schedule                                    | Both     | `ezTime.h`              |
| ESP32_time_64         | Takes advantage of the `time` functionality that is now native to the `ESP32 WiFi` library                | ESP32    |                         |
| ESP32Time_RTC         | Easy to use library `ESP32Time` that offers flexible sketch time functions                                | ESP32    | `ESP32Time.h`           |

###  What's in the Examples 

####  ESP_time_32
    1. On boot start WiFi and sync to localtime() and do a detailed tmelents time stamp
    2. Disconnect from WiFi and run a simpler tmelements time stamp every second
    3. After 5 minutes do a `NTP` sync routine

#### time_timelib_ntp sketch

![TimeLib.h library install](https://github.com/macca448/timeWithESP/blob/main/assets/TimeLib.jpg)

1. Uses the core C# <time.h> library to get local time from pool.ntp.org as local time using tmelements_t
2. Converts tmelements_t to a local time Unix Epoch value
3. Use the Local Epoch to set time in the Arduino Time Library <TimeLib.h>
4. Two time variables are created for loop tasks "syncMin = minute();" and "lastSec = second();"
5. Now the time is set WiFi is disconnected and our loop Serial Monitor Clock begins
6. "lastSec = second();" creates the main clock function
7. On seconds 10, 20, 30, 40 and 50 a "tmelements" time stamp "%A, %B %d %Y %H:%M:%S [%Z]" which looks like "Friday, April 28 2023 12:10:30 [NZST]"
8. All other seconds produce a "<TimeLib.h>" Custom Clock with this output "My Custom Clock 12:10:31  Date: 28/04/2023"
9. The second task is on the minute roll "XX:XX:00" the NTP time sync process runs. In real world application this should be an hourly or even a daily process as you'll find your ESP is pretty good at keeping time.


###  ESP_NTP_ezTime

![exTime.h install](https://github.com/macca448/timeWithESP/blob/main/assets/ezTime.jpg)

1. Just a default method sketch that will auto resync the time every 30 minutes
2. Two time stamps are presented with two methods

###  ESP_NTP_ezTime_deug
1. Set to resync every 60 seconds and give some very cool update debug information
2. Uses two internal time stamps 

###  ESP_ezTime_manual_NTP
1. Using ezTime library manually by disconnecting from WiFi after NTP sync
2. Re-sync every 60 seconds as an example

###  ESP32_time_64
1. Sets the ESP32 clock using the internal time library (int64_t epoch)making it very lightweight
2. Detailed "tmelenents" time stamp on NTP sync then WiFi disconnects
3. Uses a simple minute count to enable NTP resync

### ESP32Time_RTC

![ESP32Time.h install](https://github.com/macca448/timeWithESP/blob/main/assets/ESP32Time.jpg)

1. Connects and sync's time then disconnects
2. Two stamp types depending on current second value
3. NTP re-sync on the hour roll-over
