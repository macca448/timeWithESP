## Time Keeping On your ESP Device

Some Example Sketch's showing a few different methods to keep time using your system's clock to produce Human Readable Local Time.


### Tested and works on Both ESP32 and ESP8266
#### time_timelib_ntp
1.	Uses the core C# `<time.h>` library to get local time from `pool.ntp.org` as local time using `tmelements_t`
2.	Converts `tmelements_t` to a local time `Unix Epoch` value
3.	Use the `Local Epoch` to set time in the Arduino Time Library `<TimeLib.h>`
4.	Two time variables are created for loop tasks `syncMin = minute();` and `lastSec = second();`
5.	Now the time is set WiFi is disconnected and our loop Serial Monitor Clock begins
6.	`lastSec = second();` creates the main clock function
7.	On seconds 10, 20, 30, 40 and 50 a `tmelements` time stamp `%A, %B %d %Y %H:%M:%S [%Z]` occurs `Friday, April 28 2023 12:10:30 [NZST]`
8.	All other seconds produce a `<TimeLib.h>` Custom Clock with this output `My Custom Clock 12:10:31  Date: 28/04/2023`
9.	The second task is on the minute roll `XX:XX:00` the NTP time sync process runs. In real world application this should be an hourly or even a daily process as you'll find your ESP is pretty good at keeping time.
