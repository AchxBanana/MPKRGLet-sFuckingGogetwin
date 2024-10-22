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
int speed = 100;  // Speed range: 50 to 100
int servoBaseAngle = 90;
int gripperAngleOpen = 30;
int gripperAngleClose = 0;
int gripperAngle = 0;
unsigned long lastActionTime = 0;
const int actionDelay = 1; // Action delay in milliseconds

// Variables for LED Blinking
bool ledState = false;  // สถานะเปิด/ปิดของ LED
unsigned long previousMillis = 0;  // เก็บเวลาครั้งสุดท้ายที่มีการกระพริบ
const long interval = 500;  // กำหนดช่วงเวลาการกระพริบ (500 ms หรือ 0.5 วินาที)
int currentColor = 0;  // เก็บสถานะของสีที่ใช้งานอยู่ (0 = ไม่กระพริบ)

// Function Prototypes
void initializePins();
void setColor(int red, int green, int blue);
void displaySpeed();
void handleMovement();
void handleServoActions();
void handleColorActions();
void ServoControl();
void blinkLED(int red, int green, int blue, bool withBeep = false);
void baseServo(int baseAngle);  // Function Prototype for baseServo
void gripperServo(int gripperAngle);  // Function Prototype for gripperServo

void setup() {
  mywcx.begin();
  initializePins();
  setColor(0, 0, 0);  // Turn off RGB LEDs initially
  displaySpeed();
  baseServo(0);  // Now baseServo() is declared before use
  gripperServo(gripperAngleOpen);  // Same for gripperServo()
}

void loop() {
  mywcx.getButton();
  
  // Check if enough time has passed for the next action
  if (millis() - lastActionTime > actionDelay) {
    handleMovement();
    ServoControl();
    handleServoActions();
    handleColorActions();  // เรียกใช้งานฟังก์ชันสำหรับการกระพริบ LED
    
    // Update the last action time
    lastActionTime = millis();
  }
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

void displaySpeed() {
  oled.textSize(5);
  oled.text(0, 0, " %d  ", speed);
  oled.display();
}

void handleMovement() {
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
  } else if (mywcx.RD && mywcx.LT) {
    motor(2, speed);
    motor(1, 0);
  } else if (mywcx.RD) {
    fd(speed);
  } else if (mywcx.RU) {
    bk(speed);
  } else if (mywcx.LU) {
    motor(1, -40);  // ใช้ความเร็ว 20
    motor(2, -40); 
  } else if (mywcx.RT) {
    sl(speed - 20);
  } else if (mywcx.LT) {
    sr(speed - 20);
  } else if (mywcx.RR) {
    motor(1, -20);  // ใช้ความเร็ว 20
    motor(2, -20); 
  } else if (mywcx.LL) {
    sr(speed - 75);
  } else if (mywcx.LR) {
    sl(speed - 75);
  } else {
    AO();  // Stop all motors
  }
}

void handleServoActions() {
  if (mywcx.L2) {  // Lower servo
    ao();
    baseServo(90);
    delay(450);
    gripperServo(gripperAngleOpen);  // Open gripper
    gripperAngle = gripperAngleOpen;
  }

  if (mywcx.L1) {  // Close gripper and reset servo
    gripperServo(gripperAngleClose);
    ao();
    delay(150);
    baseServo(0);
    gripperAngle = gripperAngleClose;
  }
}

void ServoControl() {
  static unsigned long lastServoControlAction = 0;

  if (mywcx.RL && (millis() - lastServoControlAction > actionDelay)) {
    lastServoControlAction = millis();
    baseServo(110);
    delay(600);
    gripperServo(gripperAngleOpen);  // Open gripper
    gripperAngle = gripperAngleOpen;
  }

  if (mywcx.LD) {
    baseServo(0);
  }
}

// ฟังก์ชันสำหรับการกระพริบของ LED พร้อมกับการส่งเสียง Buzzer เมื่อกดปุ่ม R2
void blinkLED(int red, int green, int blue, bool withBeep = false) {
  unsigned long currentMillis = millis();
  
  // เช็คว่าถึงเวลาที่ต้องเปลี่ยนสถานะ LED หรือไม่
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // บันทึกเวลาครั้งสุดท้ายที่มีการกระพริบ

    // สลับสถานะของ LED
    ledState = !ledState;

    // ถ้า LED เปิดให้แสดงสีที่ระบุ
    if (ledState) {
      setColor(red, green, blue);
      if (withBeep) {
        digitalWrite(BUZZER_PIN, HIGH);  // เปิดเสียง Buzzer เฉพาะปุ่ม R2
      }
    } else {
      setColor(0, 0, 0);  // ปิด LED
      digitalWrite(BUZZER_PIN, LOW);  // ปิดเสียง Buzzer
    }
  }
}

void handleColorActions() {
  // ปุ่ม LL สำหรับกระพริบสีแดง
  if (mywcx.LL && mywcx.LD) {
    currentColor = 1;  // ตั้งค่าสถานะสีแดง
  }
  // ปุ่ม LU สำหรับกระพริบสีเขียว
  if (mywcx.LU && mywcx.LD) {
    currentColor = 2;  // ตั้งค่าสถานะสีเขียว
  }
  // ปุ่ม LR สำหรับกระพริบสีเหลือง
  if (mywcx.LR && mywcx.LD) {
    currentColor = 3;  // ตั้งค่าสถานะสีเหลือง
  }

  // ปุ่ม R2 สำหรับกระพริบสีม่วงและเปิดเสียง Buzzer
  if (mywcx.R2) {
    currentColor = 4;  // ตั้งค่าสถานะสีม่วงพร้อมเสียง
  }

  // ปุ่ม R1 สำหรับปิดไฟทั้งหมด
  if (mywcx.R1) {
    currentColor = 0;  // ตั้งค่าสถานะปิดไฟ
    setColor(0, 0, 0);  // ปิดไฟ LED
    digitalWrite(BUZZER_PIN, LOW);  // ปิดเสียง Buzzer
  }

  // ตรวจสอบว่าสีไหนที่ต้องการให้กระพริบ
  if (currentColor == 1) {
    blinkLED(0, 0, 255);  // กระพริบสีแดง
  } else if (currentColor == 2) {
    blinkLED(0, 255, 0);  // กระพริบสีเขียว
  } else if (currentColor == 3) {
    blinkLED(255, 0, 0);  // กระพริบสีเหลือง
  } else if (currentColor == 4) {
    blinkLED(255, 255, 255, true);  // กระพริบสีม่วงและเปิดเสียง Buzzer
  }
}

// Function to control the base servo
void baseServo(int baseAngle) {
  servo(1, baseAngle);
}

// Function to control the gripper servo
void gripperServo(int gripperAngle) {
  servo(2, gripperAngle);
}
