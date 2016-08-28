/*
 * Radio.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_TIMER_CLASS_H_
#define LIBRARIES_TIMER_CLASS_H_
#include <string.h>
#include "Arduino.h"
#define MAXTIMERCLASSES 20

class TimerClass {
public:
	TimerClass();
	virtual ~TimerClass() = 0;
	void checkAndRunUpdate()
	{
		if(millis() >= m_timestamp + m_interval)
		{
			m_timestamp = millis();
			update();
		}
	}
	virtual void update() = 0;
	virtual String debug() = 0;
	virtual String debugHeader() = 0;
	unsigned long m_interval;
private:
	unsigned long m_timestamp;

	friend class TimersClass;
};

class TimersClass{
public:
	TimersClass();
	virtual ~TimersClass();
	void checkAndRunUpdate();
	String debug();
	String debugHeader();
	void addTimer(TimerClass* timer);
private:
	TimerClass* m_timers[MAXTIMERCLASSES];
};

#endif /* LIBRARIES_TIMER_CLASS_H_ */
