/*
 * Motor.cpp
 *
 *  Created on: 25.08.2015
 */

#include "Arduino.h"
#include "Motor.h"

// TODO: add watchdog, that stops motor if no movement is detected despite
// movement command is active

Motor::Motor(unsigned long interval, RotaryEncoder* rotary_encoder, int ina,
             int inb, int pwm, int diaga, int diagb, int smoothing) {
  m_interval = interval;
  m_currentDirection = STOP;
  m_MSStopped = 0;
  m_minStopMS = smoothing;

  m_targetPosition = 0;

  m_minimalSpeed = 400;
  m_maximalSpeed = 800;
  m_speedFactor = 20.0f;
  m_hysteresis = 3;

  m_inaPin = ina;
  m_inbPin = inb;
  m_pwmPin = pwm;
  m_diagaPin = diaga;
  m_diagbPin = diagb;
  m_stopPin = 44;
  m_startPin = 46;
  m_parkingPin = 48;

  m_rotaryEncoder = rotary_encoder;
  m_parking = false;
  m_blocked = false;

  m_initMax = 0.0;
  m_initMin = 0.0;
  m_initStatus = -1;
  m_initLastCommand = 0;
  m_initMinTime = 2000;

  pinMode(m_inaPin, OUTPUT);
  pinMode(m_inbPin, OUTPUT);
  pinMode(m_diagaPin, INPUT);
  pinMode(m_diagbPin, INPUT);
  pinMode(m_startPin, INPUT_PULLUP);
  pinMode(m_stopPin, INPUT_PULLUP);
  pinMode(m_parkingPin, INPUT_PULLUP);
}

Motor::~Motor() {}

void Motor::initialize() {
  if (m_initStatus == -1) {
    m_initStatus = 1;
  } else {
    m_initStatus = 5;
  }
}

void Motor::reinitialize() { m_initStatus = 1; }

void Motor::gotoPos(int pos) {
  m_targetPosition = pos;
  m_parking = false;
}

void Motor::moveRel(int value) {
  m_parking = false;
  m_targetPosition += value;
}

void Motor::stop() {
  m_parking = false;
  motor_stop();
}

void Motor::gotoParking() { m_parking = true; }

void Motor::motor_cw(int speed) {
  // Serial.print("cw");
  // Serial.println(speed);
  digitalWrite(m_inaPin, HIGH);
  digitalWrite(m_inbPin, LOW);
  analogWrite(m_pwmPin, speed);
}
void Motor::motor_ccw(int speed) {
  // Serial.print("ccw");
  // Serial.println(speed);
  digitalWrite(m_inaPin, LOW);
  digitalWrite(m_inbPin, HIGH);
  analogWrite(m_pwmPin, speed);
}

void Motor::motor_stop() {
  // Serial.println("stop");
  m_targetPosition = m_rotaryEncoder->getCurrentPosition();
  digitalWrite(m_inaPin, LOW);
  digitalWrite(m_inbPin, LOW);
  analogWrite(m_pwmPin, 0);
}

void Motor::controlMotor(direction dir, int speed, bool overwrite) {
  // Serial.println(String("CM") + dir + "##" + speed + "##" + overwrite);
  if (abs(m_currentDirection - dir) > 1 ||  // stop when switching from forward to backward
      (m_MSStopped > 0 && millis() < m_minStopMS + m_MSStopped) ||  // minimal stop criteria not fulfilled
      dir == STOP) {
    motor_stop();
    if (m_MSStopped == 0)       //TODO: proper overflow handling
      m_MSStopped = millis();
  } else {
    m_MSStopped = 0;
    if (dir == BACKWARDS) {
      if (digitalRead(m_stopPin) == LOW) {
        motor_cw(speed);
      } else {
        // Serial.print("b");
        motor_stop();
      }
    } else {
      if ((digitalRead(m_startPin) == LOW || overwrite) &&
          digitalRead(m_parkingPin) == LOW) {
        motor_ccw(speed);
      } else {
        // Serial.print("c");
        motor_stop();
      }
    }
  }
  m_currentDirection = dir;
}

bool Motor::getBlocked() {
  return digitalRead(m_startPin) == HIGH || digitalRead(m_stopPin) == HIGH;
}

int Motor::getCurrentPosition() {
  return m_rotaryEncoder->getCurrentPosition();
}

void Motor::update() {
  if (m_initLastCommand > millis())
    m_initLastCommand = millis();
  if (m_initLastCommand + m_initMinTime < millis()) {
    m_initLastCommand = millis();
    // Serial.println(m_initStatus);

    if (m_initStatus == 1) {
      if (abs(m_targetPosition - m_rotaryEncoder->getCurrentPosition()) < 5) {
        m_targetPosition += 150;
        m_initStatus = 2;
      }
    } else if (m_initStatus == 2) {
      if (m_targetPosition == m_rotaryEncoder->getCurrentPosition()) {
        m_targetPosition -= 1500;
        m_initStatus = 3;
      }
    } else if (m_initStatus == 3) {
      if (m_targetPosition == m_rotaryEncoder->getCurrentPosition()) {
        m_initMin = m_targetPosition;
        m_targetPosition += 1500;
        m_initStatus = 4;
      }
    } else if (m_initStatus == 4) {
      if (m_targetPosition == m_rotaryEncoder->getCurrentPosition()) {
        m_initMax = m_targetPosition;
        //		Serial.print(m_initMax);
        //		Serial.print("##");
        //		Serial.println(m_initMin);
        m_rotaryEncoder->setCurrentPosition((m_initMax - m_initMin) / 2);
        m_initStatus = 5;
      }
    } else if (m_initStatus == 5) {
      m_targetPosition = 0;
      m_initStatus = 6;
    }
  }

  // Distance => Speed
  // Sign 	   => Direction
  if (m_parking) {
    controlMotor(FORWARDS, 512, true);
  } else {
    int delta = m_targetPosition - m_rotaryEncoder->getCurrentPosition();
    int speed;
    direction d;
    if (delta < 0)
      d = FORWARDS;
    else
      d = BACKWARDS;

    delta = abs(delta);
    if (d == m_currentDirection) delta += m_hysteresis;

    speed = delta * m_speedFactor;
    speed += m_minimalSpeed * 3 / 4;

    if (speed < m_minimalSpeed) d = STOP;
    if (speed > m_maximalSpeed) speed = m_maximalSpeed;

    controlMotor(d, speed);
  }
}

void Motor::debug(HardwareSerial& serial) {
  char spacer = ',';
  serial.print(m_rotaryEncoder->getCurrentPosition());
  serial.print(spacer);
  serial.print((int)m_currentDirection);
  serial.print(spacer);
  serial.print(m_targetPosition);
  serial.print(spacer);
  serial.print((digitalRead(m_startPin) == LOW));
  serial.print(spacer);
  serial.print((digitalRead(m_stopPin) == LOW));
  serial.print(spacer);
  serial.print((digitalRead(m_parkingPin) == LOW));
  serial.print(spacer);
  serial.print((digitalRead(m_diagaPin) == LOW));
  serial.print(spacer);
  serial.print((digitalRead(m_diagbPin) == LOW));
}

void Motor::debugHeader(HardwareSerial& serial) {
  serial.print(
      F("CurrentPosition,CurrentDirection,TargetPosition,startBut"
        "ton,stopButton,parkingButton,DiagA,DiagB"));
}
