/*--------------------------------------------------------------------
This library is used to control a Adafruit ESP8266 feather Huzzah
board  with Adafruit WS2812b Neopixel strip with 60 LEDs/m. 
Goal of this library is to create animations while running and 
listening to userinput from a webinterface.
--------------------------------------------------------------------*/

#ifndef ADAFRUIT_WORDCLOCK_H
#define ADAFRUIT_WORDCLOCK_H

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>

struct wc_color_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    bool operator==(const wc_color_t &other) {
        if(this->red == other.red && this->green == other.green && this->blue == other.blue) {
            return true;
        }
        return false;
    }

    bool operator!=(const wc_color_t &other) {
        return !(*this == other);
    }

};

struct wc_anidata_t{
    String id;
    wc_color_t color;
    unsigned long timeout;
    unsigned long duration;
    uint8_t std;
    uint8_t min;

    bool operator==(const wc_anidata_t &other) {
        if(this->id == other.id && this->color == other.color && this->timeout == other.timeout && this->duration == other.duration && this->std == other.std && this->min == other.min) {
            return true;
        }
        return false;
    }

    bool operator!=(const wc_anidata_t &other) {
        return !(*this == other);
    }
};


class Adafruit_Wordclock : public Adafruit_NeoPixel {
    private:
    wc_anidata_t wc_animation;
    unsigned long currentTime;
    unsigned long previousTime;
    unsigned long globalTimeout;
    uint16_t globalColorCount = 0;

    void sendNTPpacket(IPAddress& address);
    int getMinutes(uint32_t UNIXTime);
    int getHours(uint32_t UNIXTime);

    public:
    //constructors
    Adafruit_Wordclock(uint16_t n, uint8_t p=4, neoPixelType t=NEO_GRB + NEO_KHZ800);
    Adafruit_Wordclock(void);

    //getters and setters
    void setWc_animationId(String id);
    void setWc_animationColor(wc_color_t c);
    void setWc_animationTimeout(unsigned long t);
    void setWc_animationDuration(unsigned long d);
    void setWc_animationTime(uint8_t std, uint8_t min);
    const String getWc_animationId();
    const wc_color_t getWc_animationColor();
    const unsigned long getWc_animationTimeout();
    const unsigned long getWc_animationDuration();

    //various methods
    uint32_t getTime();
    void setupCommunication();
    void handleAnimations();
    void handleTime();
    void setTime(uint8_t std, uint8_t min, bool es_ist = true);
    void setPosMin(uint8_t min);
    void setPosStd(uint8_t std);
    void resetRef();
    void setRef(const int& start, const int& end);
};

#endif
