#include <EEPROM.h>
#include <CapacitiveSensor.h>
#include <Wire.h> 
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>
#define INTERVAL 300

// Define addresses in EEPROM to store the values
const int A = 0;        // Address for stepsA
const int B = A + sizeof(int);  // Address for stepsB


// double speeds[] = {8.0, 8.42, 8.86, 9.41, 10.0, 10.66, 11.42, 12.32, 13.31, 14.56, 16.0, 17.76, 20.0, 22.88, 26.65, 32.0, 40.0, 53.34, 80.0, 159.99,225};
double speeds[] = {10.2, 10.7, 11.3, 12.2 , 13.5, 14, 14.5, 15.12, 16 , 18.3, 19, 20.96, 23 , 25.88, 30, 36.0, 43.0, 60, 90.0, 160 ,225};

long start;
long refValue1 = 0 ,refValue2 = 0;

long touchRead(CapacitiveSensor &touch,long ref){
  long start = millis();
  long val = 0;
  for(int i=0;i<10;i++){
    val+=touch.capacitiveSensorRaw(30) - ref;
  }
  delay(100-(millis()-start));
  return val/10;
}

int numSpeeds = 21;

const char* times[] = {
  "10m00s", "9m30s", "9m00s", "8m30s", "8m00s", "7m30s", "7m00s",
  "6m30s", "6m00s", "5m30s", "5m00s", "4m30s", "4m00s", 
  "3m30s", "3m00s", "2m30s", "2m00s", "1m30s", "1m00s", "0m30s","0m20s"
};

// I2C LCD address -  0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//number of steps per revolution
const int stepsPerRevolution = 200;

int threshold = 700;

// 1ml==5cm
// 5000steps == 10ml;
double MAX_VOL = 10.1;

CapacitiveSensor touch1 = CapacitiveSensor(6,7);
CapacitiveSensor touch2 = CapacitiveSensor(2,3);
// CapacitiveSensor touch3 = CapacitiveSensor(7,9);

// Initialize the stepper motor object
Stepper StepperA(stepsPerRevolution, 4 , 5);
Stepper StepperB(stepsPerRevolution, 8, 9);
// Stepper StepperC(stepsPerRevolution, 10, 11);

//pin connections
const int speed1 = A2;
const int volume1 = A3;
const int speed2 = A0;
const int volume2 = A1;
// const int speed3 = A4;
// const int volume3 = A5;

// steps used
int stepsA = 0;
int stepsB = 0;
// int stepsC = 0;

void Stepper1() {
  long startTime = millis();
  long sensorValue1 = touchRead(touch1,refValue1);
  int steps = Steps(Vol(analogRead(volume1)));

  // display
  int motorSpeed = Speed(int(analogRead(speed1)));
  int volume = Vol(analogRead(volume1));
  lcd.setCursor(5,0);

  lcd.print(times[motorSpeed]);
  lcd.setCursor(4,1);
  lcd.print("A");
  if (volume < 10) {
    lcd.print(" ");
  }
  lcd.print(volume);
  lcd.print("/");
  lcd.print((stepsA*10)/5000);

  // switch values 
  // 0 - forced on and 1 - with capacitive sensors
  bool flagA = digitalRead(10);

  if(stepsA>100 && (steps-stepsA<100) ){lcd.setCursor(4,1);lcd.print("A!");}
  // with capacitive sensor on
  if (flagA){
  // If squirrel and the volume is set more than previous volume used
    if(sensorValue1>threshold && steps-stepsA>100) {
      for(;stepsA<steps;stepsA+=3){
        long st = millis();
        steps = Steps(Vol(analogRead(volume1)));
        int motorSpeed = speeds[Speed(int(analogRead(speed1)))];
        StepperA.setSpeed(motorSpeed);
        StepperA.step(3); 

        // check for the timer and sensor
        long time = millis();
        if(time - startTime  > INTERVAL)return;
      }
      // StepperA.step(stepsA-steps);
      // stepsA=steps; 
    }
  }
  // forced on state without sensor
  else if(!flagA){
    for(;stepsA<steps;stepsA+=3){
      long st = millis();
      steps = Steps(Vol(analogRead(volume1)));
      int motorSpeed = speeds[Speed(int(analogRead(speed1)))];
      StepperA.setSpeed(motorSpeed);
      StepperA.step(3); 

      // check for the timer and sensor
      long time = millis();
      if(time - startTime  > INTERVAL)return;
    }
  }

  // condition for reseting  
  if(stepsA-steps>100){
    for(;steps-stepsA<10;stepsA-=10){
      steps = Steps(Vol(analogRead(volume1)));
      int motorSpeed = speeds[Speed(int(analogRead(speed1)))];
      StepperA.setSpeed(motorSpeed);
      StepperA.step(-10); 
      Serial.println(stepsA);

      // check for timer
      long time = millis();
      if(time - startTime  > INTERVAL)return;
    }
    StepperA.step(steps-stepsA);
    stepsA = steps;
  }

  // delay block for maintaining equal interval for blocking state 
  else{
    // check for timer
    long time = millis();
    delay(INTERVAL-(time-startTime));
  }
}


