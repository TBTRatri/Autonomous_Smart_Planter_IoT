#include <Servo.h>

// Relays (Active-LOW Opto-Isolated 4-Channel Module)
const int RELAY_PUMP_A = 7; // Pump A: Fresh Water Irrigation
const int RELAY_PUMP_B = 6; // Pump B: Active Drainage
const int RELAY_PUMP_C = 5; // Pump C: Organic Repellent Spray

// Actuator - Servo
const int SERVO_PIN    = 9; // SG90 PWM Servo

// Sensors - Digital
const int PIR_PIN      = 2; // Passive Infrared Motion Sensor
const int TRIG_PIN     = 4; // HC-SR04 Ultrasonic Trigger
const int ECHO_PIN     = 3; // HC-SR04 Ultrasonic Echo

// Sensors - Analog
const int MOISTURE_PIN = A0; // Soil Moisture Sensor
const int WATER_LVL_PIN= A1; // Catchment Tray Water Level Sensor

// Thresholds
const int MOISTURE_DRY_THRESHOLD = 200; // >= 200 triggers irrigation
const int FLOOD_LIMIT_THRESHOLD   = 500; // > 500 (> 2cm) triggers active drainage
const int DISTANCE_THRESHOLD_CM  = 10;  // < 10 cm perimeter detection

const int RELAY_ON  = LOW;
const int RELAY_OFF = HIGH;

Servo deterrentServo;

long readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);

  // Fail-safe initialization to prevent relay firing on boot
  digitalWrite(RELAY_PUMP_A, RELAY_OFF);
  digitalWrite(RELAY_PUMP_B, RELAY_OFF);
  digitalWrite(RELAY_PUMP_C, RELAY_OFF);

  pinMode(RELAY_PUMP_A, OUTPUT);
  pinMode(RELAY_PUMP_B, OUTPUT);
  pinMode(RELAY_PUMP_C, OUTPUT);

  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  deterrentServo.attach(SERVO_PIN);
  deterrentServo.write(0);

  Serial.println(F("[SYSTEM READY] Autonomous Smart Planter online."));
}

void loop() {
  int moistureVal = analogRead(MOISTURE_PIN);
  int waterLvlVal = analogRead(WATER_LVL_PIN);
  int pirMotion   = digitalRead(PIR_PIN);
  long distanceCm = readUltrasonicDistance();

  // Priority 1: Drainage overrules irrigation
  if (waterLvlVal > FLOOD_LIMIT_THRESHOLD) {
    digitalWrite(RELAY_PUMP_B, RELAY_ON);
    digitalWrite(RELAY_PUMP_A, RELAY_OFF);
  } else {
    digitalWrite(RELAY_PUMP_B, RELAY_OFF);

    // Priority 2: Irrigation
    if (moistureVal >= MOISTURE_DRY_THRESHOLD) {
      digitalWrite(RELAY_PUMP_A, RELAY_ON);
    } else {
      digitalWrite(RELAY_PUMP_A, RELAY_OFF);
    }
  }

  // Pest deterrent logic
  if (pirMotion == HIGH && distanceCm < DISTANCE_THRESHOLD_CM) {
    digitalWrite(RELAY_PUMP_C, RELAY_ON);
    for (int pos = 0; pos <= 90; pos += 15) {
      deterrentServo.write(pos);
      delay(30);
    }
    for (int pos = 90; pos >= 0; pos -= 15) {
      deterrentServo.write(pos);
      delay(30);
    }
    digitalWrite(RELAY_PUMP_C, RELAY_OFF);
  } else {
    digitalWrite(RELAY_PUMP_C, RELAY_OFF);
    deterrentServo.write(0);
  }

  delay(1000);
}
