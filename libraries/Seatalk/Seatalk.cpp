#include "Arduino.h"
#include "Seatalk.h"

// True wind:
// $XXMWV,$WIMWV1+acos((sqr($WIMWV3)+sqr($GPRMC7)+sqr($WIMWV3)-2*$GPRMC7*$WIMWV3*cos($WIMWV1*3.14159265/180)-sqr($GPRMC7))/(2*sqrt(sqr($GPRMC7)+sqr($WIMWV3)-2*$GPRMC7*cos($WIMWV1*3.14159265/180))*$WIMWV3)),T,sqrt(sqr($GPRMC7)+SQR($WIMWV3)-2*$GPRMC7*$WIMWV3*cos($WIMWV1*3.14159265/180)),N,A

Seatalk::Seatalk(unsigned long interval) {
  m_interval = interval;

  m_speed.speed = -1;
  m_speed.tripMileage = -1;
  m_speed.totalMileage = -1;
  m_speed.waterTemp = -1;
  m_lampIntensity = -1;

  m_rawReadCount = 0;

  m_depth.anchorAlarm = false;
  m_depth.deepAlarm = false;
  m_depth.defective = false;
  m_depth.depthBelowTransductor = 0.0;
  m_depth.metricUnits = false;
  m_depth.shallowAlarm = false;
  m_depth.unknown = false;

  m_debug = false;
  m_lastDataRead = 0.0;
  m_bufferPosition = 0;
}

Seatalk::~Seatalk() {}

void Seatalk::resetBuffer() {
  m_rawReadCount = 0;
  m_rawMessage[0] = 0;
  m_rawMessage[1] = 0;
  m_rawMessage[2] = 0;
  m_rawMessage[3] = 0;
  m_rawMessage[4] = 0;
  m_rawMessage[5] = 0;
}



void Seatalk::shiftBuffer(short expectedMsgLength) {
#ifdef DEBUGING
  Serial.println("shifting");
  Serial.println(m_rawReadCount);
  Serial.println(expectedMsgLength);
  for(int z = 0; z < 10; z++) {
        Serial.print(m_rawMessage[z]);
        Serial.print("-");
  }
#endif
  for(short i = expectedMsgLength; i < m_rawReadCount; i++) {
    m_rawMessage[i - expectedMsgLength] = m_rawMessage[i];
  }
  /*
  for(short i = m_rawReadCount - expectedMsgLength; i < 6; i++) { // TODO: remove magic numbers
      m_rawMessage[i] = 0;
  } */
  m_rawReadCount -= expectedMsgLength;
#ifdef DEBUGING
  Serial.println("mid_shift");
  for(int z = 0; z < 10; z++) {
      Serial.print(m_rawMessage[z]);
      Serial.print("-");
  }
  Serial.println("end_shift");
  Serial.println(m_rawReadCount);
#endif
}

void Seatalk::update() {
  short expectedMsgLength = -1;
  //if (millis() - m_lastDataRead > 10)  // no Data since "long" time=> next time a new message begins
  //  resetBuffer();

  while (Serial2.available())  // There is something to be read
  {
    m_rawMessage[m_rawReadCount] = Serial2.read();
    m_rawReadCount++;
    if (m_rawReadCount > 16) {
      break;
    }
  }
  while(m_rawReadCount > 2) {
      // determine expected length of current message
      expectedMsgLength = (m_rawMessage[1] & 0x0F);
      if (expectedMessageLength(m_rawMessage[0]) != expectedMsgLength) { // not a valid message
#ifdef DEBUGING
        Serial.println("corrupt message");
        Serial.println(m_rawMessage[0]);
        Serial.println(expectedMsgLength);
        Serial.println(m_rawReadCount);
#endif
        shiftBuffer(1);
        continue;
      }
      if (m_rawReadCount < expectedMsgLength + 3) { // +3 => header(id, length, first data block)  too short => wait for more data
        break;
      }
      // everything is ok
      parseMessage(expectedMsgLength);
#ifdef DEBUGING
      Serial.println("received");
      Serial.println((int)m_rawMessage[0], HEX);
      Serial.println("received_end");
#endif
      shiftBuffer(expectedMsgLength + 3);// +3 => header(id, length, first data block)
  }
}

