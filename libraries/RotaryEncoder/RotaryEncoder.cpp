/*
 * rotaryEncoder.cpp
 *
 *  Created on: 25.08.2015
 */

#include "RotaryEncoder.h"
#include "Arduino.h"

RotaryEncoder::RotaryEncoder(unsigned long interval, int pinA, int pinB, int clearing) {
	m_interval = interval;
	m_pinA = pinA;
	m_pinB = pinB;
	m_clearing = clearing;		//if measured voltage changes (to the right direction) by more than this the state is swapped
	m_currentPosition = 0;
	//position
	m_voltageA = analogRead(m_pinA);
	m_voltageB = analogRead(m_pinB);
	if(m_voltageA > 530)
		m_aHigh = true;
	if(m_voltageB > 530)
			m_bHigh = true;


}

RotaryEncoder::~RotaryEncoder() {
}

void RotaryEncoder::update()
{

	short a, b;
	bool stateA = m_aHigh;
	bool stateB = m_bHigh;
	//read input
	a = analogRead(m_pinA);
	b = analogRead(m_pinB);
	//decide HIGH/low state
	//a
	if (stateA)
	{
		if (a < m_voltageA - m_clearing)
			stateA = false;
	}
	else if(a > m_voltageA + m_clearing)
	{
			stateA = true;
	}

	//b
	if (stateB)
	{
		if (b < m_voltageB - m_clearing)
			stateB = false;
	}
	else if(b > m_voltageB + m_clearing)
	{
			stateB = true;
	}
	//process => calculate direction/step
	//http://www.meinduino.de/arduino-grundlagen/2014/09/20/drehencoder1.html
	if(		(!m_aHigh && !m_bHigh && !stateA &&  stateB) ||
			(!m_aHigh &&  m_bHigh &&  stateA &&  stateB) ||
			( m_aHigh && !m_bHigh && !stateA && !stateB) ||
			( m_aHigh &&  m_bHigh &&  stateA && !stateB))
	{
		m_currentPosition++;
	}
	else if((!m_aHigh && !m_bHigh &&  stateA && !stateB) ||
			(!m_aHigh &&  m_bHigh && !stateA && !stateB) ||
			( m_aHigh && !m_bHigh &&  stateA &&  stateB) ||
			( m_aHigh &&  m_bHigh && !stateA &&  stateB))
	{
		m_currentPosition--;
	}
	//update Values
	m_aHigh = stateA;
	m_bHigh = stateB;

	if(stateA)
		m_voltageA = max(m_voltageA, a);
	else
		m_voltageA = min(m_voltageA, a);

	if(stateB)
		m_voltageB = max(m_voltageB, b);
	else
		m_voltageB = min(m_voltageB, b);
}

int RotaryEncoder::getCurrentPosition()
{
	return m_currentPosition;
}

void RotaryEncoder::setCurrentPosition(int pos)
{
	m_currentPosition = pos;
}

String RotaryEncoder::debug()
{
	return String(m_currentPosition);
	//return String(m_currentPosition) + "\t" + m_aHigh + "\t"  + m_bHigh + "\t" + m_voltageA + "\t" + m_voltageB;
}

String RotaryEncoder::debugHeader()
{
	return String(F("m_currentPosition"));
	//return String("m_currentPosition") + "\t" + "m_aHigh" + "\t"  + "m_bHigh" + "\t" + "m_voltageA" + "\t" + "m_voltageB";
}
