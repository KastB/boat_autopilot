/*
 * UI.cpp
 *
 *  Created on: 25.08.2015
 */

#include "UI.h"
#include "Arduino.h"
#include "Motor.h"
#include "RotaryEncoder.h"


UI::UI(unsigned long interval, IR* ir, Motor* motor, PID* pid, IMU* imu, KeypadWrapper *kpd, TimersClass *timer) {
	m_interval = interval;
	//m_uiState = NORMAL;
	m_ir = ir;
	m_motor = motor;
	m_pid = pid;
	m_imu = imu;
	m_kpd = kpd;
	m_timer = timer;

	m_cmdSerial = "";
	m_cmdKeypad = "";
	m_counter = 0;
	m_debugDevisor = 20;

	/*const byte ROWS = 4; //four rows
	const byte COLS = 4; //four columns
	char keys[ROWS][COLS] = {
		{'1','2','3', 'A'},
		{'4','5','6', 'B'},
		{'7','8','9', 'C'},
		{'*','0','#', 'D'}
	};
//	byte rowPins[ROWS] = {A0,A1,A2,A3}; //connect to the row pinouts of the kpd
//	byte colPins[COLS] = {A4,A5,A6,A7}; //connect to the column pinouts of the kpd

	byte rowPins[ROWS] = {53,51,49,47}; //connect to the row pinouts of the kpd
	byte colPins[COLS] = {45,43,41,39}; //connect to the column pinouts of the kpd


	m_kpd = new Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );*/

}

UI::~UI() {
	// TODO Auto-generated destructor stub
}

void UI::update()
{
	//Serial commands
	while(Serial.available() > 0)
	{
		char incoming = Serial.read();
		if(incoming != '\n' && incoming != ' ' && incoming != '\r')
		{
			m_cmdSerial += incoming;
		}
		else
		{
			exec(m_cmdSerial);
			m_cmdSerial = "";
		}
	}

	String irCmd = m_ir->getCommand();
	if(irCmd != "")
	{
		exec(irCmd);
		Serial.println(irCmd);
	}

	String kpdCmd = m_kpd->getCommand();
	if(kpdCmd != "")
	{
		exec(kpdCmd);
		Serial.println(kpdCmd);
	}

	if( m_counter  >= m_debugDevisor)
	{
		Serial.println(m_timer->debug());
		m_counter = 0;
	}
	m_counter++;
}
void UI::exec(String cmd)
{
	Serial.println(cmd);
	if(cmd == "?")
		msg_help();
	else if(cmd == "DELCAL")
		m_imu->deleteCalibration();
	else if(cmd == "SETROT")
	{
		m_imu->setCurrentRotationAsRef();
	}
	else if(cmd == "RESETROT")
	{
		m_imu->resetRotationRef();
	}
	else if(cmd == "T")
		m_pid->tack();
	else if(cmd == "GP")
	{
		m_pid->setInactiv();
		m_motor->gotoParking();
	}
	else if(cmd == "I")
	{
		m_pid->setInactiv();
		m_motor->initialize();
	}
	else if(cmd == "S")
	{
		m_pid->setInactiv();
		m_motor->stop();
	}
	else if(cmd == "S2")
	{
		m_motor->stop();
	}
	else if(cmd.length() > 1)
	{
		if(cmd.charAt(0) == 'P') //Parameter
		{
			String val = cmd.substring(2);
			float value = val.toFloat();

			switch (cmd.charAt(1))
			{
				case 'P': m_pid->m_P = value;break;
				case 'I': m_pid->m_errorSum = m_pid->m_errorSum / m_pid->m_I * value;m_pid->m_I = value;break;
				case 'D': m_pid->m_D = value;break;
				case 'M': m_motor->gotoPos(value);break;
				case 'R': setDebugDevisor(value);break;
				default: break;
			}
		}
		else if(cmd.charAt(0) == 'M') //Magnet goal
		{
			String val = cmd.substring(1);
			float value = val.toFloat();
			m_pid->setMag(value);
		}
		else if(cmd.charAt(0) == 'W') //Wind goal
		{
			String val = cmd.substring(1);
			float value = val.toFloat();
			m_pid->setWind(value);
		}
		else if(cmd.charAt(0) == 'I') //Increase
		{
			String val = cmd.substring(1);
			float value = val.toFloat();
			m_pid->increase(value);
		}
		else if(cmd.charAt(0) == 'D') //Decrease
		{
			String val = cmd.substring(1);
			float value = val.toFloat();
			m_pid->decrease(value);
		}


	}
}

void UI::setDebugDevisor(int div)
{
	if( div >= 1 && div < 10000)
	{
		m_debugDevisor = div;
	}
}

void UI::msg_help()
{
	String msg =String("?: this help\n") +
						"IR: delete IMU calibration\n" +
						"T: tack\n" +
						"GP: go parking\n" +
						"M???: set magnet goal\n" +
						"W???: set Wind goal\n" +
						"I???: increase\n" +
						"D???: decrease\n" +
						"R[A-D]: Program Radio-key[A-D]\n" +
						"P[P,I,D,M,R][-DBL_MAX;DBL_MAX]: set Parameter controller (PID-Control-Parameters, Motorposition, debug Rate inverse";
	Serial.println(msg);
//Serial1.println(msg);
}

String UI::debug()
{
	return "";
}

String UI::debugHeader()
{
	return "";
}
