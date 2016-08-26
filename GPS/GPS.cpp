/*
 * GPS.cpp
 *
 *  Created on: 25.08.2015
 */

#include "GPS.h"
#include "Arduino.h"
#include "Motor.h"
#include "RotaryEncoder.h"

GPS::GPS(unsigned long interval) {
	m_interval = interval;
	m_new_position = "";
	m_position = "";
	m_heading = 0.0f;
	m_speed = 0.0f;
}

GPS::~GPS() {
	// TODO Auto-generated destructor stub
}

void GPS::update()
{
	/*while(Serial1.available() > 0)
	{
		char incoming = Serial1.read();
		if(incoming != '\n' && incoming != ' ' && incoming != '\r')
		{
			m_new_position += incoming;
		}
		else
		{
			if(m_new_position.length() != 0)
			{
				m_position = m_new_position;
				m_new_position = "";
				Serial.println(m_position);
			}
		}
	}*/
}

String GPS::debug()
{
	return m_position;

}

String GPS::debugHeader()
{
	return "Position";
}
