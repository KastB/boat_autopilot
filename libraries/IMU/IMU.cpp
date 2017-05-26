#include "IMU.h"

IMU::IMU(unsigned long interval)
{
	m_interval = interval;
	mpu = new MPU6050(0x69);
	a1=a2=a3=g1=g2=g3=m1=m2=m3=0;
	mcount = 0;
	deltat = 0.0f;
	lastUpdate=now=0;

	ax = ay = az = gx = gy = gz = mx = my = mz = 0;
	q[0] = 1.0f;
	eInt[0] = eInt[1] = eInt[2] = q[1] = q[2] = q[3] = 0.0f;

	// filters out changes faster that 0.03 Hz.
	float filterFrequency = 0.03;

	// create a one pole (RC) lowpass filter
	m_lowpassFilter = new FilterOnePole ( LOWPASS, filterFrequency );


	Wire.begin();
	// initialize MPU6050 device

	mpu->initialize();


	// Set up the accelerometer, gyro, and magnetometer for data output
	mpu->setRate(7); // set gyro rate to 8 kHz/(1 * rate) shows 1 kHz, accelerometer ODR is fixed at 1 KHz

	MagRateDivisor = 10; // 1/divisor of accelRate

	// Digital low pass filter configuration.
	// It also determines the internal sampling rate used by the device as shown in the table below.
	// The accelerometer output rate is fixed at 1kHz. This means that for a Sample
	// Rate greater than 1kHz, the same accelerometer sample may be output to the
	// FIFO, DMP, and sensor registers more than once.
	/*
	*          |   ACCELEROMETER    |           GYROSCOPE
	* DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
	* ---------+-----------+--------+-----------+--------+-------------
	* 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
	* 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
	* 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
	* 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
	* 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
	* 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
	* 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
	*/
	mpu->setDLPFMode(4); // set bandwidth of both gyro and accelerometer to ~20 Hz

	// Full-scale range of the gyro sensors:
	// 0 = +/- 250 degrees/sec, 1 = +/- 500 degrees/sec, 2 = +/- 1000 degrees/sec, 3 = +/- 2000 degrees/sec
	mpu->setFullScaleGyroRange(0); // set gyro range to 250 degrees/sec

	// Full-scale accelerometer range.
	// The full-scale range of the accelerometer: 0 = +/- 2g, 1 = +/- 4g, 2 = +/- 8g, 3 = +/- 16g
	mpu->setFullScaleAccelRange(0); // set accelerometer to 2 g range

	mpu->setIntDataReadyEnabled(true); // enable data ready interrupt

	calLibRead(0, &m_calDat);                           // pick up existing mag data if there
	if(!m_calDat.magValid)
		initilizeCalibration();
	else
		setCalibrationData();

	m_lastCalStore = 0;
}

IMU::~IMU() {
	delete (mpu);
}

void IMU::setFilterFrequency(float freq)
{
	m_lowpassFilter->setFrequency(freq);
}

