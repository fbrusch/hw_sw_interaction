/**
   StreamHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);


  USE_SERIAL.begin(115200);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
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

      HTTPClient http;
      USE_SERIAL.print("[HTTP] begin...\n");

      http.begin("https://iot1-49786.firebaseio.com/led.json");
      http.addHeader("Accept", "text/event-stream");
      
      USE_SERIAL.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      if (httpCode > 0) {
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) {

        int len = http.getSize();
        uint8_t buff[128] = { 0 };
        WiFiClient * stream = http.getStreamPtr();

        while (http.connected() && (len > 0 || len == -1)) {

          // save the current state as the last state, for next time through the loop
          // get available data size
          size_t size = stream->available();
          if (size) {
            USE_SERIAL.println("**GOT DATA**");
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            buff[c] = '\0';
            char* command = strchr((char*)buff, '\n')+1;
            command = strchr(command, ':')+1;
            DynamicJsonBuffer jsonBuffer;
            JsonObject& root = jsonBuffer.parseObject(command);
            String data = root["data"];
            
            String s = String((const char*)command);
            if(data.equals("false")) {
              USE_SERIAL.println("dovrei spegnere il led");
              digitalWrite(LED_BUILTIN, LOW);
            }

            if(data.equals("true")) {
              USE_SERIAL.println("dovrei accendere il led");
              digitalWrite(LED_BUILTIN, HIGH);
            }

            
            // write it to Serial
            USE_SERIAL.print(data);

            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }

        USE_SERIAL.println();
        USE_SERIAL.print("[HTTP] connecstrtion closed or file end.\n");

      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
 
}

