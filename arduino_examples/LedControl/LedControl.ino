/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin("ssid", "password");
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());

}

void loop() {
    // wait for WiFi connection

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        http.begin("https://iot1-49786.firebaseio.com/led.json"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
                if(payload.equals("false")) {
                  digitalWrite(LED_BUILTIN, LOW);
                }
                if(payload.equals("true")) {
                  digitalWrite(LED_BUILTIN, HIGH);
                }
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
}
