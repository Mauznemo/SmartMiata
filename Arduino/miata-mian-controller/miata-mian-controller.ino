#include <FastLED.h>
#include <Encoder.h>

const int powerCheckPin = 3;
const int powerRelayPin = 4;

const int reverseCheckPin = 5;

const int aButton1Pin = 6;
const int aButton2Pin = 7;

const int leftDoorPin = 8;
const int rightDoorPin = 9;
const int trunkPin = 11;

const int ledDataPin = 12;

const int lightRelayPin = 10;

Encoder rotaryEncoder(14, 15);
long oldRotaryPosition = -999;
const int rotaryButtonPin = 16;

bool lastSystemPower = true;

const int ledCount = 56;
CRGB leds[ledCount];

const int batterySensorPin = A0;
unsigned long lastDateSentTime = 0;
const unsigned long sendDateInterval = 7000;

bool lastAButton1Val;
bool lastAButton2Val;
bool lastReverseVal;
bool lastLeftDoorVal;
bool lastRightDoorVal;
bool lastTrunkVal;
bool lastRotaryButtonVal;

void setup() {
  FastLED.addLeds<NEOPIXEL, ledDataPin>(leds, ledCount);

  pinMode(powerCheckPin, INPUT);
  pinMode(reverseCheckPin, INPUT);
  pinMode(aButton1Pin, INPUT);
  pinMode(aButton2Pin, INPUT);
  pinMode(leftDoorPin, INPUT);
  pinMode(rightDoorPin, INPUT);
  pinMode(trunkPin, INPUT);
  pinMode(powerRelayPin, OUTPUT);
  pinMode(lightRelayPin, OUTPUT);
  pinMode(rotaryButtonPin, INPUT);

  digitalWrite(powerRelayPin, HIGH);
  digitalWrite(lightRelayPin, LOW);
  digitalWrite(reverseCheckPin, HIGH);
  digitalWrite(aButton1Pin, HIGH);
  digitalWrite(aButton2Pin, HIGH);
  digitalWrite(leftDoorPin, HIGH);
  digitalWrite(rightDoorPin, HIGH);
  digitalWrite(trunkPin, HIGH);
  digitalWrite(rotaryButtonPin, HIGH);
  Serial.begin(9600);
}

void loop() {

  checkReverse();
  checkActionButton1();
  checkActionButton2();
  checkLeftDoor();
  checkRightDoor();
  checkTrunk();
  checkPower();
  checkBattery();
  checkRotary();
  checkRotaryButton();


  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    if (data == "sd") {
      sendData();
    } else if (data == "off") {
      powerOff();
    } else if (data == "le") {
      setLight(true);
    } else if (data == "ld") {
      setLight(false);
    } else if (data.startsWith("ll_")) {
      String hexColor = data.substring(4);
      setLeftLedsColor(hexColor);
    } else if (data.startsWith("rl_")) {
      String hexColor = data.substring(4);
      setRightLedsColor(hexColor);
    }
  }
}

void setRightLedsColor(String hexColor) {
  // Convert hex string to CRGB
  CRGB color = hexToCRGB(hexColor);

  // Set the first half of the LEDs to the color
  for (int i = 0; i < ledCount / 2; i++) {
    leds[i] = color;
  }

  FastLED.show();
}

void setLeftLedsColor(String hexColor) {
  // Convert hex string to CRGB
  CRGB color = hexToCRGB(hexColor);

  //Serial.println(hexColor);

  // Set the second half of the LEDs to the color
  for (int i = ledCount / 2; i < ledCount; i++) {
    leds[i] = color;
  }

  FastLED.show();
}

CRGB hexToCRGB(String hexColor) {
  long number = strtol(&hexColor[0], NULL, 16);
  byte red = (number >> 16) & 0xFF;
  byte green = (number >> 8) & 0xFF;
  byte blue = number & 0xFF;
  return CRGB(red, green, blue);
}


void setLight(bool active) {
  digitalWrite(lightRelayPin, active);
}

void powerOff() {
  delay(30000);

  digitalWrite(powerRelayPin, LOW);
}

