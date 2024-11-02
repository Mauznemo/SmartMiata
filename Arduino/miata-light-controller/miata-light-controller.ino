#include <EEPROM.h>

#define buttonPin 3 // analog input pin to use as a digital input
#define drlCheckPin 2
#define lightCheckPin 4
#define leftup 11   // digital output pin for left headlight up
#define rightup 6   // digital output pin for right headlight up
#define leftdown 12 // digital output pin for left headlight down
#define rightdown 7 // digital output pin for right headlight down

#define leftStateAddr 0
#define rightStateAddr 1
#define valAddr 2

const unsigned long DEBOUNCE_DELAY = 100;  // Debounce time in milliseconds

// power variable
bool ledVal = false; // state of headlight power

bool waving;

bool drlOn;
bool lightOn;

bool lastDrlVal;
bool lastLightVal;
bool lastButtonVal;
bool lastStableButtonState = false;
unsigned long lastDebounceTime = 0;

bool allowModifyingLightsWhileOn = false;

void setup()
{
  Serial.begin(9600);
  // Set button input pin
  pinMode(buttonPin, INPUT);
  pinMode(drlCheckPin, INPUT);
  pinMode(lightCheckPin, INPUT);
  digitalWrite(buttonPin, HIGH);

  lastButtonVal = digitalRead(buttonPin);

  pinMode(leftup, OUTPUT);
  digitalWrite(leftup, ledVal);

  pinMode(rightup, OUTPUT);
  digitalWrite(rightup, ledVal);

  pinMode(leftdown, OUTPUT);
  digitalWrite(leftdown, ledVal);

  pinMode(rightdown, OUTPUT);
  digitalWrite(rightdown, ledVal);

  ledVal = EEPROM.read(valAddr);
}

void loop()
{

  if (checkButton())
  {
    toggle();
  }

  drlOn = checkDRL();
  lightOn = checkLight();

  if (Serial.available() > 0)
  {
    String data = Serial.readStringUntil('\n');

    if (data == "wl")
    {
      winkLeft();
    }
    else if (data == "wr")
    {
      winkRight();
    }
    else if (data == "ss")
    {
      sendStates();
    }
    else if (data == "tg")
    {
      toggle();
    }
    else if (data == "sl")
    {
      toggleSleepy();
    }
    else if (data == "tw")
    {
      toggleWaveing();
    }
    else if (data == "rs")
    {
      down();
    }
    else if (data == "up")
    {
      up();
    }
    else if (data == "am")
    {
      allowModifyingLightsWhileOn = true;
    }
    else if (data == "dm")
    {
      allowModifyingLightsWhileOn = false;
    }
  }

  if (waving)
  {
    wave();
  }
}

//=================================================
// Events to trigger

void toggleWaveing()
{

  if (lightOn)
  {
    Serial.println("el");
    if (!allowModifyingLightsWhileOn)
    {
      waving = false;
      Serial.println("we");
      return;
    }
  }

  waving = !waving;
  if (waving)
  {
    Serial.println("ws");
  }
  else
  {
    Serial.println("we");
  }
}

void stopWaving()
{
  if (waving)
  {
    Serial.println("we");
  }
  waving = false;
}

void winkLeft()
{
  stopWaving();

  digitalWrite(leftup, !ledVal);
  digitalWrite(leftdown, ledVal);

  sendLeftPos(!ledVal);

  delay(750);
  digitalWrite(leftup, ledVal);
  digitalWrite(leftdown, !ledVal);

  sendLeftPos(ledVal);

  delay(750);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);
}

void winkRight()
{
  stopWaving();

  digitalWrite(rightup, !ledVal);
  digitalWrite(rightdown, ledVal);

  sendRightPos(!ledVal);

  delay(750);
  digitalWrite(rightup, ledVal);
  digitalWrite(rightdown, !ledVal);

  sendRightPos(ledVal);

  delay(750);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);
}

void toggle()
{
  stopWaving();

  if (lightOn)
  {
    Serial.println("el");
    if (!allowModifyingLightsWhileOn)
      return;
  }

  ledVal = !ledVal;
  EEPROM.write(valAddr, ledVal);

  digitalWrite(leftup, ledVal);
  digitalWrite(rightup, ledVal);
  digitalWrite(leftdown, !ledVal);
  digitalWrite(rightdown, !ledVal);

  sendLeftPos(ledVal);
  sendRightPos(ledVal);

  delay(750);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);
}