void IMU::update()
{
	if(mpu->getIntDataReadyStatus() == 1) // wait for data ready status register to update all data registers
	{
		mcount++;
	   // read the raw sensor data
		mpu->getAcceleration  ( &a1, &a2, &a3  );
		ax = a1*2.0f/32768.0f; // 2 g full range for accelerometer
		ay = a2*2.0f/32768.0f;
		az = a3*2.0f/32768.0f;

		mpu->getRotation  ( &g1, &g2, &g3  );
		gx = g1*250.0f/32768.0f; // 250 deg/s full range for gyroscope
		gy = g2*250.0f/32768.0f;
		gz = g3*250.0f/32768.0f;
	/*  The gyros and accelerometers can in principle be calibrated in addition to any factory calibration but they are generally
	 *  pretty accurate. You can check the accelerometer by making sure the reading is +1 g in the positive direction for each axis.
	 *  The gyro should read zero for each axis when the sensor is at rest. Small or zero adjustment should be needed for these sensors.
	 *  The magnetometer is a different thing. Most magnetometers will be sensitive to circuit currents, computers, and
	 *
	 *  other both man-made and natural sources of magnetic field. The rough way to calibrate the magnetometer is to record
	 *  the maximum and minimum readings (generally achieved at the North magnetic direction). The average of the sum divided by two
	 *	should provide a pretty good calibration offset. Don't forget that for the MPU9150, the magnetometer x- and y-axes are switched
	 *  compared to the gyro and accelerometer!
	*/

		if (mcount > MagRateDivisor){  // this is a poor man's way of setting the magnetometer read rate (see below)
			mpu->getMag  ( &m1, &m2, &m3 );
		/*	mx = m1*10.0f*1229.0f/4096.0f; // milliGauss (1229 microTesla per 2^12 bits, 10 mG per microTesla)
			my = m2*10.0f*1229.0f/4096.0f; // apply calibration offsets in mG that correspond to your environment and magnetometer
			mz = m3*10.0f*1229.0f/4096.0f;*/
			if(m1 > m_calDat.magMax[0])
			{
				m_calDat.magMax[0] = m1;
				m_calDat.magValid = true;
			}
			if(m2 > m_calDat.magMax[1])
			{
				m_calDat.magMax[1] = m2;
				m_calDat.magValid = true;
			}
			if(m3 > m_calDat.magMax[2])
			{
				m_calDat.magMax[2] = m3;
				m_calDat.magValid = true;
			}
			if(m1 < m_calDat.magMin[0])
			{
				m_calDat.magMin[0] = m1;
				m_calDat.magValid = true;
			}
			if(m2 < m_calDat.magMin[1])
			{
				m_calDat.magMin[1] = m2;
				m_calDat.magValid = true;
			}
			if(m3 < m_calDat.magMin[2])
			{
				m_calDat.magMin[2] = m3;
				m_calDat.magValid = true;
			}

			storeCalibration();

		    if (m_calibrationValid)
		    {
		        mx = -(m1 - m_compassCalOffset[0]) * m_compassCalScale[0];
		        my = -(m2 - m_compassCalOffset[1]) * m_compassCalScale[1];
		        mz = (m3 - m_compassCalOffset[2]) * m_compassCalScale[2];
		    }
		    else
		    {
				mx = m1;
				my = m2;
				mz = m3;
		    }
			mcount = 0;
		}
	}



	  now = micros();
	  deltat = ((now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
	  lastUpdate = now;
	  /* Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of the magnetometer;
	   * the magnetometer z-axis (+ down) is opposite to z-axis (+ up) of accelerometer and gyro!
	   * We have to make some allowance for this orientationmismatch in feeding the output to the quaternion filter.
	   * For the MPU-9150, we have chosen a magnetic rotation that keeps the sensor forward along the x-axis just like
	   * in the LSM9DS0 sensor. This rotation can be modified to allow any convenient orientation convention.
	   * This is ok by aircraft orientation standards!
	   * Pass gyro rate as rad/s
	  */
	  MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
	  //MahonyQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, my, mx, mz);

	// With these settings the filter is updating at a ~145 Hz rate using the Madgwick scheme and
	// >200 Hz using the Mahony scheme even though the display refreshes at only 2 Hz.
	// The filter update rate is determined mostly by the mathematical steps in the respective algorithms,
	// the processor speed (8 MHz for the 3.3V Pro Mini), and the magnetometer ODR:
	// an ODR of 10 Hz for the magnetometer produce the above rates, maximum magnetometer ODR of 100 Hz produces
	// filter update rates of 36 - 145 and ~38 Hz for the Madgwick and Mahony schemes, respectively.
	// This is presumably because the magnetometer read takes longer than the gyro or accelerometer reads.
	// This filter update rate should be fast enough to maintain accurate platform orientation for
	// stabilization control of a fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
	// produced by the on-board Digital Motion Processor of Invensense's MPU6050 6 DoF and MPU9150 9DoF sensors.
	// The 3.3 V 8 MHz Pro Mini is doing pretty well!

}


// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void IMU::MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
	float norm;
	float hx, hy, _2bx, _2bz;
	float s1, s2, s3, s4;
	float qDot1, qDot2, qDot3, qDot4;

	// Auxiliary variables to avoid repeated arithmetic
	float _2q1mx;
	float _2q1my;
	float _2q1mz;
	float _2q2mx;
	float _4bx;
	float _4bz;
	float _2q1 = 2.0f * q1;
	float _2q2 = 2.0f * q2;
	float _2q3 = 2.0f * q3;
	float _2q4 = 2.0f * q4;
	float _2q1q3 = 2.0f * q1 * q3;
	float _2q3q4 = 2.0f * q3 * q4;
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f/norm;
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f/norm;
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	_2q1mx = 2.0f * q1 * mx;
	_2q1my = 2.0f * q1 * my;
	_2q1mz = 2.0f * q1 * mz;
	_2q2mx = 2.0f * q2 * mx;
	hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
	hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
	_2bx = sqrt(hx * hx + hy * hy);
	_2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
	_4bx = 2.0f * _2bx;
	_4bz = 2.0f * _2bz;

	// Gradient decent algorithm corrective step
	s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
	norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
	norm = 1.0f/norm;
	s1 *= norm;
	s2 *= norm;
	s3 *= norm;
	s4 *= norm;

	// Compute rate of change of quaternion
	qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
	qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
	qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
	qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

	// Integrate to yield quaternion
	q1 += qDot1 * deltat;
	q2 += qDot2 * deltat;
	q3 += qDot3 * deltat;
	q4 += qDot4 * deltat;
	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
	norm = 1.0f/norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;

	updateRPY();
}



 // Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
 // measured ones.
void IMU::MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
	float norm;
	float hx, hy, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez;
	float pa, pb, pc;

	// Auxiliary variables to avoid repeated arithmetic
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	if (norm == 0.0f) return; // handle NaN
	norm = 1.0f / norm;        // use reciprocal for division
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
	hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
	bx = sqrt((hx * hx) + (hy * hy));
	bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

	// Estimated direction of gravity and magnetic field
	vx = 2.0f * (q2q4 - q1q3);
	vy = 2.0f * (q1q2 + q3q4);
	vz = q1q1 - q2q2 - q3q3 + q4q4;
	wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
	wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
	wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);

	// Error is cross product between estimated direction and measured direction of gravity
	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
	if (Ki > 0.0f)
	{
		eInt[0] += ex;      // accumulate integral error
		eInt[1] += ey;
		eInt[2] += ez;
	}
	else
	{
		eInt[0] = 0.0f;     // prevent integral wind up
		eInt[1] = 0.0f;
		eInt[2] = 0.0f;
	}

	// Apply feedback terms
	gx = gx + Kp * ex + Ki * eInt[0];
	gy = gy + Kp * ey + Ki * eInt[1];
	gz = gz + Kp * ez + Ki * eInt[2];

	// Integrate rate of change of quaternion
	pa = q2;
	pb = q3;
	pc = q4;
	q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
	q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
	q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
	q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);

	// Normalise quaternion
	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
	norm = 1.0f / norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;


	updateRPY();
}

