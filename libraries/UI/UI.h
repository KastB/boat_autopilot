/*
 * UI.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_UI_UI_H_
#define LIBRARIES_UI_UI_H_
#include <IMU.h>
#include <IR.h>
#include <Motor.h>
#include <PID.h>
#include <Radio.h>
#include "KeypadWrapper.h"
#include "TimersClass.h"

class UI : public TimerClass {
 public:
  UI(unsigned long interval, IR *ir, Radio *radio, Motor *motor, PID *pid,
     IMU *imu, KeypadWrapper *kpd, TimersClass *timer);
  virtual ~UI();
  void update();
  void debug(HardwareSerial &serial);
  void debugHeader(HardwareSerial &serial);

 private:
  void msg_help(HardwareSerial &serial);

  void exec(String cmd);
  void setDebugDevisor(int nterval);

  String m_cmdSerial;
  String m_cmdKeypad;
  int m_debugDevisor;
  int m_counter;

  IR *m_ir;
  Radio *m_radio;
  Motor *m_motor;
  PID *m_pid;
  IMU *m_imu;
  KeypadWrapper *m_kpd;
  TimersClass *m_timer;
};

#endif /* LIBRARIES_UI_UI_H_ */
