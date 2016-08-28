#include "TimersClass.h"

TimerClass::TimerClass()
{
	m_interval = 100;
	m_timestamp = 0;
}
TimerClass::~TimerClass()
{

}

TimersClass::TimersClass()
{
	for(int i = 0; i < MAXTIMERCLASSES; i++)
	{
		m_timers[i] = NULL;
	}
}
TimersClass::~TimersClass()
{

}

void TimersClass::checkAndRunUpdate()
{
	for(int i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++)
	{
		m_timers[i]->checkAndRunUpdate();
	}
}
String TimersClass::debug()
{
	String tmp(millis());
	tmp += "\t";
	for(int i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++)
	{
		tmp += m_timers[i]->debug() + "\t";
	}
	return tmp;
}
String TimersClass::debugHeader()
{
	String tmp("Millis\t");
	for(int i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++)
	{
		tmp += m_timers[i]->debugHeader() + "\t";
	}
	return tmp;
}
void TimersClass::addTimer(TimerClass* timer)
{
	int i;
	for(i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++);
	if (i == MAXTIMERCLASSES)
		return;
	timer->m_timestamp = millis();
	if(m_timers[i] == NULL)
		m_timers[i] = timer;
}