short Seatalk::expectedMessageLength(short msgID) {
  switch (msgID) {
    case 0x00:
      return 2;
      break;
    case 0x10:
      return 1;
      break;
    case 0x11:
      return 1;
      break;
    case 0x20:
      return 1;
      break;
    case 0x21:
      return 2;
      break;
    case 0x24:
      return 2;
      break;
    case 0x22:
      return 2;
      break;
    case 0x23:
      return 1;
      break;
    case 0x30:
      return 0;
      break;
    default:
      return -1;
  }
}

void Seatalk::parseMessage(int expectedMsgLength) {
  switch (m_rawMessage[0]) {
      /*
              00  02  YZ  XX XX  Depth below transducer: XXXX/10 feet
              Flags in Y: Y&8 = 8: Anchor Alarm is active
                                Y&4 = 4: Metric display units or
                                                 Fathom display units if
         followed by command 65 Y&2 = 2: Used, unknown meaning Flags in Z: Z&4 =
         4: Transducer defective Z&2 = 2: Deep Alarm is active Z&1 = 1: Shallow
         Depth Alarm is active Corresponding NMEA sentences: DPT, DBT
      */
    case 0x00:
      m_depth.anchorAlarm = m_rawMessage[2] & 0b10000000;
      m_depth.metricUnits = m_rawMessage[2] & 0b01000000;
      m_depth.unknown = m_rawMessage[2] & 0b00100000;
      m_depth.defective = m_rawMessage[2] & 0b00000100;
      m_depth.deepAlarm = m_rawMessage[2] & 0b00000010;
      m_depth.shallowAlarm = m_rawMessage[2] & 0b00000001;
      m_depth.depthBelowTransductor =
          ((float)m_rawMessage[4] * 256 + m_rawMessage[3]) * 0.3048 / 10.0f;
      break;
      /*
              10  01  XX  YY  Apparent Wind Angle: XXYY/2 degrees right of bow
                                               Used for autopilots Vane Mode
         (WindTrim) Corresponding NMEA sentence: MWV
      */
    case 0x10:
      float angle;
      //prevent wrong measurements
      float delta;
      angle = ((float)m_rawMessage[2] * 256 + m_rawMessage[3]) / 2.0;
   	  m_wind.apparentAngle = angle;
      m_wind.apparentAngleFiltered->input(m_wind.apparentAngle);
      break;
      /*
              11  01  XX  0Y  Apparent Wind Speed: (XX & 0x7F) + Y/10 Knots
                                               Units flag: XX&0x80=0    =>
         Display value in Knots XX&0x80=0x80 => Display value in Meter/Second
                                               Corresponding NMEA sentence: MWV
       */
    case 0x11:
      m_wind.apparentSpeed =
          ((float)(m_rawMessage[2] & 0x7F)) + (m_rawMessage[3] & 0x0F) / 10.0;
      break;
      /*
               20  01  XX  XX  Speed through water: XXXX/10 Knots
                                               Corresponding NMEA sentence: VHW
       */
    case 0x20:
      m_speed.speed = ((float)m_rawMessage[3] * 256 + m_rawMessage[2]) / 10.0f;
      break;
      /*
       *  21  02  XX  XX  0X    Mileage: XXXXX/100 nautical miles
       */
    case 0x21:
      /*		Serial.println("0x21");
//case 0x24:
                      Serial.println("mileage");
                      Serial.println(m_rawMessage[4]);
                      Serial.println(m_rawMessage[3]);
                      Serial.println(m_rawMessage[2]);

                      Serial.println(m_rawMessage[5], BIN);
                      Serial.println(m_rawMessage[4], BIN);
                      Serial.println(m_rawMessage[3], BIN);
                      Serial.println(m_rawMessage[2], BIN);*/
      m_speed.tripMileage = ((float)(m_rawMessage[4] & 0x0F) * (2 ^ 16) +
    		  	  	  	  	     (float) m_rawMessage[3] * (2 ^ 8) +
    		  	  	  	  	     (float) m_rawMessage[2]) / 100.0f;
      //m_speed.tripMileage = m_rawMessage[2];
      break;
      /*
       *  22  02  XX  XX  00  Total Mileage: XXXX/10 nautical miles
       */
    case 0x22:
      /*		Serial.println("totalmileage");
                      Serial.println(m_rawMessage[3]);
                      Serial.println(m_rawMessage[2]);

                      Serial.println(m_rawMessage[5], BIN);
                      Serial.println(m_rawMessage[4], BIN);
                      Serial.println(m_rawMessage[3], BIN);
                      Serial.println(m_rawMessage[2], BIN);*/
      m_speed.totalMileage =
          ((float)(m_rawMessage[4] & 0x0F) * (2 ^ 16) +
		   (float) m_rawMessage[3] * (2 ^ 8) +
		   (float) m_rawMessage[2]) / 10.0f;
      break;
      /*
       *  23  Z1  XX  YY  Water temperature (ST50): XX deg Celsius, YY deg
       Fahrenheit Flag Z&4: Sensor defective or not connected (Z=4)
               Corresponding NMEA sentence: MTW
       */
    case 0x23:
      m_speed.waterTemp = m_rawMessage[2];
      break;
      /*
      *	 24  02  00  00  XX  Display units for Mileage & Speed
                   XX: 00=nm/knots, 06=sm/mph, 86=km/kmh
      */
    case 0x24:
      break;

      /*
       *  30  00  0X      Set lamp Intensity; X=0: L0, X=4: L1, X=8: L2, X=C: L3
                   (only sent once when setting the lamp intensity)
       */
    case 0x30:
      switch (m_rawMessage[2]) {
        case 0x00:
          m_lampIntensity = 0;
          break;
        case 0x04:
          m_lampIntensity = 1;
          break;
        case 0x08:
          m_lampIntensity = 2;
          break;
        case 0x0C:
          m_lampIntensity = 3;
          break;
      }
      break;
    default:
      Serial.println("#########################");
      Serial.println(expectedMsgLength);
      for (int i = 0; i < expectedMsgLength; i++) {
        Serial.println(m_rawMessage[i]);
      }
  }
}

