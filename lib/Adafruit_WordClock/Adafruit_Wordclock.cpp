/*--------------------------------------------------------------------
This Library is partly a copy of the Adafruit_NeoPixel library. This
copy is used to control a Adafruit ESP8266 feather Huzzah board with 
Adafruit WS2812b Neopixel strip with 60 LEDs/m. 
Goal of this library is to create animations while running and 
listening to userinput from a webinterface.
--------------------------------------------------------------------*/

#include "Adafruit_Wordclock.h"

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