/*
 * PID.cpp
 * Angles in DEGREE (not radians as usual)
 * Angles positive CLOCKWISE (not counterclockwise as usual)
 *  Created on: 25.08.2015
 */

#include "KeypadWrapper.h"
#include "Arduino.h"

KeypadWrapper::KeypadWrapper(unsigned long interval) {
	m_interval = interval;

	m_keys[0][0] = '1';
	m_keys[0][1] = '2';
	m_keys[0][2] = '3';
	m_keys[0][3] = 'A';
	m_keys[1][0] = '4';
	m_keys[1][1] = '5';
	m_keys[1][2] = '6';
	m_keys[1][3] = 'B';
	m_keys[2][0] = '7';
	m_keys[2][1] = '8';
	m_keys[2][2] = '9';
	m_keys[2][3] = 'C';
	m_keys[3][0] = '*';
	m_keys[3][1] = '0';
	m_keys[3][2] = '#';
	m_keys[3][3] = 'D';


	m_rowPins[0] = A0;
	m_rowPins[1] = A1;
	m_rowPins[2] = A2;
	m_rowPins[3] = A3;

	m_colPins[0] = A4;
	m_colPins[1] = A5;
	m_colPins[2] = A6;
	m_colPins[3] = A7;

	m_kpd = new Keypad( makeKeymap(m_keys), m_rowPins, m_colPins, 4,  4);
	m_cmd = "";
}

KeypadWrapper::~KeypadWrapper() {
}

void KeypadWrapper::update()
{
	if (m_kpd->getKeys())
	{
		for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
		{
			if ( m_kpd->key[i].stateChanged )   // Only find keys that have changed state.
			{
				/*String msg;
				 switch (m_kpd->key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
					case PRESSED:
						msg = " PRESSED.";
					break;
						case HOLD:
						msg = " HOLD.";
					break;
						case RELEASED:
						msg = " RELEASED.";
					break;
						case IDLE:
						msg = " IDLE.";
					}
					Serial.print("Key ");
					Serial.print(m_kpd->key[i].kchar);
					Serial.println(msg);
*/
				if(m_kpd->key[i].kstate == HOLD)
				{
					switch(m_kpd->key[i].kchar){
						case 'C': m_cmd = "T";return;
						case 'D': m_cmd = "GP";return;
						default: break;
					}
				}
				if(m_kpd->key[i].kstate == RELEASED)
				{
					switch(m_kpd->key[i].kchar)
					{
						case '*':
						case '#': m_cmd = "S2";break;
						case 'D': m_cmd = "S";break;
						default:break;
					}
				}

				if(m_kpd->key[i].kstate == HOLD || m_kpd->key[i].kstate == PRESSED)
				{
					switch(m_kpd->key[i].kchar)
					{
						case '*':m_cmd = "D999";return;
						case '#':m_cmd = "I999";return;
						default: break;
					}

					if(m_kpd->key[i].kchar == 'A')
					{
						if(m_cmd.length() > 0)
						{
							if(m_cmd.charAt(1) == 'W')
							{
								m_cmd = m_cmd.substring(1);
								return;
							}
						}
						m_cmd = "-W";
					}
					else if(m_kpd->key[i].kchar == 'B')
					{
						if(m_cmd.length() > 0)
						{
							if(m_cmd.charAt(1) == 'M')
							{
								m_cmd = m_cmd.substring(1);
								return;
							}
						}
						m_cmd = "-M";
					}
					else
					{
						switch(m_kpd->key[i].kchar)
						{
						case '0': m_cmd += "0";break;
						case '1': m_cmd += "1";break;
						case '2': m_cmd += "2";break;
						case '3': m_cmd += "3";break;
						case '4': m_cmd += "4";break;
						case '5': m_cmd += "5";break;
						case '6': m_cmd += "6";break;
						case '7': m_cmd += "7";break;
						case '8': m_cmd += "8";break;
						case '9': m_cmd += "9";break;

						}
					}
				}
			}
		}
	}
}

String KeypadWrapper::getCommand()
{
	if(m_cmd.length() == 0)
	{
		return "";
	}
	if(m_cmd.charAt(0) != '-')
	{
		String cmd = m_cmd;
		m_cmd = "";
	//	Serial.println(cmd);
		return cmd;
	}
	else
	{
		return "";
	}
}

String KeypadWrapper::debug()
{
        return "";
}
String KeypadWrapper::debugHeader()
{
        return "";
}
