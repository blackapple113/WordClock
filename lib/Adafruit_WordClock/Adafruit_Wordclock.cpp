/*--------------------------------------------------------------------
This Library is partly a copy of the Adafruit_NeoPixel library. This
copy is used to control a Adafruit ESP8266 feather Huzzah board with 
Adafruit WS2812b Neopixel strip with 60 LEDs/m. 
Goal of this library is to create animations while running and 
listening to userinput from a webinterface.
--------------------------------------------------------------------*/

#include "Adafruit_Wordclock.h"
#include <ESP8266WiFi.h>

IPAddress timeServerIP;          // time.nist.gov NTP server address
const char* NTPServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

WIFIUDP UDP;                     //Erstellen einer Instanz für das Senden und Empfangen über UDP

uint32_t getTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

int getHours(uint32_t UNIXTime) {
  return UNIXTime / 3600 % 24;
}

Adafruit_Wordclock::Adafruit_Wordclock(uint16_t n, uint8_t p, neoPixelType t) : Adafruit_NeoPixel(n, p, t)
{
}

Adafruit_Wordclock::Adafruit_Wordclock(void) : Adafruit_NeoPixel()
{
}

void Adafruit_Wordclock::setWc_animationId(String id) {
    wc_animation.id = id;
}
void Adafruit_Wordclock::setWc_animationColor(wc_color_t c) {
    wc_animation.color = c;
}
void Adafruit_Wordclock::setWc_animationTimeout(unsigned long t) {
    wc_animation.timeout = t;
}
void Adafruit_Wordclock::setWc_animationDuration(unsigned long d) {
    wc_animation.duration = d;
}
const String Adafruit_Wordclock::getWc_animationId() {
    return wc_animation.id;
}
const wc_color_t Adafruit_Wordclock::getWc_animationColor() {
    return wc_animation.color;
}
const unsigned long Adafruit_Wordclock::getWc_animationTimeout() {
    return wc_animation.timeout;
}
const unsigned long Adafruit_Wordclock::getWc_animationDuration() {
    return wc_animation.duration;
}

uint32_t Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void Adafruit_WordClock::setupCommunication() {
    Serial.println("Starting UDP");
    UDP.begin(123);
    Serial.print("Local port: ");
    Serial.println(UDP.localPort());
    Serial.println();

    if(!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
        Serial.println("DNS lookup failed. Rebooting.");
        Serial.flush();
        ESP.reset();
    }
}

void Adafruit_Wordclock::handleAnimations() {
    if(wc_animation.id == "color") {
        for(uint16_t i = 0; i < numPixels(); ++i) {
            setPixelColor(i, Color(wc_animation.color.red, wc_animation.color.green, wc_animation.color.blue));
        }
        show();
        setWc_animationId("");
        Serial.print("Farbe geändert");
    } else if(wc_animation.id == "rainbow") {
        if((millis()-previousTime) >= wc_animation.timeout) {
            previousTime = millis();
            globalColorCount = (globalColorCount+1)%256;
            for(uint16_t i = 0; i < numPixels(); ++i) {
                setPixelColor(i, Wheel((i+globalColorCount) & 255));
            }
            show();
        } 
    }
}

void Adafruit_Wordclock::handleTime() {
    static unsigned long intervalNTP = 60000; // Request NTP time every minute
    static unsigned long prevNTP = 0;
    static unsigned long lastNTPResponse = millis();
    static uint32_t timeUNIX = 0;

    static unsigned long prevActualTime = 0;

    unsigned long currentMillis = millis();

    if (currentMillis - prevNTP > intervalNTP) { // If a minute has passed since last NTP request
        prevNTP = currentMillis;
        Serial.println("\r\nSending NTP request ...");
        sendNTPpacket(timeServerIP);               // Send an NTP request
    }

    uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time
    if (time) {                                  // If a new timestamp has been received
        timeUNIX = time;
        Serial.print("NTP response:\t");
        Serial.println(timeUNIX);
        lastNTPResponse = currentMillis;
    } else if ((currentMillis - lastNTPResponse) > 3600000) {
        Serial.println("More than 1 hour since last NTP response. Rebooting.");
        Serial.flush();
        ESP.reset();
    }

    uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse)/1000;
    if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print
        prevActualTime = actualTime;
        Serial.printf("\rUTC time:\t%d:%d:%d   ", getHours(actualTime), getMinutes(actualTime), getSeconds(actualTime));
    }  
}
