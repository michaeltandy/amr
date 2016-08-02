#include <limits.h>

#define ANALOG_REF        A0
#define ANALOG_BRIDGE_A   A1
#define ANALOG_BRIDGE_B   A2

#define LED1              A3
#define LED2              A4
#define LED3              A5

long aTotal = 0;
long bTotal = 0;
int aMax = INT_MIN;
int bMax = INT_MIN;
int aMin = INT_MAX;
int bMin = INT_MAX;
long microsMin = 0;
long measurementCount = 0;

bool serialOutEveryReading = false;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  Serial.begin(57600);
  Serial.println("started");
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

void loop() {
  
  int refInt = analogRead(ANALOG_REF);
  int aInt = analogRead(ANALOG_BRIDGE_A);
  int bInt = analogRead(ANALOG_BRIDGE_B);
  
  measurementCount++;
  aTotal += aInt;
  bTotal += bInt;
  aMax = max(aMax,aInt);
  bMax = max(bMax,bInt);
  aMin = min(aMin,aInt);
  bMin = min(bMin,bInt);
  
  handleSerial();
    
  if (serialOutEveryReading) {
    float reference_voltage = 5.0*refInt/1.024;
    float bridge_a_voltage = 5.0*aInt/1.024;
    float bridge_b_voltage = 5.0*bInt/1.024;
    
    Serial.print(micros());
    Serial.print("\t");
    Serial.print(reference_voltage);
    Serial.print("\t");
    Serial.print(bridge_a_voltage);
    Serial.print("\t");
    Serial.print(bridge_b_voltage);
    Serial.println();
    delay(100);
  }
  
  bool aInRange = (566 <= aInt && aInt <= 746);
  bool bLow = (bInt < 487);
  bool bInRange = (487 <= bInt && bInt <= 512);
  bool bHigh = (bInt > 512);
  
  if (!aInRange) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  } else {
    if (bLow) {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
    } else if (bInRange) {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, LOW);
    } else if (bHigh) {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, HIGH);
    } else {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, HIGH);
    }
  }
  
}

void handleSerial() {
  while (Serial.available()) {
    char c = toupper(Serial.read());
    if ( c == 'A' ) {
      startTracking();
    } else if ( c == 'B' ) {
      endTracking();
    } else if ( c == 'C' ){
      serialOutEveryReading = true;
    } else if ( c == 'D' ){
      serialOutEveryReading = false;
    }
  }
}

void startTracking() {
  aTotal = 0;
  bTotal = 0;
  aMax = INT_MIN;
  bMax = INT_MIN;
  aMin = INT_MAX;
  bMin = INT_MAX;
  measurementCount = 0;
  microsMin = micros();
}

void endTracking() {
  long duration = micros()-microsMin;
  Serial.print("A: ");
  printValueAndVoltage(aMin);
  Serial.print(" to ");
  printValueAndVoltage(aMax);
  Serial.print(" average ");
  printAverage(aTotal,measurementCount);
  Serial.println();
  
  Serial.print("B: ");
  printValueAndVoltage(bMin);
  Serial.print(" to ");
  printValueAndVoltage(bMax);
  Serial.print(" average ");
  printAverage(bTotal,measurementCount);
  Serial.println();
  
  Serial.print("Time: ");
  Serial.print(duration);
  Serial.print(" micros; measurements: ");
  Serial.print(measurementCount);
  Serial.print("; micros per measurement: ");
  Serial.print(duration/measurementCount);
  Serial.println();
}

void printValueAndVoltage(int val) {
  float volt = 5.0*val/1.024;
  Serial.print(val);
  Serial.print(" (");
  Serial.print(volt);
  Serial.print(") ");
}

void printAverage(long sum, long count) {
  float val = (sum * 1.0) / (count * 1.0);
  float volt = 5.0*val/1.024;
  Serial.print(val);
  Serial.print(" (");
  Serial.print(volt);
  Serial.print(") ");
}
