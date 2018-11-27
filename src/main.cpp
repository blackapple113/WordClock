#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "webpages/index.hh"
#include "webpages/notFound.hh"
#include "Adafruit_Wordclock.hh"

//LEDpin für die Ansteuerung der Neopixel
#define LED_PIN 4

//WLAN kennung (momentan Hotspot)
const char* wifi_ssid="WordClock";
const char* wifi_pwd="1234567890";

Adafruit_Wordclock strip = Adafruit_Wordclock(60, LED_PIN, NEO_GRB + NEO_KHZ800);

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

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pwd);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());

  handleServer();

  server.begin();
}

void loop() {
  //Clientanfragen
  server.handleClient();
  //Animationssteuerung
  strip.handleAnimations();
}