//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   
//
// Required Library :
//   https://github.com/PaulStoffregen/Time
//

#include "FwBox_NtpTime.h"


//
// NTP 設定
//
//static const char NTP_SERVER[] = "us.pool.ntp.org";
static const char NTP_SERVER[] = "tw.pool.ntp.org";
//static const char NTP_SERVER[] = "pool.ntp.org";

const int TIME_ZONE = 8; // Taipei Time
const unsigned int UDP_LOCAL_PORT = 8888;  // local port to listen for UDP packets

WiFiUDP Udp;


void FwBox_NtpTimeBegin()
{
  //
  // If no internet, set the default time.
  //
  setTime(6,0,0,1,1,2016);

  //
  // 設定 NTP 時間同步
  //
  NTP_TIME_DBG_PRINTLN("Starting UDP");
  Udp.begin(UDP_LOCAL_PORT);
  NTP_TIME_DBG_PRINT("Local port: ");
  NTP_TIME_DBG_PRINTLN(Udp.localPort());
  NTP_TIME_DBG_PRINTLN("waiting for sync");
  setSyncProvider(getNtpTime); // Set the external time provider
  setSyncInterval(36000); // Set the number of seconds between re-sync
}

//
// NTP code
//
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  for (int i = 0; i < 6; i++) { // Retry 6 times if "No NTP Response"
    while (Udp.parsePacket() > 0) ; // discard any previously received packets
    NTP_TIME_DBG_PRINTLN("Transmit NTP Request");
    // get a random server from the pool
    WiFi.hostByName(NTP_SERVER, ntpServerIP);
    NTP_TIME_DBG_PRINT(NTP_SERVER);
    NTP_TIME_DBG_PRINT(": ");
    NTP_TIME_DBG_PRINTLN(ntpServerIP);
    sendNtpPacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
      int size = Udp.parsePacket();
      if (size >= NTP_PACKET_SIZE) {
        NTP_TIME_DBG_PRINTLN("Receive NTP Response");
        Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];
        return secsSince1900 - 2208988800UL + TIME_ZONE * SECS_PER_HOUR;
      }
    }
    NTP_TIME_DBG_PRINTLN("No NTP Response :-(");
    delay(500);
  }
  return 0; // return 0 if unable to get the time
}

//
// send an NTP request to the time server at the given address
//
void sendNtpPacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
