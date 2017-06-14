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

#include "PID.h"
#include "Arduino.h"

PID::PID(unsigned long interval, IMU *imu, Seatalk *seatalk, Motor *motor) {
	m_interval = interval;
/*	m_P = 36.0f;
	m_I = 3.79f;
	m_D = 82.02f;
*/
	m_P = 4.0f;
	m_I = 0.1f;
	m_D = 4.0f;

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

	m_iNoUpdateDelay = 5000;
	m_InoUpdate = 0;

	float filterFrequency = 1.0;

	// create a one pole (RC) lowpass filter
	m_lowpassFilter = new FilterOnePole ( LOWPASS, filterFrequency );
}

PID::~PID() {
}

void PID::setFilterFrequency(float freq)
{
	m_lowpassFilter->setFrequency(freq);
}

void PID::setWind()
{
	setWind(m_seatalk->m_wind.apparentAngle);
}
void PID::setWind(float goal)
{
	m_goal = goal;
	m_goalType = WIND;
	normalize(m_goal);
	m_lowpassFilter->setToNewValue(m_motor->getCurrentPosition());
	m_InoUpdate = millis() + m_iNoUpdateDelay;
}

void PID::setMag()
{
	float roll, pitch, yaw, filteredYaw;
	m_imu->getRPY(roll, pitch, yaw, filteredYaw);
	setMag(filteredYaw);
}
void PID::setMag(float goal)
{
	m_goal = goal;
	m_goalType = MAGNET;
	normalize(m_goal);
	m_lowpassFilter->setToNewValue(m_motor->getCurrentPosition());
	m_InoUpdate = millis() + m_iNoUpdateDelay;
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
	float p = m_P;
	unsigned long currentTime = millis();
	float error;
	int position;
	float roll, pitch, yaw, filteredYaw;
	m_imu->getRPY(roll, pitch, yaw, filteredYaw);
	if(m_goalType == MAGNET)
	{
		current = yaw;
		error = m_goal - current;
	}
	else if(m_goalType == WIND)
	{
		current = m_seatalk->m_wind.apparentAngle;
		error = current - m_goal;
	}
	else
	{
		m_lastError = 0.0;
		m_lastTime = currentTime;
		m_lastFilteredYaw = filteredYaw;
		return;
	}

	if( fabs(error) < 10.0f )
		p /= 2.0f;

	normalize(error);
	while (error > 180.0f)
	{
		error -= 360.0f;
	}
	while (error <= -180.0f)
	{
		error += 360.0f;
	}

	if( currentTime > m_lastTime) //overflow handling
	{
		float dt = (currentTime - m_lastTime);
		dt = dt / 1000.0f;



		float v = m_seatalk->m_speed.speed;
		v = max(v, 1.0);
		float rotVel = (m_lastFilteredYaw - filteredYaw);
		while(rotVel > 180.0f)
		{
			rotVel -= 360.0f;
		}
		while(rotVel < -180.0f)
		{
			rotVel += 360.0f;
		}
		rotVel /= dt;
		/*
		if(m_InoUpdate < currentTime) // dont integrate during swing-in	unless
		{
			m_errorSum += error * dt;
		}
		else
		{
			if(fabs(error) < 10.0) 	// error is small
			{
				m_errorSum += error * dt;
			}
		}*/


		if(fabs(rotVel) > 1.0f)
		{
			float tmp = m_errorSum + error * dt;
			if(fabs(tmp) < fabs(m_errorSum))
				m_errorSum = tmp;
		}
		else
		{
			m_errorSum += error * dt;
		}

		if(m_errorSum != m_errorSum)
			m_errorSum = 0.0;

		position = 	p * error +
							//Faster turns should be more damped => quadratic term
							m_D *  rotVel * fabs(rotVel)+
							m_I * m_errorSum;

		/*
		 * p: 2 d:8 i: 0.2
		position = 	m_P * error +
					m_D * (m_lastFilteredYaw - filteredYaw) / dt +
					m_I * m_errorSum; */
/*
		position = 	(m_P * error +
					m_D * pow((m_lastFilteredYaw - filteredYaw), 2) / dt) /
						  pow(v,2)	 * sin(fabs(error) / 90.0f * PI)+
								m_I * m_errorSum;
*/
		//don't increase integral over physical bounds of the hardware (anti windup)
		if(m_motor->getBlocked())
		{
			if (abs(m_motor->getCurrentPosition() < abs(position)))
			{
				m_errorSum += (m_motor->getCurrentPosition() - position) / m_I;
			}
		}
		//set position
		m_lowpassFilter->input(position);
		m_motor->gotoPos(m_lowpassFilter->output());
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
	return String(m_P) + "\t" + m_I + "\t" + m_D + "\t" + m_goalType + "\t" + m_goal + "\t" + m_lastError + "\t" + m_errorSum + "\t" + m_lastFilteredYaw;
}
String PID::debugHeader()
{
	return String("m_P") + "\t" + "m_I" + "\t" + "m_D" + "\t" + "m_goalType" + "\t" + "m_goal" + "\t" + "m_lastError" + "\t" + "m_errorSum" + "\t" + "m_lastFilteredYaw";
}
