/*
 * PID.cpp
 * Angles in DEGREE (not radians as usual)
 * Angles positive CLOCKWISE (not counterclockwise as usual)
 *  Created on: 25.08.2015
 *
 *
 *
 *  KpKrit = 60
 *  TKrit = 19s
 *
 *  Kp = 0.6 * KpKrit					= 36.0
 *  Ki = 0.6 * KpKrit * 2 / TKrit		= 3.79
 *  Kd = 0.6 * KpKrit * 0.12 * TKrit	= 82.02
 *
 *  Kp = 27.0
 *  Ki = 1.67
 *
 */

#include "Arduino.h"
#include "PID.h"

/* TODO: find nicer solution for I - problems (I affects dynamic PID performance
   negatively, but needs to be rather big when under sail
*/

PID::PID(unsigned long interval, IMU *imu, Seatalk *seatalk, Motor *motor) {
	m_interval = interval;

	m_P  = 	4.0f;
	m_P2 = 	2.0f;
	m_I  = 	0.2f;
	m_D  = 	3.0f;
	m_settled = 5.0f;

	m_motor = motor;
	m_imu = imu;
	m_seatalk = seatalk;

	m_goal = 0.0f;
	m_goalType = INACTIVE;

	m_lastTime = millis();
	m_errorSum = 0.0f;
	m_lastError = 0.0f;
	m_lastFilteredYaw = 0.0f;

	m_tackStartTime = 0;
	m_tackMinTime = 1000;

	m_iNoUpdateDelay = 7000;
	m_InoUpdate = 0;
	m_rotVelDyn = 1.0f;

	// create a one pole (RC) lowpass filter
	m_lowpassOutput = new FilterOnePole ( LOWPASS, 1.0 );

	m_closeHauledAngle = 45.0f;
}

PID::~PID() {}

void PID::setFilterFrequency(float freq) {
	m_lowpassOutput->setFrequency(freq);
}

void PID::setWind() { setWind(m_seatalk->m_wind.apparentAngle); }
void PID::setWind(float goal) {
	m_goal = goal;
	m_goalType = WIND;
	normalize(m_goal);
	m_lowpassOutput->setToNewValue(m_motor->getCurrentPosition());
	m_InoUpdate = millis() + m_iNoUpdateDelay;
}

void PID::setMag() {
	float roll, pitch, yaw, filteredYaw;
	m_imu->getRPY(roll, pitch, yaw, filteredYaw);
	setMag(filteredYaw);
}
void PID::setMag(float goal) {
	m_goal = goal;
	m_goalType = MAGNET;
	normalize(m_goal);
	m_lowpassOutput->setToNewValue(m_motor->getCurrentPosition());
	m_InoUpdate = millis() + m_iNoUpdateDelay;
}

void PID::tack() {
  if (millis() > m_tackStartTime + m_tackMinTime) {
		m_tackStartTime = millis();
    if (m_goalType == WIND) {
			m_goal = -m_goal;
    } else {
			m_goal += m_seatalk->m_wind.apparentAngleFiltered->output() * 2.0f;
		}
		normalize(m_goal);
	}
	m_InoUpdate = millis() + m_iNoUpdateDelay;
}


