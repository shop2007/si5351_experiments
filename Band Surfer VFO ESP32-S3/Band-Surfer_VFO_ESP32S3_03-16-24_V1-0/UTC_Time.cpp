#include "UTC_Time.h"

void UTC_TIME::timeavailable(struct timeval *t)
{
  //Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}



void UTC_TIME::begin(const char *wifi_SSID, const char *wifi_Password){

  sntp_servermode_dhcp(1);    // (optional)   
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  //connect to WiFi
  //Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(wifi_SSID, wifi_Password);
  //while (WiFi.status() != WL_CONNECTED) {
     delay(500);
      //Serial.print(".");
  //}
  //Serial.println(" CONNECTED");

}


String UTC_TIME::printLocalTime(){  
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char locTime[6];
  sprintf(locTime, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

  String UTC_time(locTime);
  if(millis() - m_millis > 5000){
  //Serial.println(UTC_time);
  m_millis = millis();
  }  
return UTC_time;
}

























