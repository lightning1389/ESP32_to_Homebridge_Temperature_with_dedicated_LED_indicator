/**
 * WiFi Environment Sensor
 */

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

// Connection data to the WiFi
const char* ssid = "YOURSSID";
const char* password = "YOURPASSWORDHERE";

// Webservice endpoint to push new sensor data to
// This would be the raspberry pi running Homebridge
const char* webservice = "http://192.168.178.35:3200/data";
//const char* webservice = "http://192.168.178.147:3366/data";

// Time to (deep) sleep between measurements
const int sleepTimeInSeconds = 5 * 60;

// Variables holding (t)emperature, (h)umidity and (p)ressure
float t, h, p;

// Runs once on boot
void setup () {

  // Initialize serial port
  // Can be commented out, if not needed
  Serial.begin(115200);

  

  Serial.println("=== WiFi Environment Sensor ===");

  // Initialize wires
  Wire.begin(21, 22);

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

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

  // Check for BME280 availability and initialize it if found
  if (!bme.begin(0x76)) {
    Serial.println();
    Serial.println("ERROR: Could not find BME280 sensor, check wiring");
    while (1);
  }
  else {
    bme.setSampling(Adafruit_BME280::MODE_FORCED, Adafruit_BME280::SAMPLING_X1, Adafruit_BME280::SAMPLING_X1, Adafruit_BME280::SAMPLING_X1, Adafruit_BME280::FILTER_OFF, Adafruit_BME280::STANDBY_MS_1000);
  }

  delay(1000);

  // Get data from the sensor
  bme.takeForcedMeasurement();

  t = bme.readTemperature();
  h = bme.readHumidity();
  p = bme.readPressure() / 100.0F;

  Serial.println();
  if (!isnan(t) && !isnan(h) && !isnan(p)) {

    // Print sensor data to the console
    Serial.println("Temperature\tHumidity\t\tPressure");
    Serial.print(t); Serial.print(" *C\t\t");
    Serial.print(h); Serial.print(" %\t\t");
    Serial.print(p); Serial.println(" hPa");

    // Send POST request to sensor data collecting webservice
    HTTPClient http;
        delay(500);
    delay(500);


    Serial.println();
    Serial.print("Posting sensor data to collecting webservice ...");

    String payload = "t=" + String(t) + "&h=" + String(h) + "&p=" + String(p) + "&id=" + WiFi.macAddress();

    http.begin(webservice);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(payload);
                 delay(500);


    if (httpCode == HTTP_CODE_OK) {
      Serial.println(" done");
          delay(500);

    } else {
      Serial.println(" failed");
    }
    http.end();

  } else {
    Serial.println("ERROR: Received no sensor data from BME280");
  }

  // Go to deep sleep
  Serial.println();

  Serial.print("Going to deep sleep for ");
  

  Serial.print(sleepTimeInSeconds);
 
  Serial.println(" seconds. Good night!");


  ESP.deepSleep(sleepTimeInSeconds * 10 * 1000);

}

// Run over and over again
void loop () {
}
