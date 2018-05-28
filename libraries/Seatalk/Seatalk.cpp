#include "Seatalk.h"
#include "Arduino.h"

// True wind: $XXMWV,$WIMWV1+acos((sqr($WIMWV3)+sqr($GPRMC7)+sqr($WIMWV3)-2*$GPRMC7*$WIMWV3*cos($WIMWV1*3.14159265/180)-sqr($GPRMC7))/(2*sqrt(sqr($GPRMC7)+sqr($WIMWV3)-2*$GPRMC7*cos($WIMWV1*3.14159265/180))*$WIMWV3)),T,sqrt(sqr($GPRMC7)+SQR($WIMWV3)-2*$GPRMC7*$WIMWV3*cos($WIMWV1*3.14159265/180)),N,A

Seatalk::Seatalk(unsigned long interval) {
	m_interval = interval;

	m_speed.speed = -1;
	m_speed.tripMileage = -1;
	m_speed.totalMileage = -1;
	m_speed.waterTemp = -1;
	m_lampIntensity = -1;

	m_rawReadCount = 0;

	m_depth.anchorAlarm = false;
	m_depth.deepAlarm = false;
	m_depth.defective = false;
	m_depth.depthBelowTransductor = 0.0;
	m_depth.metricUnits = false;
	m_depth.shallowAlarm = false;
	m_depth.unknown = false;

	m_debug = false;
}

Seatalk::~Seatalk() {
}


void Seatalk::update()
{
	short expectedMsgLength = -1;
	if(!Serial2.available()) //no Data since last check => next time a new message begins
	{
		m_rawReadCount = 0;
		m_rawMessage[0] = 0;
		m_rawMessage[1] = 0;
		m_rawMessage[2] = 0;
		m_rawMessage[3] = 0;
		m_rawMessage[4] = 0;
		m_rawMessage[5] = 0;
	}
	while(Serial2.available()) // There is something to be read
	{
		m_rawMessage[m_rawReadCount] = Serial2.read();
		m_rawReadCount++;
		if(m_rawReadCount > 2) //minimum length == 3
		{
			//determine expected length of current message

			expectedMsgLength = (m_rawMessage[1]&0x0F);
			if( expectedMessageLength(m_rawMessage[0]) == expectedMsgLength)	//parse
			{
				//everything is ok
				parseMessage();
			}
			else																//print and debug
			{
				if(expectedMsgLength > 15)
				{
					expectedMsgLength = 0;
				}
				if (m_debug)
				{
					if(expectedMsgLength > 15)
					{
						Serial.println("Message is corrupt");
					}
					else if(expectedMessageLength(m_rawMessage[0]) == -1)
					{
						Serial.println("New Message Type received or Message corrupt");
					}
					else if(expectedMessageLength(m_rawMessage[0]) != -1 )
					{
						Serial.println("Perhaps there is some coding in that message");
					}
					if(m_rawReadCount == 3)
					{
						Serial.print("Seatalk:0:");
						Serial.println(m_rawMessage[0],HEX);
						Serial.print("Seatalk:1:");
						Serial.println(m_rawMessage[1],HEX);
					}
					Serial.print("Seatalk:");
					Serial.print(m_rawReadCount-1);
					Serial.print(":");
					Serial.println(m_rawMessage[m_rawReadCount-1],HEX);
				}
			}

			if(m_rawReadCount == expectedMsgLength || m_rawReadCount == 18) // all needed Data received => process message
			{
				m_rawReadCount = 0;
			}
		}
	}
}

short Seatalk::expectedMessageLength(short msgID)
{
	switch(msgID)
	{
		case 0x00:return 2;break;
		case 0x10:return 1;break;
		case 0x11:return 1;break;
		case 0x20:return 1;break;
		case 0x21:return 2;break;
		case 0x24:return 2;break;
		case 0x22:return 2;break;
		case 0x23:return 1;break;
		case 0x30:return 0;break;
		default: return -1;
	}
}

