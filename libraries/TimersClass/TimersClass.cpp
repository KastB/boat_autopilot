#include "TimersClass.h"

TimerClass::TimerClass() {
  m_interval = 100;
  m_timestamp = 0;
}
TimerClass::~TimerClass() {}

TimersClass::TimersClass() {
  for (int i = 0; i < MAXTIMERCLASSES; i++) {
    m_timers[i] = NULL;
  }
}
TimersClass::~TimersClass() {}

void TimersClass::checkAndRunUpdate() {
  unsigned long start = 0;
  for (int i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++) {
	start = millis();
    m_timers[i]->checkAndRunUpdate();

/*
    if(millis() - start > 5)
    {
    	Serial.print(i);
    	Serial.print("##");
    	Serial.println(millis() - start);
    }*/
  }

}
void TimersClass::debug(HardwareSerial& serial) {
  serial.print(millis());
  for (int i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++) {
    serial.print("\t");
    m_timers[i]->debug(serial);
  }
  serial.println("");
}
void TimersClass::debugHeader(HardwareSerial& serial) {
  serial.print(F("Millis\t"));
  for (int i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++) {
    m_timers[i]->debugHeader(serial);
    serial.print("\t");
  }
  serial.println("");
}
void TimersClass::addTimer(TimerClass* timer) {
  int i;
  for (i = 0; i < MAXTIMERCLASSES && m_timers[i] != NULL; i++)
    ;
  if (i == MAXTIMERCLASSES) return;
  timer->m_timestamp = millis() + timer->m_interval;
  if (m_timers[i] == NULL) m_timers[i] = timer;
}
