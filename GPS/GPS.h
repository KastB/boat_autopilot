/*
 * GPS.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_GPS_GPS_H_
#define LIBRARIES_GPS_GPS_H_

#include "TimersClass.h"

class GPS : public TimerClass {
public:
	//enum uiState{NORMAL, MENU, RADIO_CHOOSE, RADIO_SEND, PARAMETER_CHOOSE};

	GPS(unsigned long interval);
	virtual ~GPS();
	void update();
	String debug();
	String debugHeader();

private:
    float m_heading;
    float m_speed;
    String m_position;
    String m_new_position;

};

#endif /* LIBRARIES_GPS_GPS_H_ */
