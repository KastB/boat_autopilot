/*
 * UI.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_UI_UI_H_
#define LIBRARIES_UI_UI_H_
#include <PID.h>
#include <Motor.h>
//#include <Radio.h>
#include <IR.h>
#include "TimersClass.h"
#include <IMU.h>
#include "KeypadWrapper.h"

class UI : public TimerClass {
public:

	UI(unsigned long interval, IR* ir, Motor* motor, PID* pid, IMU* imu, KeypadWrapper *kpd, TimersClass *timer);
	virtual ~UI();
	void update();
	String debug();
	String debugHeader();

private:
	void msg_help();

	void exec(String cmd);
	void setDebugDevisor(int nterval);

	String m_cmdSerial;
	String m_cmdKeypad;
	int m_debugDevisor;
	int m_counter;


	IR 		*m_ir;
	Motor 		*m_motor;
	PID 		*m_pid;
	IMU			*m_imu;
	KeypadWrapper *m_kpd;
	TimersClass *m_timer;

};

#endif /* LIBRARIES_UI_UI_H_ */
