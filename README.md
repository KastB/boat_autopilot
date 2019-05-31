# Autopilot for Sailboats
Autopilot for a 12m sailboat based on an Arduino Mega as a controller and relay for different sensors, a raspberry pi as a proxy server to manage several clients and an android app as clients, that can be run on multiple devices simultaniously.
In use for several years now.

Overview
---------
Features:
  - Course relative to the wind
  - Course relative to the compass
  - Tacking
  - Remote control via app or 433 Mhz remote
  - Little rudder movement (at least for our configuration) with an easily parametrizable extended PID controller

There are several modules to make use of different sensors:
  - seatalk interface to read wind speed/direction, vessel speed, water depth/temperature... from raymarine devices
  - gps interface for nmea complient gps sensors
  - position sensor for an incremental rotary sensor, to measure the position of the rudder
  - ir for an ir remote control (hardy useful in the sun)
  - keyboard for direct keyboard input
  - ui for a bluetooth console input
  - imu for an imu consisting of a magnetometer and accelerometer with autocalibration
  - radio for a 433Mhz remote control
 Additionally there is one module to control the motors to actuate the rudder and one module for the actual controller.

Hardware
--------


**Actuator**

As linear actuators, that have a clutch to enable manual control while the autopilot is turned off, are quite expensive a garage door drive is used to move the rudder. To compensate the distance offset between the radial movement of the rudder lever and the linear one of the door drive drawer rails are used.
The motor of the door drive is designed of 24V but works well with 12V, that are available on the boat.
As a motor controller a Olimex BB-VNH3SP30 is used.
The emergency unlocking of the door drive is used as a clutch, thus there is a complete mechanical decoupling possible on any failure.

**Sensors**

Several sensors are neccessary to set up the control loop(s).

***Rudder Reference***

The rudder reference is used to drive the rudder to a defined position, that is determined by a PID controller on a higher level.
As absolute references the three built in buttons are used, that are formaly designed as switch off positions at closed and open state as well as a slow-down-point when closing. These buttons are re-positioned as end-stops as well as the neutral position. To be able to control any rudder position in between a relative angle sensor is implemented using the light barriers of an old  non-laser computer mouse. Despite the possibility to let the built-in chip evaluate the signals and read the values and position using the PS/2 interface, the raw data is used. This leads to two advantages: First, the time for reading the analog voltage is far less than the time for communication. That's especially important as the fusion algorithm for the IMU should be triggered regularly and quite frequently. Second, the coding wheel had to be mounted on the motor axis which isn't that easy. That lead to variing distances between the coding wheel, the led and the lightsensor, thus the sinosoid waveform is oscilating. By writing an algorithm, that respects that noise of the measurement the results are more reliable.

***Heading***

The most simple application of the autopilot would be keeping a certain heading with respect to a compass. As Magnetometers tend to drift and have a not so high update rate the measurements are fusionized with an accelerometer. The results of that IMU are quite good or should at least be sufficient for that application. Suppliers of autopilots use more sophisticated and more expensive technology, that shouldn't be neccessary.I've decided to use a MPU9150 wich combines a MPU6050 accelerometer and an AK8975 compass in one device.

***wind direction***

The second step is an autopilot that controls the heading relative to the appearent wind direction. In order to achieve that the compass heading will be changed by the low pass filtered wind angle. To measure that angle the already installed Raymarine system is used, which is a pretty old one, that uses the Seatalk protocol (neither Seatalk NG nor NMEA). A simple circuit and a serial interface is sufficient as long as no data should be sent.

**control unit**

The head of the system, that does all the calculations is an Arduino Mega, as it has sufficient memory and I/O. It's also cheap and easy to program.

**User interface**

A very important part of the autopilot is the user interface. Three different ways are intendet to be implemented in order to control the system.

***Serial interface***

The serial interface is the most powerfull one, that should be able to change parameters as well as the desired heading, mode (wind or compass heading) and debug the system.

***Bluetooth Android App***

The bluetooth interface is more or less a wireless serial interface. It can be used to program the arduino wirelessly and an simple android app is planned in order to visualize the current heading, wind angle, speed, and mode as well as change heading and mode.


***433 Mhz Remote***

As there is not always a smartphone available a cheap and simple 433Mhz remote control with four buttens should be used in order to change heading and mode.


Software
---------

The application has one module for each of the previously described hardware counterparts. All of them inherit from a timer class, thus they are evaluated on a individually defined period. Additionally a debug interface is implemented in that class, thus easy csv export is possible to plot some graphs etc.
