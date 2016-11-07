#include "Arduino.h"
#include <IR.h>
#include <TimersClass.h>
#include <UI.h>
#include <RotaryEncoder.h>
#include <Radio.h>

#include <Motor.h>
#include <PID.h>
#include <IMU.h>
#include <Seatalk.h>
#include <GPS.h>


//Connection Handlers

TimersClass 	g_timer;
Radio 			g_radio 		= 	Radio			(50, 0,10);
//IR				g_ir			=	IR				(50, 2);
RotaryEncoder 	g_rotaryEncoder	=	RotaryEncoder	(10, 9,8);
Motor 			g_motor			= 	Motor			(50, &g_rotaryEncoder);
IMU				g_imu 			= 	IMU				(20);
Seatalk			g_seatalk		= 	Seatalk			(20);				//needs to be called with correct frequency in order to detect corrupt messages without 9-bit mode (without using command bit)
GPS				g_gps			=	GPS				(10);
PID 			g_pid 			= 	PID				(500, &g_imu, & g_seatalk, &g_motor);
KeypadWrapper	g_keypadWrapper	=	KeypadWrapper	(25);
UI 				g_ui 			= 	UI				(50, NULL, &g_radio, &g_motor,&g_pid, &g_imu, &g_keypadWrapper, &g_timer);



void setup()
{
	//=> Frequency: 16 000 000/2/1/800=10 000 on Pins 6,7,8 with phase-correct mode
	ICR4 = 800; //PWM count to 800
	TCCR4A = (TCCR4A  & 0b11111100) | 0b10; 	// Phase and Frequency correct => three last bits of 1001
	TCCR4B = (TCCR4B  & 0b11100000) | 0b10001;	// First bit of 1001 and 010 for divisor of 1

	Serial.begin(115200);					//PC
	Serial1.begin(9600);					//GPS
	Serial2.begin(4800);					//Seatalk

	//Timers
	g_timer.addTimer(&g_rotaryEncoder);
	g_timer.addTimer(&g_radio);
//	g_timer.addTimer(&g_ir);
	g_timer.addTimer(&g_motor);
	g_timer.addTimer(&g_pid);
	g_timer.addTimer(&g_ui);
	g_timer.addTimer(&g_imu);
	g_timer.addTimer(&g_seatalk);
	g_timer.addTimer(&g_gps);

	//Headers
	Serial.println(g_timer.debugHeader());
	while(Serial.available())
	{
		Serial.read();
	}

}

void loop()
{
	g_timer.checkAndRunUpdate();
}