void Stepper2() {
  long startTime = millis();
  long sensorValue2 = touchRead(touch2,refValue2);
  int steps = Steps(Vol(analogRead(volume2)));

  // display
  int motorSpeed = Speed(int(analogRead(speed2)));
  int volume = Vol(analogRead(volume2));
  lcd.setCursor(11,0);
  lcd.print(times[motorSpeed]);
  lcd.setCursor(10,1);
  lcd.print("B");
  if (volume < 10) {
    lcd.print("  ");
  }
  lcd.print(volume);
  lcd.print("/");
  lcd.print((stepsB*10)/5000);

  // switch values 
  // 0 - forced on and 1 - with capacitive sensors
  bool flagB = digitalRead(11);

  if(stepsB > 100 && (steps-stepsB<100) ){lcd.setCursor(10,1);lcd.print("B!");}
  // If squirrel and the volume is set more than previous volume used
  if (flagB){
    if(sensorValue2>threshold && steps-stepsB>100) {
      long st = millis();
      for(;stepsB<steps;stepsB+=3){
        steps = Steps(Vol(analogRead(volume2)));
        int motorSpeed = speeds[Speed(int(analogRead(speed2)))];
        StepperB.setSpeed(motorSpeed);
        StepperB.step(3); 
        Serial.println(stepsB);

        // check for the timer and sensor
        long time = millis();
        if(time - startTime  > INTERVAL){return;}
      }
      // StepperB.step(stepsB-steps);
      // stepsB=steps; 
    }
  }
  // forced state
  else if(!flagB){
    long st = millis();
    for(;stepsB<steps;stepsB+=3){
      steps = Steps(Vol(analogRead(volume2)));
      int motorSpeed = speeds[Speed(int(analogRead(speed2)))];
      StepperB.setSpeed(motorSpeed);
      StepperB.step(3); 
      Serial.println(stepsB);

      // check for the timer and sensor
      long time = millis();
      if(time - startTime  > INTERVAL){return;}
    }
    // StepperB.step(stepsB-steps);
    // stepsB=steps; 
  }

  // condition for reseting  
  if(stepsB-steps>100){
    for(;steps-stepsB<10;stepsB-=10){
      steps = Steps(Vol(analogRead(volume2)));
      int motorSpeed = speeds[Speed(int(analogRead(speed2)))];
      StepperB.setSpeed(motorSpeed);
      StepperB.step(-10); 
      Serial.println(stepsB);

      // check for timer
      long time = millis();
      if(time - startTime > INTERVAL)return;
    }
    StepperB.step(steps-stepsB);
    stepsB = steps;
  }
  // delay block for maintaining equal interval for blocking state 
  else{
    // check for timer
    long time = millis();
    delay(INTERVAL-(time-startTime));
  }
}

// to map potentiometer value to speed(in form of time interval)
int Speed(int potValue) {
  int index = map(potValue, 0, 1020, 0, numSpeeds-1);
  return index;
}


// map for pot value to volume in ml
double Vol(int v)
{
  double x = MAX_VOL*v/1024;
  return x;
}

// number of steps required
int Steps(int v)
{
    int result = stepsPerRevolution * 2.5 * v;
    // Round to the nearest multiple of 10
    int x = (int)((result + 5) / 10) * 10;
    return x;
}

void setup() {
  start = millis();
  //Read the stored states from EEPROM
  EEPROM.get(A, stepsA);
  EEPROM.get(B, stepsB);
  // Initialize the values if they are not within an expected range (example range check)
  if (stepsA < 0 || stepsA > 5000) {  // Adjust the range as needed
    stepsA = 0;
    EEPROM.put(A, stepsA);
  }
  if (stepsB < 0 || stepsB > 6000) {  // Adjust the range as needed
    stepsB = 0;
    EEPROM.put(B, stepsB);
  }
  Serial.begin(9600);
  // setting up capacitive sensor and turning off auto calibration
  touch1.set_CS_AutocaL_Millis(0xFFFFFFFF);
  touch2.set_CS_AutocaL_Millis(0xFFFFFFFF); 
  touch1.capacitiveSensor(1);
  touch2.capacitiveSensor(1);

  // setting up lcd display
  lcd.begin();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Please Do Setup");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPD");
  lcd.setCursor(0, 1);
  lcd.print("Vol");
}

void loop() {
  // setting up value for the capacitive sensor
  if(millis()-start>5000 && millis()-start<6000){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Setting Up");
    delay(1000);
    refValue1 = touchRead(touch1,refValue1);
    refValue2 = touchRead(touch2,refValue2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SPD");
    lcd.setCursor(0, 1);
    lcd.print("Vol");
  }
  // active state for stepper motor
  else if(millis()-start>6000){
    Stepper1();
    Stepper2();
  }

  // Write the new states to EEPROM
  EEPROM.put(A, stepsA);
  EEPROM.put(B, stepsB);
}