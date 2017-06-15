#ifndef FilterOrientation_h
#define FilterOrientation_h
#include "Filters.h"
#include "Arduino.h"

class FilterOrientation {
public:
	FilterOrientation(FILTER_TYPE ft=LOWPASS, float fc=1.0, float initialValue=0);
	virtual ~FilterOrientation();

	void setToNewValue(float val);
	float input(float inVal );
	float output();
	void setFrequency(float newFrequency);

private:
	FilterOnePole *m_lowpassFilter;
};

#endif
