const int sensorPin = 2; // Pin connected to the RPM sensor
volatile unsigned long pulseCount = 0; // Variable to store the pulse count
float rpm = 0; // Variable to store the calculated RPM
unsigned long previousMillis = 0; // Variable to store the previous time

void setup() {
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING); // Attach interrupt to the rising edge of the pulse
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Calculate RPM every second
  if (currentMillis - previousMillis >= 200) {
    //detachInterrupt(digitalPinToInterrupt(sensorPin)); // Disable interrupts while calculating RPM
    //6p = 0.33k
    //inp = rpmk
    //(pulseCount * 0.33)/5
    Serial.print("Pulse Count: ");
    Serial.print(pulseCount);

    rpm = (float)pulseCount/7.00; // Calculate RPM
    pulseCount = 0; // Reset pulse count
    //attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING); // Re-attach interrupt
    Serial.print(" RPM: ");
    Serial.print(rpm);
    //int rpmK = int(rpm * 0.0001);
    Serial.println("k");
    previousMillis = currentMillis;
  }
}

void countPulse() {
  pulseCount++;
}