void IMU::getRPY(float &roll, float &pitch, float &yaw, float &filteredYaw)
{
	roll = m_roll;
	pitch = m_pitch;
	yaw = m_yaw;
	filteredYaw = m_filteredYaw;
}

void IMU::updateRPY()
{
	double quat[4];
//	for(int i = 0; i < 4; i++)
	//	Serial.println(q[i]);

//	for(int i = 0; i < 4; i++)
//		Serial.println(m_calDat.rotRef[i]);
	quat[3] = q[3]*m_calDat.rotRef[3] - q[0]*m_calDat.rotRef[0] - q[1]*m_calDat.rotRef[1] - q[2]*m_calDat.rotRef[2];
	quat[0] = q[3]*m_calDat.rotRef[0] + q[0]*m_calDat.rotRef[3] - q[1]*m_calDat.rotRef[2] + q[2]*m_calDat.rotRef[1];
	quat[1] = q[3]*m_calDat.rotRef[1] + q[0]*m_calDat.rotRef[2] + q[1]*m_calDat.rotRef[3] - q[2]*m_calDat.rotRef[0];
	quat[2] = q[3]*m_calDat.rotRef[2] - q[0]*m_calDat.rotRef[1] + q[1]*m_calDat.rotRef[0] + q[2]*m_calDat.rotRef[3];

//	for(int i = 0; i < 4; i++)
	//	Serial.println(quat[i]);

	  // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
	  // In this coordinate system, the positive z-axis is down toward Earth.
	  // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
	  // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
	  // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
	  // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
	  // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
	  // applied in the correct order which for this configuration is yaw, pitch, and then roll.
	  // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
	/*yaw   = atan2(2.0f * (quat[1] * quat[2] + quat[0] * quat[3]), quat[0] * quat[0] + quat[1] * quat[1] - quat[2] * quat[2] - quat[3] * quat[3]);
	pitch = -asin(2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
	roll  = atan2(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]), quat[0] * quat[0] - quat[1] * quat[1] - quat[2] * quat[2] + quat[3] * quat[3]);*/

	m_yaw   = atan2(2.0f * (quat[3] * quat[0] + quat[1] * quat[2]), 1.0f - 2.0f * (quat[0] * quat[0] + quat[1] * quat[1]));
	m_pitch = asin (2.0f * (quat[3] * quat[1] - quat[2] * quat[0]));
	m_roll  = atan2(2.0f * (quat[3] * quat[2] + quat[0] * quat[1]), 1.0f - 2.0f * (quat[1] * quat[1] + quat[2] * quat[2]));

	m_pitch *= 180.0f / PI;
	m_yaw   *= 180.0f / PI ;
	m_roll  *= 180.0f / PI;

	m_yaw += 180.0f;

	float y = m_yaw;
	while( m_lowpassFilter->output() < y - 360.0f)
	{
		y -= 360.0f;
	}
	while( m_lowpassFilter->output() > y + 360.0f)
	{
		y += 360.0f;
	}

	m_lowpassFilter->input(y);

	if(m_lowpassFilter->output() >= 360.0f)
	{
		m_lowpassFilter->setToNewValue(m_lowpassFilter->output() - 360.0f);
	}
	if(m_lowpassFilter->output() < 0.0f)
	{
		m_lowpassFilter->setToNewValue(m_lowpassFilter->output() + 360.0f);
	}
	//TODO: set start value for lowpassfilter (once on startup instead of 0)

	m_filteredYaw = m_lowpassFilter->output();

}
String IMU::debug()
{
	float roll, pitch, yaw, filteredYaw;
	getRPY(roll, pitch, yaw, filteredYaw);
	return String(yaw) +
			"\t" +
			pitch +
			"\t" +
			roll +
			"\t" +
			(float)1.0f/deltat +

	/*return String((int)1000*ax) +
	"\t" +
	1000*ay +
	"\t" +
	1000*az +
	"\t" +
	 gx +
	"\t" +
	 gy +
	"\t" +
	 gz +
	"\t" +
	 mx +
	"\t" +
	 my +
	"\t" +
	 mz +
	"\t" +

	yaw +
	"\t" +
	pitch +
	"\t" +
	roll +

	"\t" +
	(float)1.0f/deltat +*/
	"\t" +
	m_calDat.magMin[0] +
	"\t" +
	m_calDat.magMin[1] +
	"\t" +
	m_calDat.magMin[2] +
	"\t" +
	m_calDat.magMax[0] +
	"\t" +
	m_calDat.magMax[1] +
	"\t" +
	m_calDat.magMax[2];
}

