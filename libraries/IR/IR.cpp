/*
 * PID.cpp
 * Angles in DEGREE (not radians as usual)
 * Angles positive CLOCKWISE (not counterclockwise as usual)
 *  Created on: 25.08.2015
 */

#include "Arduino.h"
#include "IR.h"

IR::IR(unsigned long interval) {
  m_interval = interval;
  m_initialized = false;
  m_hold = false;
  m_irrecv = NULL;
  m_lastKey = 0;
  m_lastReceived = millis();
  m_keyReleasedTimeout = 300;
  m_keyHoldTimeout = 1000;
  m_keyHoldTime = 0;
}

IR::~IR() {}

void IR::update() {
  if (!m_initialized)  // doing this before setup is run (in Constructor) will
                       // cause undefined behavior
  {
    m_irrecv = new IRrecv(2);
    m_irrecv->enableIRIn();  // Start the receiver
    m_initialized = true;
  }
  decode_results results;            // Somewhere to store the results
  if (m_irrecv->decode(&results)) {  // Grab an IR code
    if (results.decode_type == NEC) {
      m_lastReceived = millis();
      // Serial.println(results.value);      // Blank line between entries
      if (results.value == HOLD) {
        if (m_keyHoldTime == 0) m_keyHoldTime = millis();

        if (!m_hold && m_keyHoldTime + m_keyHoldTimeout > millis()) {
          m_hold = true;
          switch (m_lastKey) {
            case LEFT:
              m_cmd = "D999999";
              break;
            case RIGHT:
              m_cmd = "I999999";
              break;
            case TACK:
              m_cmd = "T";
              break;
            case GP:
              m_cmd = "GP";
              break;
            default:
              break;
          }
        }
      } else {
        m_hold = false;
        m_keyHoldTime = 0;
      }

      switch (results.value) {
        case LEFT:
          m_cmd = "D1";
          break;
        case RIGHT:
          m_cmd = "I1";
          break;
        default:
          break;
      }

      if (results.value == WIND) {
        if (m_cmd.length() > 0) {
          if (m_cmd.charAt(1) == 'W') {
            m_cmd = m_cmd.substring(1);
            return;
          }
        }
        m_cmd = "-W";
      } else if (results.value == MAG) {
        if (m_cmd.length() > 0) {
          if (m_cmd.charAt(1) == 'M') {
            m_cmd = m_cmd.substring(1);
            return;
          }
        }
        m_cmd = "-M";
      } else {
        switch (results.value) {
          case ZERO:
            m_cmd += "0";
            break;
          case ONE:
            m_cmd += "1";
            break;
          case TWO:
            m_cmd += "2";
            break;
          case THREE:
            m_cmd += "3";
            break;
          case FOUR:
            m_cmd += "4";
            break;
          case FIVE:
            m_cmd += "5";
            break;
          case SIX:
            m_cmd += "6";
            break;
          case SEVEN:
            m_cmd += "7";
            break;
          case EIGHT:
            m_cmd += "8";
            break;
          case NINE:
            m_cmd += "9";
            break;
        }
      }

      if (m_lastKey != results.value && results.value != HOLD) {
        // Serial.print(m_lastKey);
        // Serial.print("##");
        // Serial.println(results.value);
        switch (m_lastKey) {
          case LEFT:
          case RIGHT:
            m_cmd = "S2";
            break;
          case GP:
            m_cmd = "S";
            break;
          default:
            break;
        }
        m_lastKey = results.value;
      }
    }
    m_irrecv->resume();  // Prepare for the next value
  } else                 // key released
  {
    if (millis() > m_lastReceived + m_keyReleasedTimeout) {
      switch (m_lastKey) {
        case LEFT:
        case RIGHT:
          m_cmd = "S2";
          break;
        case GP:
          m_cmd = "S";
          break;
        default:
          break;
      }
      m_lastKey = 0;
    }
  }
}

String IR::getCommand() {
  if (m_cmd.length() == 0) {
    return "";
  }
  if (m_cmd.charAt(0) != '-') {
    String cmd = m_cmd;
    m_cmd = "";
    // Serial.println(cmd);
    return cmd;
  } else {
    return "";
  }
}

String IR::debug() { return ""; }
String IR::debugHeader() { return ""; }