// the motors sound weird when doing this so maybe don't
void toggleSleepy()
{
  stopWaving();

  if (lightOn)
  {
    Serial.println("el");
    if (!allowModifyingLightsWhileOn)
      return;
  }

  ledVal = !ledVal;
  EEPROM.write(valAddr, ledVal);

  digitalWrite(leftup, ledVal);
  digitalWrite(rightup, ledVal);
  digitalWrite(leftdown, !ledVal);
  digitalWrite(rightdown, !ledVal);

  sendLeftSleepyPos(ledVal);
  sendRightSleepyPos(ledVal);

  delay(750 / 2);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);
}

void wave()
{
  if (lightOn)
  {
    return;
  }

  ledVal = !ledVal;
  EEPROM.write(valAddr, ledVal);

  digitalWrite(leftup, !ledVal);
  digitalWrite(rightup, ledVal);
  digitalWrite(leftdown, ledVal);
  digitalWrite(rightdown, !ledVal);

  sendLeftPos(!ledVal);
  sendRightPos(ledVal);

  delay(750);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);
}
void down()
{
  stopWaving();

  if (lightOn)
  {
    Serial.println("el");
    if (!allowModifyingLightsWhileOn)
      return;
  }

  ledVal = LOW;
  EEPROM.write(valAddr, ledVal);

  digitalWrite(leftup, ledVal);
  digitalWrite(rightup, ledVal);
  digitalWrite(leftdown, !ledVal);
  digitalWrite(rightdown, !ledVal);

  sendLeftPos(ledVal);
  sendRightPos(ledVal);

  delay(750);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);

  // ledVal = false;
}

void up()
{
  stopWaving();

  ledVal = HIGH;
  EEPROM.write(valAddr, ledVal);

  digitalWrite(leftup, ledVal);
  digitalWrite(rightup, ledVal);
  digitalWrite(leftdown, !ledVal);
  digitalWrite(rightdown, !ledVal);

  sendLeftPos(ledVal);
  sendRightPos(ledVal);

  delay(750);
  digitalWrite(leftup, LOW);
  digitalWrite(rightup, LOW);
  digitalWrite(leftdown, LOW);
  digitalWrite(rightdown, LOW);
  // ledVal = false;
}

void sendRightPos(bool rightUp)
{
  EEPROM.write(rightStateAddr, rightUp);

  if (rightUp)
  {
    Serial.println("ru");
  }
  else
  {
    Serial.println("rd");
  }
}

void sendLeftPos(bool leftUp)
{
  EEPROM.write(leftStateAddr, leftUp);

  if (leftUp)
  {
    Serial.println("lu");
  }
  else
  {
    Serial.println("ld");
  }
}

void sendStates()
{
  bool leftState = EEPROM.read(leftStateAddr);
  bool rightState = EEPROM.read(rightStateAddr);

  if (leftState)
  {
    Serial.println("lu");
  }
  else
  {
    Serial.println("ld");
  }

  if (rightState)
  {
    Serial.println("ru");
  }
  else
  {
    Serial.println("rd");
  }
}

void sendRightSleepyPos(bool rightUp)
{
  if (rightUp)
  {
    Serial.println("rs");
  }
  else
  {
    Serial.println("rd");
  }
}

void sendLeftSleepyPos(bool leftUp)
{
  if (leftUp)
  {
    Serial.println("ls");
  }
  else
  {
    Serial.println("ld");
  }
}

bool checkButton() {
  bool buttonVal = digitalRead(buttonPin);
  bool stateChanged = false;
  
  // If the button state changed
  if (buttonVal != lastButtonVal) {
    // Reset the debounce timer
    lastDebounceTime = millis();
  }
  
  // Check if enough time has passed since the last state change
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // If the button state is different from the last stable state
    if (buttonVal != lastStableButtonState) {
      lastStableButtonState = buttonVal;
      stateChanged = true;
    }
  }
  
  lastButtonVal = buttonVal;
  return stateChanged;
}

bool checkDRL()
{
  bool drlVal = digitalRead(drlCheckPin);

  if (lastDrlVal != drlVal)
  {
    lastDrlVal = drlVal;
    onDrlStateChanged(drlVal);
  }
  return drlVal;
}

bool checkLight()
{
  bool lightVal = !digitalRead(drlCheckPin); //Input is inverted

  if (lastLightVal != lightVal)
  {
    lastLightVal = lightVal;
    onLightStateChanged(lightVal);
  }
  return lightVal;
}

void onDrlStateChanged(bool val)
{
  if (val)
  {
    up();
    Serial.println("drle");
  }
  else
  {
    Serial.println("drld");
  }
}

void onLightStateChanged(bool val)
{
  if (val)
  {
    up();
    Serial.println("lie");
  }
  else
  {
    Serial.println("lid");
  }
}

