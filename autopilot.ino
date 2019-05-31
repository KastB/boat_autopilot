//TODO: jumps in yaw sometimes => imperfect behaviour of pid
//TODO: maybe smooth motor
//TODO: imu gets nans and keeps them =>either prevent or deal with them

//TODO: bluetooth mode doesnt work anymore
//TODO: log file
//TODO: write readme: high frequencies => exceptions with public intentbroadcast
//TODO: test new communication scheme
#include <IR.h>
#include <Radio.h>
#include <RotaryEncoder.h>
#include <TimersClass.h>
#include <UI.h>
#include "Arduino.h"

#include <GPS.h>
#include <IMU.h>
#include <Motor.h>
#include <PID.h>
#include <Seatalk.h>
#include <PowerSensors.h>

#if SERIAL_TX_BUFFER_SIZE < 512
{ "Error: TX_Buffersize too small: latencies might occur"
  "Add -DSERIAL_TX_BUFFER_SIZE=512 to the build parameters"}
#endif

// Connection Handlers

TimersClass g_timer;
Radio g_radio = Radio(50, 0, 10);
//IR g_ir =	IR(50, 2);
RotaryEncoder g_rotaryEncoder = RotaryEncoder(2, 9, 8);
Motor g_motor = Motor(50, &g_rotaryEncoder);
IMU	g_imu = IMU(10); // 7-11ms, every 10th 27 when getMagBlocking is used
Seatalk g_seatalk = Seatalk(5); //needs to be called with rather high frequency in order to detect corrupt messages without 9-bit mode (without using command bit)
GPS g_gps = GPS(10, &Serial);
PID g_pid = PID(500, &g_imu, & g_seatalk, &g_motor);
KeypadWrapper	g_keypadWrapper	=	KeypadWrapper	(25); // vermutlich kürzer als 2ms => 16 analog reads
UI g_ui = UI(50, NULL, &g_radio, &g_motor,&g_pid, &g_imu, &g_keypadWrapper, &g_timer); //~8ms
PowerSensors g_power = PowerSensors(100, 13, 15);

void setup() {
  //=> Frequency: 16 000 000/2/1/800=10 000 on Pins 6,7,8 with phase-correct
  //mode
  ICR4 = 800;  // PWM count to 800
  // Phase and Frequency correct => three last bits of 1001
  TCCR4A = (TCCR4A & 0b11111100) | 0b10;
  // First bit of 1001 and 010 for divisor of 1
  TCCR4B = (TCCR4B & 0b11100000) | 0b10001;

  Serial.begin(115200);  // PC
  Serial1.begin(9600);   // GPS
  Serial2.begin(4800);   // Seatalk

  // Timers
  g_timer.addTimer(&g_rotaryEncoder);
  g_timer.addTimer(&g_radio);
  //	g_timer.addTimer(&g_ir);
  g_timer.addTimer(&g_motor);
  g_timer.addTimer(&g_pid);
  g_timer.addTimer(&g_ui);
  g_timer.addTimer(&g_imu);
  g_timer.addTimer(&g_seatalk);
  g_timer.addTimer(&g_gps);
  g_timer.addTimer(&g_power);

  // Headers
  g_timer.debugHeader(Serial);
  while (Serial.available()) {
    Serial.read();
  }
}

void loop() { g_timer.checkAndRunUpdate(); }