String IMU::debugHeader()
{
	return String("yaw") +
			"\t" +
			"pitch" +
			"\t" +
			"roll" +
			"\t" +
			"freq"+
/*	return String("ax") +
	"\t" +
	"ay"  +
	"\t" +
	"az"  +
	"\t" +
	 "gx" +
	"\t" +
	 "gy" +
	"\t" +
	 "gz" +
	"\t" +
	 "mx" +
	"\t" +
	 "my" +
	"\t" +
	 "mz" +
	"\t" +
	"yaw" +
	"\t" +
	"pitch" +
	"\t" +
	"roll" +
	"\t" +
	"freq" +*/
	"\t" +
	"magMin[0]" +
	"\t" +
	"magMin[1]" +
	"\t" +
	"magMin[2]" +
	"\t" +
	"magMax[0]" +
	"\t" +
	"magMax[1]" +
	"\t" +
	"magMax[2]";
}




void IMU::deleteCalibration()
{
	calLibErase(0);
	initilizeCalibration();
}
void IMU::initilizeCalibration()
{
	a1=a2=a3=g1=g2=g3=m1=m2=m3=0;
	mcount = 0;
	deltat = 0.0f;
	lastUpdate=now=0;

	ax = ay = az = gx = gy = gz = mx = my = mz = 0;
	q[0] = 1.0f;
	eInt[0] = eInt[1] = eInt[2] = q[1] = q[2] = q[3] = 0.0f;

	m_calDat.magValid = false;
	for (int i = 0; i < 3; i++)
	{
		m_calDat.magMin[i] = 10000;                    // init mag cal data
		m_calDat.magMax[i] = -10000;
		m_calDat.rotRef[i] = 0.0;
	}
	m_calDat.rotRef[3] = 1.0;

}
void IMU::storeCalibration()
{
	if(m_calDat.magValid) //new Data arrived
	{
		setCalibrationData();
		if(millis() - m_lastCalStore > 10000) //and last store was > 10sec in past
		{
			calLibWrite(0,&m_calDat);
			m_lastCalStore = millis();
			m_calDat.magValid = false;
		}
	}
}

void IMU::setCurrentRotationAsRef()
{
	m_calDat.rotRef[0] = -q[0];
	m_calDat.rotRef[1] = -q[1];
	m_calDat.rotRef[2] = -q[2];
	m_calDat.rotRef[3] = q[3];
	calLibWrite(0,&m_calDat);
}

void IMU::resetRotationRef()
{
	m_calDat.rotRef[0] = 0;
	m_calDat.rotRef[1] = 0;
	m_calDat.rotRef[2] = 0;
	m_calDat.rotRef[3] = 1;
}

void IMU::setCalibrationData()
{
    float maxDelta = -1;
    float delta;

	maxDelta = 360.0f;
	for (int i = 0; i < 3; i++) {
		delta = fabs(m_calDat.magMax[i] - m_calDat.magMin[i]);
		m_compassCalScale[i] = maxDelta / delta;            // makes everything the same range
		m_compassCalOffset[i] = (m_calDat.magMax[i] + m_calDat.magMin[i]) / 2.0f;
	}
	m_calibrationValid = true;
}

