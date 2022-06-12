/*
 * Motor.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_MOTOR_MOTOR_H_
#define LIBRARIES_MOTOR_MOTOR_H_
#include "RotaryEncoder.h"
#include "TimersClass.h"

class Motor : public TimerClass {
 public:
  enum direction { FORWARDS, STOP, BACKWARDS };
  Motor(unsigned long interval, RotaryEncoder* rotary_encoder, int ina = 3,
        int inb = 4, int pwm = 7, int diaga = 5, int diagb = 6,
        int smoothing = 100);
  virtual ~Motor();
  void debug(HardwareSerial& serial);
  void debugHeader(HardwareSerial& serial);
  void update();

  void gotoPos(int pos);  //+/- range (+ moves to the front => boat goes
                          //counterclockwise)
  void moveRel(int value);
  void stop();
  void gotoParking();
  void initialize();
  void reinitialize();

  bool getBlocked();
  int getCurrentPosition();

 private:
  direction m_currentDirection;
  unsigned int m_MSStopped;
  unsigned int m_minStopMS;

  // motor
  // Voltage Source for motor:   Pin on Olimex: 1
  //+5V:						  Pin on Olimex: 2
  // GND:						  Pin on Olimex: 3
  int m_inaPin;    // Pin on Olimex: 4
  int m_inbPin;    // Pin on Olimex: 5
  int m_pwmPin;    // Pin on Olimex: 6
  int m_diagaPin;  // Pin on Olimex: 7
  int m_diagbPin;  // Pin on Olimex: 8

  int m_startPin;    // Pin at negative end
  int m_stopPin;     // Pin at positive end
  int m_parkingPin;  // Pin to go to when turnung off	+ => larger than stop
                     // pin

  int m_targetPosition;  // Target Position in normalized coordinates

  float m_speedFactor;  // P-value of position controller
  int m_minimalSpeed;
  int m_maximalSpeed;
  int m_hysteresis;
  float m_maxAcceleration;
  float m_lastSpeed;

  bool m_parking;

  bool m_blocked;

  int m_initStatus;
  int m_initMax;
  int m_initMin;
  unsigned long m_initLastCommand;
  unsigned long m_initMinTime;

  void motor_cw(int speed);
  void motor_ccw(int speed);
  void motor_stop();

  // maxSpeed is 800
  void controlMotor(direction dir, int speed, bool overwrite = false);

  RotaryEncoder* m_rotaryEncoder;

  friend class UI;
};

#endif /* LIBRARIES_MOTOR_MOTOR_H_ */