void PID::update() {
	if( m_tackStartTime > millis())	//overflow handling
	{
		m_tackStartTime = millis();
	}

	float p;
	unsigned long currentTime = millis();
	float error;
	int position;
	float roll, pitch, yaw, filteredYaw, current_direction;
	boolean increase_error = false;
	m_imu->getRPY(roll, pitch, yaw, filteredYaw);

  if (m_goalType == MAGNET) {
		current_direction = yaw;
		normalize(current_direction);
		error = m_goal - current_direction;
	}
	else if(m_goalType == WIND)
	{
		current_direction = m_seatalk->m_wind.apparentAngle;
		normalize(current_direction);
		error = current_direction - m_goal;

		//if we are high on the wind and higher -than commanded scale error up
		if (fabs(m_goal) < m_closeHauledAngle && fabs(current_direction) < m_goal)
				increase_error = true;
	} else {
		m_lastError = 0.0;
		m_lastTime = currentTime;
		m_lastFilteredYaw = filteredYaw;
		return;
	}
	normalize(error);

  if (fabs(error) < m_settled) { // smaller P value for settled system => less
                                 // unnecessary rudder movements
		p = m_P2;
  }
  else if (fabs(error) < 2.0f * m_settled) { // larger P value for unsettled
                                             // system => faster response
		p = m_P;
  }
	else								// smooth transition between settled and unsettled system
	{
		p = m_P2 + (m_P - m_P2) / m_settled * (fabs(error)-m_settled);
	}
	if (increase_error) p = m_P * 1.1f;

	if( currentTime > m_lastTime) //overflow handling
	{
		float dt = (currentTime - m_lastTime);
		dt = dt / 1000.0f;

		float rotVel = (m_lastFilteredYaw - filteredYaw);
		normalize(rotVel);

		rotVel /= dt;

    if (currentTime > m_InoUpdate) {
      if (fabs(rotVel) > m_rotVelDyn || m_motor->getBlocked()) {
				float tmp = m_errorSum + error * dt;
        if (fabs(tmp) < fabs(m_errorSum)) m_errorSum = tmp;
      } else {
				m_errorSum += error * dt;
			}
		}

    if (m_errorSum != m_errorSum) m_errorSum = 0.0;

		position = 	p * error +
					//Faster turns should be more damped => quadratic term
					m_D * rotVel * fabs(rotVel) +
					m_I * m_errorSum;

		//set position
		m_lowpassOutput->input(position);
		m_motor->gotoPos(m_lowpassOutput->output());
		m_lastFilteredYaw = filteredYaw;
	}
	m_lastError = error;
	m_lastTime = currentTime;
}

void PID::toggleState()
{
	switch(m_goalType)
	{
		case INACTIVE: m_goalType = WIND; break;
		case WIND: m_goalType = MAGNET; break;
		case MAGNET: m_goalType = INACTIVE; break;
		default: m_goalType = INACTIVE;break;
	}
}

void PID::setInactiv() { m_goalType = INACTIVE; }

void PID::resetErrorSum() { m_errorSum = 0.0f; }

void PID::increase(int value) {
  if (m_goalType == INACTIVE) {
		m_motor->moveRel(value);
  } else {
		m_goal += value;
	}
	normalize(m_goal);
	m_InoUpdate = millis() + m_iNoUpdateDelay;
}
void PID::decrease(int value) {
  if (m_goalType == INACTIVE) {
		m_motor->moveRel(-value);
  } else {
		m_goal -= value;
	}
	normalize(m_goal);
	m_InoUpdate = millis() + m_iNoUpdateDelay;
}

void PID::normalize(float &angle)
{
  //normalize
  while (angle > 180.0f) {
	angle -= 360.0f;
  }
  while (angle <= -180.0f) {
	angle += 360.0f;
  }
}

void PID::debug(HardwareSerial &serial) {
  char spacer = '\t';
  serial.print(m_P);
  serial.print(spacer);
  serial.print(m_I);
  serial.print(spacer);
  serial.print(m_D);
  serial.print(spacer);
  serial.print(m_goalType);
  serial.print(spacer);
  serial.print(m_goal);
  serial.print(spacer);
  serial.print(m_lastError);
  serial.print(spacer);
  serial.print(m_errorSum);
  serial.print(spacer);
  serial.print(m_lastFilteredYaw);
}

void PID::debugHeader(HardwareSerial &serial) {
  serial.print(
      "m_P\tm_I\tm_D\tm_goalType\tm_goal\tm_lastError\tm_errorSum\tm_"
      "lastFilteredYaw");
}