void Seatalk::normalize(float &angle)
{
  //normalize
  while (angle > 180.0f) {
	angle -= 360.0f;
  }
  while (angle <= -180.0f) {
	angle += 360.0f;
  }
}

void Seatalk::debug(HardwareSerial& serial) {
  char spacer = ',';
  serial.print(m_speed.speed);
  serial.print(spacer);
  serial.print(m_speed.tripMileage);
  serial.print(spacer);
  serial.print(m_speed.totalMileage);
  serial.print(spacer);
  serial.print(m_speed.waterTemp);
  serial.print(spacer);
  serial.print(m_lampIntensity);
  serial.print(spacer);

  serial.print(m_wind.apparentAngle);
  serial.print(spacer);
  serial.print(m_wind.apparentSpeed);
  serial.print(spacer);
  serial.print(m_wind.displayInKnots);
  serial.print(spacer);
  serial.print(m_wind.displayInMpS);
  serial.print(spacer);

  serial.print(m_depth.anchorAlarm);
  serial.print(spacer);
  serial.print(m_depth.deepAlarm);
  serial.print(spacer);
  serial.print(m_depth.defective);
  serial.print(spacer);
  serial.print(m_depth.depthBelowTransductor);
  serial.print(spacer);
  serial.print(m_depth.metricUnits);
  serial.print(spacer);
  serial.print(m_depth.shallowAlarm);
  serial.print(spacer);
  serial.print(m_depth.unknown);
}

void Seatalk::debugHeader(HardwareSerial& serial) {
  serial.print(
      F("m_speed,m_speed.tripMileage,m_speed.totalMileage,m_speed."
        "waterTemp,m_lampIntensity,m_wind.apparentAngle,m_wind."
        "apparentSpeed,m_wind.displayInKnots,m_wind.displayInMpS,m_depth."
        "anchorAlarm,m_depth.deepAlarm,m_depth.defective,m_depth."
        "depthBelowTransductor,m_depth.metricUnits,m_depth.shallowAlarm,m_"
        "depth.unknown"));
}
