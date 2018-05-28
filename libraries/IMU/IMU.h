/*
 * IMU.h
 * Wrapper, that uses code from I2Cdev
*/

// I2Cdev library collection - MPU6050 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 8/24/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#ifndef LIBRARIES_IMU_H_
#define LIBRARIES_IMU_H_

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "CalLib.h"
#include "TimersClass.h"
#include "FilterOrientation.h"

// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
#define GyroMeasError PI * (40.0f / 180.0f)       // gyroscope measurement error in rads/s (shown as 3 deg/s)
#define GyroMeasDrift PI * (0.0f / 180.0f)      // gyroscope measurement drift in rad/s/s (shown as 0.0 deg/s/s)
// There is a tradeoff in the beta parameter between accuracy and response speed.
// In the original Madgwick study, beta of 0.041 (corresponding to GyroMeasError of 2.7 degrees/s) was found to give optimal accuracy.
// However, with this value, the LSM9SD0 response time is about 10 seconds to a stable initial quaternion.
// Subsequent changes also require a longish lag time to a stable output, not fast enough for a quadcopter or robot car!
// By increasing beta (GyroMeasError) by about a factor of fifteen, the response time constant is reduced to ~2 sec
// I haven't noticed any reduction in solution accuracy. This is essentially the I coefficient in a PID control sense;
// the bigger the feedback coefficient, the faster the solution converges, usually at the expense of accuracy.
// In any case, this is the free parameter in the Madgwick filtering and fusion scheme.
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f


class IMU : public TimerClass{
public:
	IMU(unsigned long interval);
	virtual ~IMU();
	void update();
	void debug(HardwareSerial& serial);
	void debugHeader(HardwareSerial& serial);
	void deleteCalibration();
	void enableCalibration();
	void disableCalibration();
	void setCurrentRotationAsRef();
	void setCalibrationOffset(float offset);
	void resetRotationRef();
	void setFilterFrequency(float freq);
	void getRPY(float &roll, float &pitch, float &yaw, float &filteredYaw);

private:
	// Declare device MPU6050 class
	MPU6050 *mpu;
	int16_t a1, a2, a3, g1, g2, g3, m1, m2, m3;     // raw data arrays reading
	uint16_t mcount; 								// used to control magnetometer rate
	uint8_t MagRateDivisor;     					// read rate for magnetometer data

	float deltat;        // integration interval for both filter schemes
	uint16_t lastUpdate; // used to calculate integration interval
	uint16_t now ;        // used to calculate integration interval

	float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values
	float q[4];    // vector to hold quaternion
	float eInt[3];       // vector to hold integral error for Mahony method
	float m_roll, m_pitch, m_yaw, m_filteredYaw;

	CALLIB_DATA m_calDat;
	unsigned long m_lastCalStore;

	void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
	void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
	void updateRPY();
	void quaternion_product(float *q1, float *q2, float *r); //x,y,z,w
	void quaternion_inverse(float *q, float *i); //x,y,z,w
	void quaternion_print(float *q);
	void normalize(float &angle);

	void initilizeCalibration();
	void storeCalibration();
	void setCalibrationData();

	float m_compassCalOffset[3];
	float m_compassCalScale[3];
	bool m_calibrationValid;
	bool m_enableCalibration;


	FilterOrientation *m_lowpassFilter;
};

#endif /* LIBRARIES_IMU_H_ */
