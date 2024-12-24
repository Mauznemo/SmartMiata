#include <SoftwareSerial.h>

#define RX_PIN 10
#define TX_PIN 11

SoftwareSerial bleSerial(RX_PIN, TX_PIN);

String receivedData = "";
bool autoLocking;
bool doorsLocked = true;

const int trunkUnlockPin = 2;
const int doorLockPin = 3;
const int doorUnlockPin = 4;

const int doorsRelayPin1 = 5;
const int doorsRelayPin2 = 6;
const int trunkRelayPin1 = 7;
const int wakePin = 8;

const int motorTimeMs = 50;
const int blockTimeMs = 1500;

void setup()
{
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

  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, LOW);
  digitalWrite(trunkRelayPin1, LOW);

  Serial.begin(9600);

  bleSerial.begin(9600);

  // Give some time for the HM-10 to initialize
  delay(1000);

  bleSerial.print("AT+NOTI1");

  delay(500);
}

void loop()
{
  checkTrunkUnlockPin();
  checkDoorUnlockPin();
  checkDoorLockPin();
  checkSerial();
  checkBle();
}

void checkBle()
{
  if (bleSerial.available())
  {
    // Read the data from the BLE module
    char incomingChar = bleSerial.read();
    if (isPrintable(incomingChar))
    {
      receivedData += incomingChar;
    }

    // Serial.println(incomingChar);

    if (receivedData.indexOf("OK+CONN") != -1)
    {
      receivedData = "";
    }
    else if (receivedData.indexOf("OK+LOST") != -1)
    {
      if (autoLocking)
      {
        // Lock the car
        lockDoors();
      }
      autoLocking = false;
      receivedData = "";
    }

    // If the data ends with a newline character, process it
    if (incomingChar == '\n')
    {
      receivedData.trim();
      if (receivedData == "ds")
      {
        bleSerial.println(doorsLocked ? "ld" : "ud");
      }
      else if (receivedData == "ld")
      {
        bleSerial.println("ld");
        lockDoors();
      }
      else if (receivedData == "ud")
      {
        bleSerial.println("ud");
        unlockDoors();
      }
      else if (receivedData == "ut")
      {
        unlockTrunk();
      }
      else if (receivedData == "al")
      {
        autoLocking = true;
        if (doorsLocked)
        {
          unlockDoors();
        }
      }
      else if (receivedData == "ald")
      {
        autoLocking = false;
      }

      receivedData = "";
    }
  }
}

void unlockTrunk()
{
  digitalWrite(trunkRelayPin1, HIGH);

  delay(motorTimeMs);

  digitalWrite(trunkRelayPin1, LOW);

  Serial.println("ut");

  delay(blockTimeMs);
}

void unlockDoors()
{
  digitalWrite(doorsRelayPin1, HIGH);
  digitalWrite(doorsRelayPin2, LOW);

  delay(motorTimeMs);

  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, LOW);

  Serial.println("ud");
  doorsLocked = false;
  // wakeUpSystem();

  delay(blockTimeMs);
}

void wakeUpSystem()
{
  digitalWrite(wakePin, HIGH);

  delay(1000);

  digitalWrite(wakePin, LOW);
}

void lockDoors()
{
  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, HIGH);

  delay(motorTimeMs);

  digitalWrite(doorsRelayPin1, LOW);
  digitalWrite(doorsRelayPin2, LOW);

  Serial.println("ld");
  doorsLocked = true;

  delay(blockTimeMs);
}

void checkSerial()
{
  if (Serial.available() > 0)
  {
    String data = Serial.readStringUntil('\n');

    if (data == "ld")
    {
      lockDoors();
    }
    else if (data == "ud")
    {
      unlockDoors();
    }
    else if (data == "ut")
    {
      unlockTrunk();
    }
  }
}

bool lastTrunkUnlockVal;
bool lastDoorUnlockVal;
bool lastDoorLockVal;

void checkTrunkUnlockPin()
{
  bool trunkUnlockVal = digitalRead(trunkUnlockPin);

  if (lastTrunkUnlockVal != trunkUnlockVal)
  {
    lastTrunkUnlockVal = trunkUnlockVal;
    onTrunkUnlockChanged(trunkUnlockVal);
  }
}

void checkDoorUnlockPin()
{
  bool doorUnlockVal = digitalRead(doorUnlockPin);

  if (lastDoorUnlockVal != doorUnlockVal)
  {
    lastDoorUnlockVal = doorUnlockVal;
    onDoorUnlockChanged(doorUnlockVal);
  }
}

void checkDoorLockPin()
{
  bool doorLockVal = digitalRead(doorLockPin);

  if (lastDoorLockVal != doorLockVal)
  {
    lastDoorLockVal = doorLockVal;
    onDoorLockChanged(doorLockVal);
  }
}

void onTrunkUnlockChanged(bool val)
{
  if (!val)
  {
    unlockTrunk();
  }
}

void onDoorUnlockChanged(bool val)
{
  if (val)
  {
    unlockDoors();
  }
}

void onDoorLockChanged(bool val)
{
  if (val)
  {
    lockDoors();
  }
}