void sendData() {
  onLeftDoorChanged(lastLeftDoorVal);
  onRightDoorChanged(lastRightDoorVal);
  onTrunkChanged(lastTrunkVal);
  onReverseChanged(lastReverseVal);

  if (!digitalRead(powerCheckPin)) {
    Serial.println("off");
  }
}

void checkBattery() {
  unsigned long currentTime = millis();

  if (currentTime - lastDateSentTime >= sendDateInterval) {
    int sensorValue = analogRead(batterySensorPin);
    float voltage = sensorValue * (5.0 / 1023.0) * 3.0;
    Serial.print("v_");
    Serial.println(voltage);
    lastDateSentTime = currentTime;
    return;
  }
}

void checkRotary() {
  long newPosition = rotaryEncoder.read();

  if (newPosition > oldRotaryPosition && (newPosition - oldRotaryPosition) >= 2) {
    Serial.println("cwd");
    oldRotaryPosition = newPosition;
  } else if (newPosition < oldRotaryPosition && (oldRotaryPosition - newPosition) >= 2) {
    Serial.println("cwu");
    oldRotaryPosition = newPosition;
  }
}

void checkRotaryButton() {
  bool buttonVal = digitalRead(rotaryButtonPin);

  if (lastRotaryButtonVal != buttonVal) {
    lastRotaryButtonVal = buttonVal;
    onRotaryButtonChanged(buttonVal);
  }
}

void checkReverse() {
  bool reverseVal = digitalRead(reverseCheckPin);

  if (lastReverseVal != reverseVal) {
    lastReverseVal = reverseVal;
    onReverseChanged(reverseVal);
  }
}

void checkActionButton1() {
  bool aButton1Val = digitalRead(aButton1Pin);

  if (lastAButton1Val != aButton1Val) {
    lastAButton1Val = aButton1Val;
    onActionButton1Changed(aButton1Val);
  }
}

void checkActionButton2() {
  bool aButton2Val = digitalRead(aButton2Pin);

  if (lastAButton2Val != aButton2Val) {
    lastAButton2Val = aButton2Val;
    onActionButton2Changed(aButton2Val);
  }
}

void checkLeftDoor() {
  bool leftDoorVal = digitalRead(leftDoorPin);

  if (lastLeftDoorVal != leftDoorVal) {
    lastLeftDoorVal = leftDoorVal;
    onLeftDoorChanged(leftDoorVal);
  }
}

void checkRightDoor() {
  bool rightDoorVal = digitalRead(rightDoorPin);

  if (lastRightDoorVal != rightDoorVal) {
    lastRightDoorVal = rightDoorVal;
    onRightDoorChanged(rightDoorVal);
  }
}

void checkTrunk() {
  bool trunkVal = digitalRead(trunkPin);

  if (lastTrunkVal != trunkVal) {
    lastTrunkVal = trunkVal;
    onTrunkChanged(trunkVal);
  }
}

void checkPower() {
  bool powerVal = digitalRead(powerCheckPin);

  if (lastSystemPower != powerVal) {
    lastSystemPower = powerVal;
    onPowerChanged(powerVal);
  }
}

void onPowerChanged(bool val) {
  if (val) {
    Serial.println("p");
  } else {
    Serial.println("po");
  }
}

void onReverseChanged(bool val) {
  if (val) {
    Serial.println("re");
  } else {
    Serial.println("rd");
  }
}

void onRotaryButtonChanged(bool val) {
  if (val) {
    Serial.println("cwbu");
  } else {
    Serial.println("cwbd");
  }
}


void onActionButton1Changed(bool val) {
  if (val) {
    Serial.println("a1u");
  } else {
    Serial.println("a1d");
  }
}

void onActionButton2Changed(bool val) {
  if (val) {
    Serial.println("a2u");
  } else {
    Serial.println("a2d");
  }
}

void onLeftDoorChanged(bool val) {
  if (val) {
    Serial.println("ldc");
  } else {
    Serial.println("ldo");
  }
}

void onRightDoorChanged(bool val) {
  if (val) {
    Serial.println("rdc");
  } else {
    Serial.println("rdo");
  }
}

void onTrunkChanged(bool val) {
  if (val) {
    Serial.println("to");
  } else {
    Serial.println("tc");
  }
}
