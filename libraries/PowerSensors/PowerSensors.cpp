#include "Arduino.h"
#include "PowerSensors.h"

PowerSensors::PowerSensors(unsigned long interval, int voltagePin, int currentPin) {
	m_interval = interval;
	
	m_voltagePin = voltagePin;
	m_currentPin = currentPin;

	m_current = -1.0;
	m_voltage = -1.0;

	// create a one pole (RC) lowpass filter
	m_lowpassOutput = new FilterOnePole ( LOWPASS, 1.0 );
}

PowerSensors::~PowerSensors() {}

void PowerSensors::setFilterFrequency(float freq) {
	m_lowpassOutput->setFrequency(freq);
}

void PowerSensors::update() {
	float voltage = float(analogRead(m_voltagePin)) * 12.0f * 5.0f / 4.3f / 1023.0f;
	if (voltage > m_voltage / 2.0f)
		m_voltage = voltage;
	m_current = (float(analogRead(m_currentPin)) / 1023.0f * 5.0f - 2.5f) * 30.0f / 2.5f;
	m_lowpassOutput->input(m_voltage*m_current);
}

void PowerSensors::debug(HardwareSerial &serial) {
  char spacer = ',';
  serial.print(m_voltage);
  serial.print(spacer);
  serial.print(m_current);
  serial.print(spacer);
  serial.print(m_lowpassOutput->output());
}

void PowerSensors::debugHeader(HardwareSerial &serial) {
  serial.print("m_voltage,m_current,m_power,");
}
