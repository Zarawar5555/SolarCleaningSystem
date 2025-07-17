#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Servo.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
Adafruit_INA219 ina219;
IRrecv irrecv(8);  
decode_results results;
Servo sweepServo;
const int pumpPin = 9;
const int servoPin = 10; 
const float currentThreshold = 0.10;
bool cleaningTriggered = false;
void setup() {
  Serial.begin(9600);
  ina219.begin();
  lcd.begin(16, 2);
  irrecv.enableIRIn();
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  sweepServo.attach(servoPin);
  sweepServo.write(90);
}
void loop() {
  float current = ina219.getCurrent_mA() / 1000.0; 
  float voltage = ina219.getBusVoltage_V();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 2);
  lcd.print(" I:");
  lcd.print(current * 1000, 0);
  lcd.print("mA   ");
  if (irrecv.decode(&results)) {
    cleaningTriggered = true;
    irrecv.resume();
  }
  if (cleaningTriggered && current < currentThreshold) {
    performCleaning();
    cleaningTriggered = false;
  }
  delay(500);
}

void performCleaning() {
  digitalWrite(pumpPin, HIGH);
  delay(700);
  digitalWrite(pumpPin, LOW);

  for (int i = 90; i <= 170; i += 2) {
    sweepServo.write(i);
    delay(20);
  }

  for (int i = 170; i >= 10; i -= 2) {
    sweepServo.write(i);
    delay(20);
  }

  for (int i = 10; i <= 90; i += 2) {
    sweepServo.write(i);
    delay(20);
  }

  digitalWrite(pumpPin, HIGH);
  delay(700);
  digitalWrite(pumpPin, LOW);
}
