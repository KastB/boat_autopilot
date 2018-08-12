/*
 * PID.h
 *
 *  Created on: 12.08.2018
 */
#pragma once
#include "TimersClass.h"
#include "FilterOnePole.h"

class PowerSensors : public TimerClass {
 public:
  PowerSensors(unsigned long interval, int voltagePin, int currentPin);
  virtual ~PowerSensors();
  void debug(HardwareSerial &serial);
  void debugHeader(HardwareSerial &serial);
  void update();

  void setFilterFrequency(float freq);

 private:
  FilterOnePole *m_lowpassOutput;
  float m_voltage;
  float m_current;

  int m_voltagePin;
  int m_currentPin;
};
