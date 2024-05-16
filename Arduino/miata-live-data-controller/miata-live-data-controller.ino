#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <MPU6050.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

StaticJsonDocument<200> doc;

TinyGPSPlus gps;
MPU6050 mpu;
SoftwareSerial ss(RXPin, TXPin);

unsigned long lastSentTime = 0;
unsigned long prevTime = 0;
const unsigned long sendInterval = 7000;
float speedKmH = -1;
int rpm;
//float roll = 0, pitch = 0, yaw = 0;

bool sendAllData;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);

  Wire.begin();
  
  mpu.initialize();
  
  // Calibrate the sensor
  //mpu.CalibrateGyro();
  //mpu.CalibrateAccel();
}

void loop() {
  unsigned long currentTime = millis();

 if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    if (data == "sa") {
      sendAllData = true;
    } 
    else if (data == "ss") {
      sendAllData = false;
    } 
  }

  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isValid()) {
      // Print speed
      float speedKnots = gps.speed.knots();
      // Convert speed to km/h
      speedKmH = speedKnots * 1.852;  // 1 knot = 1.852 km/h

      // Check if it's time to send data
      if (currentTime - lastSentTime >= sendInterval) {
        sendTimeAndDate();
        lastSentTime = currentTime;
        return;
      }
      //break;
    }
  } 

  if(!sendAllData){
    Serial.print(rpm);
    Serial.print("_");
    Serial.println(speedKmH);
    return;
  }

  //float dt = (currentTime - prevTime) / 1000.0; // Convert milliseconds to seconds
  //prevTime = currentTime; // Update previous time

   // Read raw accelerometer and gyro data
  int16_t ax, ay, az;

  mpu.getAcceleration(&ax, &ay, &az);
  
  // Convert raw data to degrees per second for gyro
  //float gyroX = gx / 131.0;
  //float gyroY = gy / 131.0;
  //float gyroZ = gz / 131.0;
  
  // Convert raw data to g's for accelerometer
  float accelX = ax / 16384.0;
  float accelY = ay / 16384.0;
  float accelZ = az / 16384.0;

  //roll += gyroX * dt;
  //pitch += gyroY * dt;
 // yaw += gyroZ * dt;
  
   // Create a JSON document
  doc["s"] = round(speedKmH * 100.0) / 100.0;
  doc["r"] = rpm;
  //doc["g"]["x"] = round(roll * 100.0) / 100.0;
  //doc["g"]["y"] = round(pitch * 100.0) / 100.0;
  //doc["g"]["z"] = round(yaw * 100.0) / 100.0;
  doc["x"] = round(accelX * 100.0) / 100.0;
  doc["y"] = round(accelY * 100.0) / 100.0;
  doc["z"] = round(accelZ * 100.0) / 100.0;
  
  // Serialize JSON to a string
  String output;
  serializeJson(doc, output);
  
  // Print the JSON string
  Serial.println(output);
  
  // Clear the JSON document for the next iteration
  doc.clear();
}


void sendTimeAndDate() {
  if (!gps.time.isValid())
    return;

  StaticJsonDocument<100> jsonDoc;

  // Add time to JSON
  JsonObject timeObject = jsonDoc.createNestedObject("time");
  timeObject["h"] = gps.time.hour();
  timeObject["m"] = gps.time.minute();
  timeObject["s"] = gps.time.second();

  // Add date to JSON
  JsonObject dateObject = jsonDoc.createNestedObject("date");
  dateObject["d"] = gps.date.day();
  dateObject["m"] = gps.date.month();
  dateObject["y"] = gps.date.year();

  // Serialize JSON to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send JSON string over Serial
  Serial.println(jsonString);
}
