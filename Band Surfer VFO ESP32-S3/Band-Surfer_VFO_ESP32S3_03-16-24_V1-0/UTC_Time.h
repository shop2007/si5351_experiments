#ifndef UTC_TIME_H
//#define UTC_TIME_H
#include <WiFi.h>
#include "time.h"
#include "sntp.h"

class UTC_TIME{

public:
void timeavailable(struct timeval *t);
void begin(const char *wifi_SSID, const char *wifi_Password);
String printLocalTime();

private:
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
unsigned long m_millis;


};//end of UTC_TIME class

#endif //UTC_TIME_H