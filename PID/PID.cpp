/*
 * PID.cpp
 * Angles in DEGREE (not radians as usual)
 * Angles positive CLOCKWISE (not counterclockwise as usual)
 *  Created on: 25.08.2015
 */

#include "PID.h"
#include "Arduino.h"

PID::PID(unsigned long interval, IMU *imu, Seatalk *seatalk, Motor *motor) {
	m_interval = interval;
	m_P = 4.0f;
	m_I = 10.0f;
	m_D = 0.5f;

	m_motor = motor;
	m_imu = imu;
	m_seatalk = seatalk;

	m_goal = 0.0f;
	m_goalType = INACTIVE;

	m_lastTime = millis();
	m_errorSum = 0.0f;
	m_lastError = 0.0f;

	m_tackStartTime = 0;
	m_tackMinTime = 1000;
}

PID::~PID() {
}

void PID::setWind()
{
	m_goal = m_seatalk->m_wind.apparentAngle;
	m_goalType = WIND;
	normalize(m_goal);
}
void PID::setWind(float goal)
{
	m_goal = goal;
	m_goalType = WIND;
	normalize(m_goal);
}

void PID::setMag()
{
	float roll, pitch, yaw;
	m_imu->getRPY(roll, pitch, yaw);
	m_goal = yaw;
	m_goalType = MAGNET;
	normalize(m_goal);
}
void PID::setMag(float goal)
{
	m_goal = goal;
	m_goalType = MAGNET;
	normalize(m_goal);
}

void PID::tack()
{
	if(millis() > m_tackStartTime + m_tackMinTime)
	{
		m_tackStartTime = millis();
		if(m_goalType == WIND)
		{
			m_goal = -m_goal;
		}
		else
		{
			m_goal += m_seatalk->m_wind.apparentAngle * 2.0f;
		}
		normalize(m_goal);
	}
}

void PID::update()
{
	if( m_tackStartTime > millis())	//overflow handling
	{
		m_tackStartTime = millis();
	}

	float current = 0.0;
	unsigned long currentTime = millis();
	float error;
	int position;
	float roll, pitch, yaw;
	m_imu->getRPY(roll, pitch, yaw);
	if(m_goalType == MAGNET)
		current = yaw;
	else if(m_goalType == WIND)
		current = m_seatalk->m_wind.apparentAngle;
	else
		return;

	error = m_goal - current;
	normalize(error);
	while (error > 360.0f)
	{
		error -= 180.0f;
	}
	while (error <= -180.0f)
	{
		error += 360.0f;
	}

	if( currentTime > m_lastTime) //overflow handling
	{
		float dt = (currentTime - m_lastTime);
		dt = dt / 1000.0f;
		float errorSum = m_errorSum + error * dt;
		position = 	m_P * error +
					m_D * (m_lastError - error) / dt +
					m_I * errorSum;
		//set position
		m_motor->gotoPos(position);

		//don't increase integral over physical bounds of the hardware
		if(!m_motor->getBlocked())
		{
			m_errorSum = errorSum;
		}
		else
		{
			m_errorSum = (float)m_motor->getCurrentPosition() / m_I;
		}
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
void PID::setInactiv()
{
	m_goalType = INACTIVE;
}
void PID::increase(int value)
{
	if(m_goalType == INACTIVE)
	{
		m_motor->moveRel(value);
	}
	else
	{
		m_goal += value;
	}
}
void PID::decrease(int value)
{
	if(m_goalType == INACTIVE)
	{
		m_motor->moveRel(-value);
	}
	else
	{
		m_goal -= value;
	}
}

void PID::normalize(float &angle)
{
	//normalize
	while (angle > 360.0f)
	{
		angle -= 360.0f;
	}
	while (angle <= 0.0f)
	{
		angle += 360.0f;
	}
}
String PID::debug()
{
	return String(m_P) + "\t" + m_I + "\t" + m_D + "\t" + m_goalType + "\t" + m_goal + "\t" + m_lastError + "\t" + m_errorSum;
}
String PID::debugHeader()
{
	return String("m_P") + "\t" + "m_I" + "\t" + "m_D" + "\t" + "m_goalType" + "\t" + "m_goal" + "\t" + "m_lastError" + "\t" + "m_errorSum";
}
