#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

const int rpmPin = 2;

const int steerPin = A0;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

float speedKmH = -1;
float rpm = 0;
float stWheelAngle;  //-540 - 540 (1080°) 1.11° precision > 0.07° precision of wheel angle (1:15)

unsigned long lastDateSentTime = 0;
const unsigned long sendDateInterval = 7000;

volatile unsigned long pulseCount = 0;
unsigned long previousMillis = 0;

void setup() {
  pinMode(rpmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rpmPin), countPulse, RISING);

  Serial.begin(9600);
  ss.begin(GPSBaud);
}

void loop() {
  while (ss.available() > 0)
    gps.encode(ss.read());

  if (gps.speed.isUpdated()) {
    speedKmH = gps.speed.kmph();
  }

  unsigned long currentTime = millis();

  if (currentTime - lastDateSentTime >= sendDateInterval) {
    sendTimeAndDate();
    lastDateSentTime = currentTime;
    return;
  }

  if (currentTime - previousMillis >= 200) {
    //detachInterrupt(digitalPinToInterrupt(rpmPin)); // Disable interrupts while calculating RPM
    rpm = (float)pulseCount / 7.00;  // Calculate RPM
    pulseCount = 0;                  // Reset pulse count

    //attachInterrupt(digitalPinToInterrupt(rpmPin), countPulse, RISING); // Re-attach interrupt
    previousMillis = currentTime;
  }

  int potValue = analogRead(steerPin); //10 rotations: 0 - 1023, 1 rotation play: 0 - 920.7 (51.15 - 971.85)
  //float angle = (1080 / 971.85) * (potValue + 51.15); //1080 (totalAngle) / 972 * (potValue + 25 (offset))
  stWheelAngle = fmap(potValue, 51, 972, -540.0, 540.0);
  
  if (changed()) {
    Serial.print(rpm);
    Serial.print("_");
    Serial.print(speedKmH);
    Serial.print("_");
    Serial.println(stWheelAngle);
  }
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void countPulse() {
  pulseCount++;
}

void sendTimeAndDate() {
  if (!gps.time.isValid() || !gps.date.isValid())
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

float lastRpm;
float lastSpeed;
float lastStWheelAngle;

bool changed() {
  if (lastRpm != rpm || lastSpeed != speedKmH || lastStWheelAngle != stWheelAngle) {
    lastRpm = rpm;
    lastSpeed = speedKmH;
    lastStWheelAngle = stWheelAngle;

    return true;
  }

  return false;
}