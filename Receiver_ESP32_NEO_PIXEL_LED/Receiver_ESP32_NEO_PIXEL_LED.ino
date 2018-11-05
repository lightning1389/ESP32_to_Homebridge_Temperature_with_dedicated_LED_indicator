/**
 * WiFi Environment Sensor
 */

#include <FastLED.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


// Connection data to the WiFi
const char* ssid = "YOURSSID";
const char* password = "YOURPASSWORD";

// Webservice endpoint to push new sensor data to
const char* webservice = "http://192.168.178.35:3200/data/latest/30:AE:A4:23:2E:64";

// Time to (deep) sleep between measurements
const int sleepTimeInSeconds = 5;

// Variables holding (t)emperature, (h)umidity and (p)ressure
float t, h, p;

int warmth;


#define NUM_LEDS 1
#define DATA_PIN 2

CRGB leds[NUM_LEDS];

// Runs once on boot
void setup () {

  // Initialize serial port
  Serial.begin(115200);

  Serial.println("=== WiFi Sensor RGB Display ===");

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(" .");

 WiFi.begin(ssid, password);
    delay(500);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" done");

  Serial.print("Connected to ");
  Serial.print(ssid);
  Serial.print(" with IP address ");
  Serial.println(WiFi.localIP());
  Serial.print("This device's MAC address is ");
  Serial.println(WiFi.macAddress());
  delay(1000);

    // Send POST request to sensor data collecting webservice
    HTTPClient http;
    http.begin(webservice);
        delay(500);
    delay(500);


    Serial.println();
    Serial.print("Getting sensor data ...");

        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
                StaticJsonBuffer<512> jsonBuffer;
                JsonObject& root = jsonBuffer.parseObject(payload);

                float temp = root["temperature"];

                Serial.printf("temperature = %f\n",temp);

                FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
                warmth = map(temp, 26, 30, 160, 255);
                
                 if(temp <= 26) {
                  warmth = 160;
                }

                if(temp >= 30) {
                  warmth = 255;
                }
                
                Serial.printf("warmth = %i\n",warmth);
                
                }
                leds[0] = CHSV(warmth,255,255);
                FastLED.show();
               

            }
        
        else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();


  // Go to deep sleep
  Serial.println();
  Serial.print("Going to deep sleep for ");
  Serial.print(sleepTimeInSeconds);
  Serial.println(" seconds. Good night!");
  delay(500);
  
  ESP.deepSleep(sleepTimeInSeconds * 1e6);

}

// Run over and over again
void loop () {
}
