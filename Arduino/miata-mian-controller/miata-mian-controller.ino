const int powerCheckPin = 3;
const int powerRelayPin = 4;

const int reverseCheckPin = 5;

const int aButton1Pin = 6;
const int aButton2Pin = 7;

const int leftDoorPin = 8;
const int rightDoorPin = 9;
const int trunkPin = 11;

const int lightRelayPin = 10;

bool lastSystemPower = true;

void setup() {
  pinMode(powerCheckPin, INPUT);
  pinMode(reverseCheckPin, INPUT);
  pinMode(aButton1Pin, INPUT);
  pinMode(aButton2Pin, INPUT);
  pinMode(leftDoorPin, INPUT);
  pinMode(rightDoorPin, INPUT);
  pinMode(trunkPin, INPUT);
  pinMode(powerRelayPin, OUTPUT);
  pinMode(lightRelayPin, OUTPUT);

  digitalWrite(powerRelayPin, HIGH);
  digitalWrite(lightRelayPin, LOW);
  digitalWrite(reverseCheckPin, HIGH);
  digitalWrite(aButton1Pin, HIGH);
  digitalWrite(aButton2Pin, HIGH);
  digitalWrite(leftDoorPin, HIGH);
  digitalWrite(rightDoorPin, HIGH);
  digitalWrite(trunkPin, HIGH);
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
    }
  }
}


void setLight(bool active) {
  digitalWrite(lightRelayPin, active);
}

void powerOff() {
  delay(30000);

  digitalWrite(powerRelayPin, LOW);
}

bool lastAButton1Val;
bool lastAButton2Val;
bool lastReverseVal;
bool lastLeftDoorVal;
bool lastRightDoorVal;

void sendData() {
  onLeftDoorChanged(lastLeftDoorVal);
  onRightDoorChanged(lastRightDoorVal);
  onReverseChanged(lastReverseVal);

  if (!digitalRead(powerCheckPin)) {
    Serial.println("off");
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
    Serial.println("tc");
  } else {
    Serial.println("to");
  }
}
