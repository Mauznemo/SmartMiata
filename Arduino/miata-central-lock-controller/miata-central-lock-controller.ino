const int trunkUnlockPin = 2;
const int doorLockPin = 3;
const int doorUnlockPin = 4;

const int doorsRelayPin1 = 5;
const int doorsRelayPin2 = 6;
const int trunkRelayPin1 = 7;
const int wakePin = 8;

const int motorTimeMs = 50;
const int blockTimeMs = 1500;

void setup() {
  pinMode(trunkUnlockPin, INPUT);
  pinMode(doorUnlockPin, INPUT);
  pinMode(doorLockPin, INPUT);

  digitalWrite(trunkUnlockPin, HIGH);
  digitalWrite(doorUnlockPin, HIGH);
  digitalWrite(doorLockPin, HIGH);

  pinMode(doorsRelayPin1, OUTPUT);
  pinMode(doorsRelayPin2, OUTPUT);
  pinMode(trunkRelayPin1, OUTPUT);

  pinMode(wakePin, OUTPUT);

  //digitalWrite(doorsRelayPin1, HIGH);
  //digitalWrite(doorsRelayPin2, HIGH);
  //digitalWrite(trunkRelayPin1, HIGH);

  Serial.begin(9600);
}

void loop() {
  //checkTrunkUnlockPin();
  //checkDoorUnlockPin();
  //checkDoorLockPin();
  checkSerial();
}

void unlockTrunk(){
  digitalWrite(trunkRelayPin1, HIGH);

  delay(motorTimeMs);

  digitalWrite(trunkRelayPin1, LOW);

  Serial.println("Unlocked Trunk");

  delay(blockTimeMs);
}

void unlockDoors(){
  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, HIGH);

  delay(motorTimeMs);

  digitalWrite(doorsRelayPin1, HIGH);
  digitalWrite(doorsRelayPin2, HIGH);

  Serial.println("Unlocked doors");

  delay(blockTimeMs);
}

void lockDoors(){
  digitalWrite(doorsRelayPin1, HIGH);
  digitalWrite(doorsRelayPin2, LOW);

  delay(motorTimeMs);

  digitalWrite(doorsRelayPin1, HIGH);
  digitalWrite(doorsRelayPin2, HIGH);

  Serial.println("Locked doors");

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
