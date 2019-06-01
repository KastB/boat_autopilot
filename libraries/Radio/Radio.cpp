/*
 * Radio.cpp
 *
 *  Created on: 25.08.2015
 */

#include <RCSwitch.h>  //Momentan Fernbedienung codieren/decodieren
#include "Radio.h"

Radio::Radio(unsigned long interval, int interruptIn, int pinOut) {
  m_interval = interval;
  // remote control - 433Mhz communication
  m_bytesToSent = E;
  m_radio = RCSwitch();
  m_radio.enableTransmit(
      pinOut);  // Transmitter is connected to Arduino Pin #10
  m_radio.enableReceive(
      interruptIn);             // Receiver on inerrupt 0 => that is pin #2
  m_radio.setPulseLength(500);  // Optional set pulse length.

  m_pressedButton = E;
  m_lastPressedKey = E;
  m_repeatDelay = 750;
  m_lastRequestedTime = 0;
}

Radio::~Radio() {
  // TODO Auto-generated destructor stub
}

enum Radio::key Radio::getLastKey() {
  if (m_lastRequestedTime > millis()) m_lastRequestedTime = millis();

  if (millis() > m_lastRequestedTime + m_repeatDelay ||
      m_lastPressedKey != m_pressedButton) {
    m_lastPressedKey = m_pressedButton;
    m_lastRequestedTime = millis();
    return m_pressedButton;
  }
  return E;
}

void Radio::update() {
  receive();
  send();
}
void Radio::receive() {
  m_pressedButton = E;
  if (m_radio.available()) {
    int received = m_radio.getReceivedValue();
    if (received == A || received == B || received == C || received == D) {
      m_pressedButton = (key)received;
    } else {
      m_pressedButton = E;
    }
    m_radio.resetAvailable();
  }
}

void Radio::send() {
  if (m_bytesToSent != E) {
    m_radio.send(m_bytesToSent, 24);
  }
}
void Radio::debug(HardwareSerial& serial) {
  serial.print((int)m_pressedButton);
  serial.print(",");
  serial.print((int)m_bytesToSent);
}

void Radio::debugHeader(HardwareSerial& serial) {
  serial.print(F("m_pressedButtonDebug,m_bytesToSent"));
}
