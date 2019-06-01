/*
 * PID.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_PID_PID_H_
#define LIBRARIES_PID_PID_H_

#include <IMU.h>
#include <Motor.h>
#include <Seatalk.h>
#include "TimersClass.h"

class PID : public TimerClass {
 public:
  PID(unsigned long interval, IMU *imu, Seatalk *seatalk, Motor *motor);
  virtual ~PID();
  void debug(HardwareSerial &serial);
  void debugHeader(HardwareSerial &serial);
  void update();

  void setWind();            // current
  void setWind(float goal);  // with goal

  void setMag();            // current
  void setMag(float goal);  // with goal

  void tack();

  void toggleState();
  void setInactiv();
  void resetErrorSum();
  void increase(int value);
  void decrease(int value);

  void setFilterFrequency(float freq);

 private:
  enum goalType { INACTIVE, WIND, MAGNET };
  // controller
  float m_P;
  float m_P2;
  float m_I;
  float m_D;

  float m_settled;

  float m_errorSum;
  float m_lastError;
  float m_lastYaw;
  unsigned long m_lastTime;

  unsigned long
      m_iNoUpdateDelay;  // delay after goal change before I is updated again
  unsigned long m_InoUpdate;  // no update of I till before this time
  float m_rotVelDyn;  // no update of I when boat rotates faster than this speed
                      // (°/s)

  float m_closeHauledAngle;  // when we are higher on the wind than this number
                             // we are high on the wind [°]

  FilterOnePole *m_lowpassOutput;

  unsigned long m_tackStartTime;
  unsigned long m_tackMinTime; /** minimal time between two tacks*/

  float m_goal;
  enum goalType m_goalType;

  IMU *m_imu;
  Motor *m_motor;
  Seatalk *m_seatalk;

  void normalize(float &error);

  friend class UI;
};

#endif /* LIBRARIES_PID_PID_H_ */
