/*
 * PID.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_KEYPADWRAPPER_H_
#define LIBRARIES_KEYPADWRAPPER_H_

#include "TimersClass.h"
#include <Keypad.h>


class KeypadWrapper : public TimerClass {
public:
	KeypadWrapper(unsigned long interval);
	virtual ~KeypadWrapper();
	String debug();
	String debugHeader();
	void update();
	String getCommand();

private:
    Keypad *m_kpd;
    String m_cmd;
    char m_keys[4][4];
	byte m_rowPins[4];
	byte m_colPins[4];
};

#endif /* LIBRARIES_KEYPADWRAPPER_H_ */
