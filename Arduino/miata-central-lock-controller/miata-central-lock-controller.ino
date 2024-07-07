const int trunkUnlockPin = 0;
const int doorUnlockPin = 1;
const int doorLockPin = 2;

const int doorsRelayPin1 = 3;
const int doorsRelayPin2 = 4;
const int trunkRelayPin1 = 5;

const int motorTimeMs = 500;
const int blockTimeMs = 1500;

void setup() {
  pinMode(trunkUnlockPin, INPUT);
  pinMode(doorUnlockPin, INPUT);
  pinMode(doorLockPin, INPUT);

  pinMode(doorsRelayPin1, OUTPUT);
  pinMode(doorsRelayPin2, OUTPUT);
  pinMode(trunkRelayPin1, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  checkTrunkUnlockPin();
  checkDoorUnlockPin();
  checkDoorLockPin();
  checkSerial();
}

void unlockTrunk(){
  digitalWrite(trunkRelayPin1, HIGH);

  delay(motorTimeMs);

  digitalWrite(trunkRelayPin1, LOW);

  Serial.println("ut");

  delay(blockTimeMs);
}

void unlockDoors(){
  digitalWrite(doorsRelayPin1, HIGH);
  digitalWrite(doorsRelayPin2, LOW);

  delay(motorTimeMs);

  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, LOW);

  Serial.println("ud");

  delay(blockTimeMs);
}

void lockDoors(){
  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, HIGH);

  delay(motorTimeMs);

  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, LOW);

  Serial.println("ld");

  delay(blockTimeMs);
}

void checkSerial(){
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    if (data == "ld") {
      lockDoors();
    } else if (data == "ud") {
      unlockDoors();
    } else if (data == "ut") {
      unlockTrunk();
    }
  }
}

bool lastTrunkUnlockVal;
bool lastDoorUnlockVal;
bool lastDoorLockVal;

void checkTrunkUnlockPin() {
  bool trunkUnlockVal = digitalRead(trunkUnlockPin);

  if (lastTrunkUnlockVal != trunkUnlockVal) {
    lastTrunkUnlockVal = trunkUnlockVal;
    onTrunkUnlockChanged(trunkUnlockVal);
  }
}

void checkDoorUnlockPin() {
  bool doorUnlockVal = digitalRead(doorUnlockPin);

  if (lastDoorUnlockVal != doorUnlockVal) {
    lastDoorUnlockVal = doorUnlockVal;
    onDoorUnlockChanged(doorUnlockVal);
  }
}

void checkDoorLockPin() {
  bool doorLockVal = digitalRead(doorLockPin);

  if (lastDoorLockVal != doorLockVal) {
    lastDoorLockVal = doorLockVal;
    onDoorLockChanged(doorLockVal);
  }
}


void onTrunkUnlockChanged(bool val){
  if(val){
    unlockTrunk();
  }
}

void onDoorUnlockChanged(bool val){
 if(val){
    unlockDoors();
  }
}

void onDoorLockChanged(bool val){
if(val){
    lockDoors();
  }
}
