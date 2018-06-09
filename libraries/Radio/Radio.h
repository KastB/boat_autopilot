/*
 * Radio.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_RADIO_RADIO_H_
#define LIBRARIES_RADIO_RADIO_H_
#include <RCSwitch.h>
#include "TimersClass.h"

class Radio : public TimerClass {
 public:
  Radio(unsigned long interval, int interruptIn, int pinOut);
  virtual ~Radio();
  void update();

  enum key { A = 1234, B = 5678, C = 9043, D = 5187, E = 0 };

  enum key getLastKey();

  void debug(HardwareSerial& serial);
  void debugHeader(HardwareSerial& serial);

 private:
  RCSwitch m_radio;

  enum key m_bytesToSent;
  enum key m_pressedButton;

  unsigned long m_repeatDelay;
  unsigned long m_lastRequestedTime;
  enum key m_lastPressedKey;

  void send();
  void receive();
  friend class UI;
};

#endif /* LIBRARIES_RADIO_RADIO_H_ */
