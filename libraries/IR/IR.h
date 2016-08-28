/*
 * PID.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_IR_H_
#define LIBRARIES_IR_H_

#include "TimersClass.h"
#include <IRremote.h>



class IR : public TimerClass {
public:
	IR(unsigned long interval, int pin);
	virtual ~IR();
	String debug();
	String debugHeader();
	void update();
	String getCommand();
private:
	enum keys : unsigned long{
		HOLD	= 4294967295,

		LEFT	= 16615543,
		RIGHT	= 16599223,

		TACK	= 16582903,
		GP		= 16580863,

		MAG		= 16623703,
		WIND	= 16607383,

		ZERO	= 16593103,
		ONE		= 16597183,
		TWO		= 16629823,
		THREE	= 16589023,
		FOUR	= 16621663,
		FIVE	= 16605343,
		SIX		= 16637983,
		SEVEN	= 16584943,
		EIGHT	= 16617583,
		NINE	= 16601263
	};

    String m_cmd;

    bool m_initialized;
    bool m_hold;
    unsigned long m_lastReceived;
	unsigned long m_keyReleasedTimeout;
	unsigned long m_keyHoldTimeout;
	unsigned long m_keyHoldTime;

	IRrecv  *m_irrecv;
	unsigned long m_lastKey;
};

#endif /* LIBRARIES_IR_H_ */