void Seatalk::parseMessage()
{
	switch(m_rawMessage[0])
	{
	/*
		00  02  YZ  XX XX  Depth below transducer: XXXX/10 feet
		Flags in Y: Y&8 = 8: Anchor Alarm is active
				  Y&4 = 4: Metric display units or
						   Fathom display units if followed by command 65
				  Y&2 = 2: Used, unknown meaning
		Flags in Z: Z&4 = 4: Transducer defective
				  Z&2 = 2: Deep Alarm is active
				  Z&1 = 1: Shallow Depth Alarm is active
		Corresponding NMEA sentences: DPT, DBT
	*/
		case 0x00:
			m_depth.anchorAlarm = m_rawMessage[2] & 0b10000000;
			m_depth.metricUnits = m_rawMessage[2] & 0b01000000;
			m_depth.unknown		= m_rawMessage[2] & 0b00100000;
			m_depth.defective	= m_rawMessage[2] & 0b00000100;
			m_depth.deepAlarm	= m_rawMessage[2] & 0b00000010;
			m_depth.shallowAlarm= m_rawMessage[2] & 0b00000001;
			m_depth.depthBelowTransductor = ((float)m_rawMessage[4] * 256 + m_rawMessage[3]) * 0.3048 / 10.0f;
			break;
	/*
		10  01  XX  YY  Apparent Wind Angle: XXYY/2 degrees right of bow
						 Used for autopilots Vane Mode (WindTrim)
						 Corresponding NMEA sentence: MWV
	*/
		case 0x10:
			m_wind.apparentAngle = ((float)m_rawMessage[2] * 256 + m_rawMessage[3]) / 2.0;
			m_wind.apparentAngleFiltered->input(m_wind.apparentAngle);
			break;
	/*
		11  01  XX  0Y  Apparent Wind Speed: (XX & 0x7F) + Y/10 Knots
						 Units flag: XX&0x80=0    => Display value in Knots
									 XX&0x80=0x80 => Display value in Meter/Second
						 Corresponding NMEA sentence: MWV
	 */
		case 0x11:
			m_wind.apparentSpeed = ((float)(m_rawMessage[2] & 0x7F)) + (m_rawMessage[3] & 0x0F) / 10.0;
			break;
	/*
		 20  01  XX  XX  Speed through water: XXXX/10 Knots
						 Corresponding NMEA sentence: VHW
	 */
		case 0x20:
			m_speed.speed = ((float)m_rawMessage[3] * 256 + m_rawMessage[2])/ 10.0f;
			break;
	/*
	 *  21  02  XX  XX  0X    Mileage: XXXXX/100 nautical miles
	 */
		case 0x21:
	/*		Serial.println("0x21");
//case 0x24:
			Serial.println("mileage");
			Serial.println(m_rawMessage[4]);
			Serial.println(m_rawMessage[3]);
			Serial.println(m_rawMessage[2]);

			Serial.println(m_rawMessage[5], BIN);
			Serial.println(m_rawMessage[4], BIN);
			Serial.println(m_rawMessage[3], BIN);
			Serial.println(m_rawMessage[2], BIN);*/
			m_speed.tripMileage = ((float)(m_rawMessage[4] & 0x0F) * (2^16) + m_rawMessage[3] * (2^8) + m_rawMessage[2] )/ 100.0f;
			break;
	/*
	 *  22  02  XX  XX  00  Total Mileage: XXXX/10 nautical miles
	 */
		case 0x22:
	/*		Serial.println("totalmileage");
			Serial.println(m_rawMessage[3]);
			Serial.println(m_rawMessage[2]);

			Serial.println(m_rawMessage[5], BIN);
			Serial.println(m_rawMessage[4], BIN);
			Serial.println(m_rawMessage[3], BIN);
			Serial.println(m_rawMessage[2], BIN);*/
			m_speed.totalMileage = m_speed.tripMileage = ((float)(m_rawMessage[4] & 0x0F) * (2^16) + m_rawMessage[3] * (2^8) + m_rawMessage[2])/ 10.0f;
			break;
	/*
	 *  23  Z1  XX  YY  Water temperature (ST50): XX deg Celsius, YY deg Fahrenheit
                 Flag Z&4: Sensor defective or not connected (Z=4)
                 Corresponding NMEA sentence: MTW
	 */
		case 0x23:
			m_speed.waterTemp = m_rawMessage[2];
			break;
	/*
	*	 24  02  00  00  XX  Display units for Mileage & Speed 
                     XX: 00=nm/knots, 06=sm/mph, 86=km/kmh
	*/
		case 0x24:
			break;

	/*
	 *  30  00  0X      Set lamp Intensity; X=0: L0, X=4: L1, X=8: L2, X=C: L3
                     (only sent once when setting the lamp intensity)
	 */
		case 0x30:
			switch(m_rawMessage[2])
			{
				case 0x00: m_lampIntensity = 0;break;
				case 0x04: m_lampIntensity = 1;break;
				case 0x08: m_lampIntensity = 2;break;
				case 0x0C: m_lampIntensity = 3;break;
			}
			break;
	}
}
String Seatalk::debug()
{
	return String(m_speed.speed) + "\t" +
	m_speed.tripMileage + "\t" +
	m_speed.totalMileage + "\t" +
	m_speed.waterTemp + "\t" +
	m_lampIntensity + "\t" +

	m_wind.apparentAngle + "\t" +
	m_wind.apparentSpeed + "\t" +
	m_wind.displayInKnots + "\t" +
	m_wind.displayInMpS + "\t" +

	m_depth.anchorAlarm  + "\t" +
	m_depth.deepAlarm  + "\t" +
	m_depth.defective  + "\t" +
	m_depth.depthBelowTransductor  + "\t" +
	m_depth.metricUnits  + "\t" +
	m_depth.shallowAlarm  + "\t" +
	m_depth.unknown;
}

String Seatalk::debugHeader()
{
	return F("m_speed\tm_speed.tripMileage\tm_speed.totalMileage\tm_speed.waterTemp\tm_lampIntensity\tm_wind.apparentAngle\tm_wind.apparentSpeed\tm_wind.displayInKnots\tm_wind.displayInMpS\tm_depth.anchorAlarm\tm_depth.deepAlarm\tm_depth.defective\tm_depth.depthBelowTransductor\tm_depth.metricUnits\tm_depth.shallowAlarm\tm_depth.unknown");
}
