/*--------------------------------------------------------------------
This Library is partly a copy of the Adafruit_NeoPixel library. This
copy is used to control a Adafruit ESP8266 feather Huzzah board with 
Adafruit WS2812b Neopixel strip with 60 LEDs/m. 
Goal of this library is to create animations while running and 
listening to userinput from a webinterface.
--------------------------------------------------------------------*/

#include "Adafruit_Wordclock.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define SIZE 114 

bool ref[SIZE]; 

IPAddress timeServerIP;          // time.nist.gov NTP server address
const char* NTPServerName = "0.de.pool.ntp.org";

const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

WiFiUDP UDP;                     //Erstellen einer Instanz für das Senden und Empfangen über UDP

uint32_t Adafruit_Wordclock::getTime() {
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

void Adafruit_Wordclock::sendNTPpacket(IPAddress& address) {
  Serial.print("Time Server IP: ");
  Serial.println(address);
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

int Adafruit_Wordclock::getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

int Adafruit_Wordclock::getHours(uint32_t UNIXTime) {
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
void Adafruit_Wordclock::setWc_animationTime(uint8_t std, uint8_t min) {
    wc_animation.std = std;
    wc_animation.min = min;
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

void Adafruit_Wordclock::setupCommunication() {
    Serial.println("Starting UDP");
    UDP.begin(123);
    Serial.print("Local port: ");
    Serial.println(UDP.localPort());
    Serial.println();

    while(!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
        Serial.println("DNS lookup failed. Rebooting.");
    }
}

void Adafruit_Wordclock::handleAnimations() {
    static Color activeColor {Color(127,127,127)};

    if(wc_animation.id == "color") {
        for(uint16_t i = 0; i < numPixels(); ++i) {
            activeColor = Color(wc_animation.color.red, wc_animation.color.green, wc_animation.color.blue);
            setPixelColor(i, activeColor);
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
    } else if(wc_animation.id = "time") {
        resetRef();
        for(uint16_t i = 0; i < numPixels(); ++i) {
            if(ref[i]) {
                setPixelColor(i, activeColor);
            } else {
                setPixelColor(i, Color(0,0,0));
            }
        }
        show();
    }
}

void Adafruit_Wordclock::resetRef() {
    for(int i = 0; i < SIZE; ++i) {
        ref[i] = false;
    }
}

void Adafruit_Wordclock::setRef(const int& start, const int& end) {
    for(int i = start; i <= end; ++i) {
        ref[i] = true;
    }
}


void Adafruit_Wordclock::setPosStd(uint8_t std) {
    switch(std%12) {
        case 0:
            setRef(13,17);
            break;
        case 1:
            setRef(46,48);
            break;
        case 2:
            setRef(53,56);
            break;
        case 3:
            setRef(42,45);
            break;
        case 4:
            setRef(35,38);
            break;
        case 5:
            setRef(57,60);
            break;
        case 6:
            setRef(24,28);
            break;
        case 7:
            setRef(18,23);
            break;
        case 8:
            setRef(31,34);
            break;
        case 9:
            setRef(4,7);
            break;
        case 10:
            setRef(1,4);
            break;
        case 11:
            setRef(60,62);
            break;
    }
}

void Adafruit_Wordclock::setPosMin(uint8_t min) {
    switch(min%5) {
        case 1:
            ref[113] = true;
            break;
        case 2:
            ref[113] = true;
            ref[101] = true;
            break;
        case 3:
            ref[113] = true;
            ref[101] = true;
            ref[12] = true;
            break;
        case 4:
            ref[113] = true;
            ref[101] = true;
            ref[12] = true;
            ref[0] = true;
            break;
        default:
            ref[113] = false;
            ref[101] = false;
            ref[12] = false;
            ref[0] = false;
    }

    int z = 110;
    for(int i = min; i%5 > 0; --i) {
        ref[z] = true;
        ++z;
    }

    uint8_t min_5 = min - min%5;
    switch(min_5) {
        case 0:
            setRef(9,11); //Uhr
            break;
        case 5:
            setRef(102,105); //fünf (minute)
            setRef(75,78);   //nach
            break;
        case 10:
            setRef(90,93); //zehn (minute)
            setRef(75,78); //nach
            break;
        case 15:
            setRef(79,85); //viertel
            setRef(75,78); //nach
            break;
        case 20:
            setRef(94,100); //zwanzig
            setRef(75,78); //nach
            break;
        case 25:
            setRef(102,105);  //fünf (minute)
            setRef(68,70); //vor
            setRef(64,67); //halb
            break;
        case 30:
            setRef(64,67); //halb
            break;
        case 35:
            setRef(102,105);  //fünf (minute)
            setRef(75,78); //nach
            setRef(64,67); //halb
            break;
        case 40:
            setRef(94,100); //zwanzig
            setRef(68,70); //vor
            break;
        case 45:
            setRef(79,85); //viertel
            setRef(68,70); //vor
            break;
        case 50:
            setRef(90,93); //zehn (minute)
            setRef(68,70); //vor
            break;
        case 55:
            setRef(102,105);  //fünf (minute)
            setRef(68,70); //vor
            break;
    }
}

void Adafruit_Wordclock::setTime(uint8_t std, uint8_t min, bool es_ist) {
    resetRef();

    if(es_ist) {
        setRef(111,112);
        setRef(107,109);
    }
    if(min >= 25) {
        ++std;
    }
    if(min-min%5 > 0 && std%12 == 1) {
        ref[49] = true;
    }
    setPosMin(min);
    setPosStd(std);
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
        timeUNIX = time+3600;
        Serial.print("NTP response:\t");
        Serial.println(timeUNIX);
        lastNTPResponse = currentMillis;
    } else if ((currentMillis - lastNTPResponse) > 3600000) {
        Serial.println("More than 1 hour since last NTP response. Rebooting.");
        Serial.flush();
        ESP.reset();
    }

    uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse)/5000;
    if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print
        prevActualTime = actualTime;
        Serial.printf("\rUTC time:\t%d:%d   ", getHours(actualTime), getMinutes(actualTime));
        setTime(getHours(actualTime), getMinutes(actualTime));
        setWc_animationId("time");
        setWc_animationTime(getHours(actualTime), getMinutes(actualTime));
    }  
}
