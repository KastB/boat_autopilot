/*
 * Seatalk.h
 *
 *  Created on: 25.08.2015
 *  visit http://www.thomasknauf.de/seatalk.htm for more information
 *
 *      ONLY READ - NO SEND OF DATA (Standard Library can't handle 9bit protocol)
 *      9th bit isn't respected => always 0 but the first byte => workaround: start of new message if old message is finished, or timeout is reached
 *
 *

 00  02  YZ  XX XX  Depth below transducer: XXXX/10 feet
                       Flags in Y: Y&8 = 8: Anchor Alarm is active
                                  Y&4 = 4: Metric display units or
                                           Fathom display units if followed by command 65
                                  Y&2 = 2: Used, unknown meaning
                      Flags in Z: Z&4 = 4: Transducer defective
                                  Z&2 = 2: Deep Alarm is active
                                  Z&1 = 1: Shallow Depth Alarm is active
                    Corresponding NMEA sentences: DPT, DBT


10  01  XX  YY  Apparent Wind Angle: XXYY/2 degrees right of bow
                 Used for autopilots Vane Mode (WindTrim)
                 Corresponding NMEA sentence: MWV

 11  01  XX  0Y  Apparent Wind Speed: (XX & 0x7F) + Y/10 Knots
                 Units flag: XX&0x80=0    => Display value in Knots
                             XX&0x80=0x80 => Display value in Meter/Second
                 Corresponding NMEA sentence: MWV

 20  01  XX  XX  Speed through water: XXXX/10 Knots
                 Corresponding NMEA sentence: VHW

 21  02  XX  XX  0X  Trip Mileage: XXXXX/100 nautical miles

 22  02  XX  XX  00  Total Mileage: XXXX/10 nautical miles

 23  Z1  XX  YY  Water temperature (ST50): XX deg Celsius, YY deg Fahrenheit
                 Flag Z&4: Sensor defective or not connected (Z=4)
                 Corresponding NMEA sentence: MTW

 30  00  0X      Set lamp Intensity; X=0: L0, X=4: L1, X=8: L2, X=C: L3
                     (only sent once when setting the lamp intensity)
 */

#ifndef LIBRARIES_SEATALK_H_
#define LIBRARIES_SEATALK_H_
#include "TimersClass.h"
#include "FilterOrientation.h"

class Seatalk : public TimerClass{
public:
	struct depth{
	 	 bool anchorAlarm;
	 	 bool metricUnits;
	 	 bool unknown;
	 	 bool defective;
	 	 bool deepAlarm;
	 	 bool shallowAlarm;
	 	 float depthBelowTransductor;
	 };
	struct wind{
		float apparentAngle;	//PI +=ccw
		float apparentSpeed;	//Knots
		bool displayInKnots;
		bool displayInMpS;
		FilterOrientation *apparentAngleFiltered;
		wind()
		{
			apparentAngle = 0.0f;
			apparentSpeed = 0.0f;
			displayInKnots = true;
			displayInMpS = false;
			apparentAngleFiltered = new FilterOrientation(LOWPASS, 1.0f);
		}
		~wind()
		{
			delete (apparentAngleFiltered);
		}
	};
	struct speed{
		float speed;
		float tripMileage;
		float totalMileage;
		float waterTemp;
	};

	Seatalk(unsigned long interval);
	virtual ~Seatalk();
	String debug();
	String debugHeader();
	void update();

	bool m_debug;

	wind m_wind;
	depth m_depth;
	speed m_speed;
	short m_lampIntensity;
private:
	short m_rawMessage[18];
	short m_rawReadCount;

	short expectedMessageLength(short msgID);
	void parseMessage();
};

#endif /* LIBRARIES_SEATALK_H_ */
