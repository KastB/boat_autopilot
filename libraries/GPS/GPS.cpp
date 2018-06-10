/*
 * GPS.cpp
 *
 *  Created on: 25.08.2015
 */

#include "Arduino.h"
#include "GPS.h"
#include "Motor.h"
#include "RotaryEncoder.h"

GPS::GPS(unsigned long interval, HardwareSerial *serial) {
  m_interval = interval;
  m_position = "";
  m_serial = serial;
}

GPS::~GPS() {
  // TODO Auto-generated destructor stub
}

void GPS::update() {
  while (Serial1.available() > 0) {
    char incoming = Serial1.read();
    if (incoming != '\n') { // && incoming != ' '
      if (incoming != '\r')
        m_position += incoming;
    } else {
      if (m_position.length() > 5) {
        m_serial->println(m_position);
        m_position = "";
      }
      else {
        m_position = "";
      }
    }
    if(m_position.length() > m_maxLength)
    {
      m_position = "";
    }
  }
}

void GPS::debug(HardwareSerial& serial) { serial.print("-"); }

void GPS::debugHeader(HardwareSerial& serial) { serial.print(F("GPS")); }
