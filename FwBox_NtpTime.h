//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   
//
// Required Library :
//

#ifndef __FWBOX_NTPTIME_H__
#define __FWBOX_NTPTIME_H__

#include <Arduino.h>
#include <TimeLib.h> // Library for Time function
#include <WiFiUdp.h>
//
// Wifi library
//
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif

//
// Debug definitions
//
#define FW_BOX_NTP_TIME_DEBUG 0

#if FW_BOX_NTP_TIME_DEBUG == 1
  #define NTP_TIME_DBG_PRINT(VAL) Serial.print(VAL)
  #define NTP_TIME_DBG_PRINTLN(VAL) Serial.println(VAL)
  #define NTP_TIME_DBG_PRINTF(FORMAT, ARG) Serial.printf(FORMAT, ARG)
  #define NTP_TIME_DBG_PRINTF2(FORMAT, ARG1, ARG2) Serial.printf(FORMAT, ARG1, ARG2)
#else
  #define NTP_TIME_DBG_PRINT(VAL)
  #define NTP_TIME_DBG_PRINTLN(VAL)
  #define NTP_TIME_DBG_PRINTF(FORMAT, ARG)
  #define NTP_TIME_DBG_PRINTF2(FORMAT, ARG1, ARG2)
#endif


void FwBox_NtpTimeBegin();
time_t getNtpTime();
void sendNtpPacket(IPAddress &address);


#endif // __FWBOX_NTPTIME_H__
