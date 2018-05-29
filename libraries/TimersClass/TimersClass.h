/*
 * Radio.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_TIMER_CLASS_H_
#define LIBRARIES_TIMER_CLASS_H_
#include <string.h>
#include "Arduino.h"
#define MAXTIMERCLASSES 20

class TimerClass {
 public:
  TimerClass();
  virtual ~TimerClass() = 0;
  void checkAndRunUpdate() {
    if (millis() >= m_timestamp) {
      // m_timestamp = millis();
      m_timestamp = m_timestamp + m_interval;
      if (millis() >= m_timestamp && m_warnCounter > m_warnReduction) {
        Serial.println("Running out of time!!!");
        m_warnCounter = 0;
      }
      update();
    }
  }
  virtual void update() = 0;
  virtual void debug(HardwareSerial& serial) = 0;
  virtual void debugHeader(HardwareSerial& serial) = 0;
  unsigned long m_interval;

 private:
  unsigned long m_timestamp;
  const unsigned int m_warnReduction = 100;
  unsigned int m_warnCounter = 0;
  friend class TimersClass;
};

class TimersClass {
 public:
  TimersClass();
  virtual ~TimersClass();
  void checkAndRunUpdate();
  void debug(HardwareSerial& serial);
  void debugHeader(HardwareSerial& serial);
  void addTimer(TimerClass* timer);

 private:
  TimerClass* m_timers[MAXTIMERCLASSES];
};

#endif /* LIBRARIES_TIMER_CLASS_H_ */
