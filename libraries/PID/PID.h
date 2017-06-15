/*
 * PID.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_PID_PID_H_
#define LIBRARIES_PID_PID_H_

#include "TimersClass.h"
#include <IMU.h>
#include <Seatalk.h>
#include <Motor.h>


class PID : public TimerClass {
public:
	PID(unsigned long interval, IMU *imu, Seatalk *seatalk, Motor *motor);
	virtual ~PID();
	String debug();
	String debugHeader();
	void update();

	void setWind(); 			//current
	void setWind(float goal);	//with goal

	void setMag(); 				//current
	void setMag(float goal);	//with goal

	void tack();

	void toggleState();
	void setInactiv();
	void increase(int value);
	void decrease(int value);

	void setFilterFrequency(float freq);
private:
	enum goalType{INACTIVE, WIND, MAGNET};
	//controller
	float m_P;
	float m_P2;
	float m_I;
	float m_D;

	float m_settled;

	float m_errorSum;
	float m_lastError;
	float m_lastFilteredYaw;
	unsigned long m_lastTime;

	unsigned long m_iNoUpdateDelay;		// delay after goal change before I is updated again
	unsigned long m_InoUpdate;			// no update of I till before this time
	float m_rotVelDyn;					// no update of I when boat rotates faster than this speed (°/s)

	FilterOnePole *m_lowpassOutput;

	unsigned long m_tackStartTime;
	unsigned long m_tackMinTime;	/** minimal time between two tacks*/

	float m_goal;
	enum goalType m_goalType;


	IMU *m_imu;
	Motor *m_motor;
	Seatalk *m_seatalk;



	void normalize(float &error);

	friend class UI;
};

#endif /* LIBRARIES_PID_PID_H_ */
