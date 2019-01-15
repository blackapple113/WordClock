#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include "webpages/index.h"
#include "webpages/notFound.h"
#include "Adafruit_Wordclock.h"

//LEDpin für die Ansteuerung der Neopixel
#define LED_PIN 4

//WLAN kennung (momentan Hotspot)
const char* wifi_ssid="WordClock";
const char* wifi_pwd="1234567890";

// const char* wifi_ssid="NWLAN Gast";
// const char* wifi_pwd="19090808";

// const char* wifi_ssid="RoteWucht";
// const char* wifi_pwd="70777687578076323836";

//Initialisierung des LED-Streifens der WordClock
Adafruit_Wordclock strip = Adafruit_Wordclock(120, LED_PIN, NEO_GRB + NEO_KHZ800);

String new_state = "";

ESP8266WebServer server(80);

//Methode zum derzeitigen einstellen der vom Client
//übergebenen Daten zur LED steuerung
void handleLED() {
  new_state = server.arg("LEDstate");

if(new_state == "COLOR") {
    strip.setWc_animationId("color");
    wc_color_t c;
    c.red = server.arg("colorR").toInt();
    c.green = server.arg("colorG").toInt();
    c.blue = server.arg("colorB").toInt();
    strip.setWc_animationColor(c);
  } else if(new_state == "ANIMATION") {
    strip.setWc_animationId(server.arg("animation_id"));
    strip.setWc_animationDuration(server.arg("animation_duration").toInt());
    strip.setWc_animationTimeout(server.arg("animation_timeout").toInt());
  } else if(new_state == "TIME") {

  }

  server.send(200, "text/plane", new_state);
  new_state = "";
}

//Server pfadhandling zum Steuern der einzelnen Seiten
void handleServer() {
  //Root handling
  server.on("/", []() {
    String index = index_html;
    server.send(200, "text/html", index);
  });

  //URL to set the LED strip
  server.on("/setLED", handleLED);

  //404 Page not found
  server.onNotFound([]() {
    String notFound = notfound_html;
    server.send(404, "text/html", notFound);
  });
}

void setup() { 
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  strip.begin();
  strip.show();

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pwd);

  strip.setupCommunication();

  int x = 0;
  int cnt = 0;

  while(WiFi.status() != WL_CONNECTED && strip.getTime() == 0) {
    for(uint16_t i = 0; i < strip.numPixels(); ++i) {
      if(i >= 71 && i <= 74) {
        strip.setPixelColor(i, strip.Color(x,x,x));
      } else {
        strip.setPixelColor(i, strip.Color(0,0,0));
      }
    }
    if(cnt > 127) {
      x -= 2;
    } else {
      x += 2;
    }
    strip.show();
    cnt = (cnt + 2) % 256;
    delay(40);
  }

  for(uint16_t i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();

  Serial.println();
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  handleServer();

  server.begin();
}

void loop() {
  //Clientanfragen
  server.handleClient();
  //Animationssteuerung
  strip.handleAnimations();
  //NTP-Zeit
  strip.handleTime();
}
