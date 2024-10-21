#include <POP32.h>
#include <WCX.h>

WCX mywcx;

// Pin Definitions
const int RED_PIN = PC15;
const int GREEN_PIN = PC14;
const int BLUE_PIN = PA15;
const int BUZZER_PIN = PB11;

// State Variables
bool isBeeping = false;
int speed = 50;  // Speed range: 50 to 100
int servoBaseAngle = 90;
int gripperAngleOpen = 30;
int gripperAngleClose = 0;
int gripperAngle = 0;
// Function Prototypes
void initializePins();
void setColor(int red, int green, int blue);
void updateSpeed(int change);
void displaySpeed();
void handleMovement();
void handleServoActions();
void handleColorActions();

void setup() {
  mywcx.begin();
  initializePins();
  setColor(0, 0, 0);  // Turn off RGB LEDs initially
  displaySpeed();
  baseServo(0);
  gripperServo(gripperAngleOpen);
}

void loop() {
  mywcx.getButton();
  handleMovement();
  ServoControl();
  handleServoActions();
  handleColorActions();
}

void initializePins() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}


void updateSpeed(int change) {
  speed += change;
  speed = constrain(speed, 50, 100);  // Ensure speed remains within bounds
}

void displaySpeed() {
  oled.textSize(5);
  oled.text(0, 0, " %d  ", speed);
  oled.display();
}

void handleMovement() {
  if (mywcx.R1) {
    updateSpeed(25);
    displaySpeed();
    delay(10);
    while (mywcx.getButton()) {}
  }

  if (mywcx.R2) {
    updateSpeed(-25);
    displaySpeed();
    delay(10);
    while (mywcx.getButton()) {}
  }

  // Movement Control
  if (mywcx.RU && mywcx.RT) {
    motor(1, -speed);
    motor(2, 0);
  } else if (mywcx.RU && mywcx.LT) {
    motor(2, -speed);
    motor(1, 0);
  } else if (mywcx.RD && mywcx.RT) {
    motor(1, speed);
    motor(2, 0);
  } else if (mywcx.RD && mywcx.RT) {
    motor(2, speed);
    motor(1, 0);
  } else if (mywcx.RD) {
    fd(speed);
  } else if (mywcx.RU) {
    bk(speed);
  } else if (mywcx.RT) {
    sl(speed - 20);
  } else if (mywcx.LT) {
    sr(speed - 20);
  } else if(mywcx.RR){
    motor(1, -20);  // ใช้ความเร็ว 20
    motor(2, -20); 
  }else if(mywcx.RR && mywcx.LT){
    sr(speed - 55);
  }else if(mywcx.RR && mywcx.RT){
    sl(speed - 55);
  }else{
    AO();  // Stop all motors
  }
}

void handleServoActions() {
  if (mywcx.L2) {  // Lower servo
    baseServo(90);
    delay(700);
    gripperServo(gripperAngleOpen);  // Open gripper
    gripperAngle = gripperAngleOpen;
  }

  if (mywcx.L1) {  // Close gripper and reset servo
    gripperServo(gripperAngleClose);
    delay(200);
    baseServo(0);
    gripperAngle = gripperAngleClose;
  }
}

void ServoControl(){
   if (mywcx.RL) {  // Lower servo
    baseServo(110);
    delay(700);
    gripperServo(gripperAngleOpen);  // Open gripper
    gripperAngle = gripperAngleOpen;
  }

  /*if(mywcx.RR){
    if(gripperAngle > 0){
      gripperAngle--;
      gripperServo(gripperAngle);
      delay(15);
    }
  }*/
}

void handleColorActions() {
  if (mywcx.LU) {  // Set to green
    digitalWrite(BUZZER_PIN, HIGH);
    setColor(0, 255, 0);
  }

  if (mywcx.LL) {  // Set to red
    digitalWrite(BUZZER_PIN, HIGH);
    setColor(255, 0, 0);
  }

  if (mywcx.LR) {  // Set to yellow
    digitalWrite(BUZZER_PIN, HIGH);
    setColor(200, 255, 0);
  }

  if (mywcx.LD) {  // Turn off colors
    digitalWrite(BUZZER_PIN, LOW);
    setColor(0, 0, 0);
  }
}

// Servo control functions remain unchanged
void baseServo(int baseAngle) {
  servo(1, baseAngle);
}

void gripperServo(int gripperAngle) {
  servo(2, gripperAngle);
}