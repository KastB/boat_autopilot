"""
Description

@author: Bernd Kast
@copyright: Copyright (c) 2018, Siemens AG
@note:  All rights reserved.
"""

import serial, pynmea2, time, math

SERIALPORTIN = "/dev/ttyS22"
BAUDRATEIN = 19200
SERIALPORTOUT = "/dev/ttyS20"
BAUDRATEOUT= 4800
TEST = True

HEADER = "Millis,m_currentPosition,m_pressedButtonDebug,m_bytesToSent,CurrentPosition,CurrentDirection,TargetPosition,MSStopped,startButton,stopButton,parkingButton,m_P,m_I,m_D,m_goalType,m_goal,m_lastError,m_errorSum,m_lastFilteredYaw,yaw,filteredYaw,pitch,roll,freq,magMin[0],magMin[1],magMin[2],magMax[0],magMax[1],magMax[2],m_speed,m_speed.tripMileage,m_speed.totalMileage,m_speed.waterTemp,m_lampIntensity,m_wind.apparentAngle,m_wind.apparentSpeed,m_wind.displayInKnots,m_wind.displayInMpS,m_depth.anchorAlarm,m_depth.deepAlarm,m_depth.defective,m_depth.depthBelowTransductor,m_depth.metricUnits,m_depth.shallowAlarm,m_depth.unknown,Position"

def decode_data(line):
    dat = line.split(",")
    data = dict()
    for e, d in zip(HEADER.split(','), dat):
        data[e] = d
    return data

def new_serial(name, boud):
    ser = serial.Serial(name, boud, rtscts=True, dsrdtr=True)
    ser.bytesize = serial.EIGHTBITS  # number of bits per bytes
    ser.parity = serial.PARITY_NONE  # set parity check: no parity
    ser.stopbits = serial.STOPBITS_ONE  # number of stop bits
    # ser.timeout = None          #block read
    # ser.timeout = 0             #non-block read

    ser.timeout = None  # timeout block read
    ser.xonxoff = False  # disable software flow control
    ser.rtscts = False  # disable hardware (RTS/CTS) flow control
    ser.dsrdtr = False  # disable hardware (DSR/DTR) flow control
    ser.writeTimeout = 0  # timeout for write
    return ser

if not TEST:
    ser_in = new_serial(SERIALPORTIN, BAUDRATEIN)
ser_out = new_serial(SERIALPORTOUT, BAUDRATEOUT)

print('Starting Up Serial Monitor')
fh = None
try:
    if TEST:
        fh = open("/export/home/bernd/src/boat_autopilot/data/data.csv")
    while True:
        if TEST:
            line = fh.readline()
            time.sleep(1)
        else:
            line = ser_in.readline().decode()
        data = decode_data(line)
        print(data)
        # https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:opencpn_user_manual:advanced_features:nmea_sentences
        #ser_out.write(pynmea2.GGA('GP', 'GGA', ('184353.07', '1929.045', 'S', '02410.506', 'E', '1', '04', '2.6', '100.00', 'M', '-33.9', 'M', '', '0000')).render(True,True,True))
        try:
            ser_out.write(pynmea2.VHW('GP', 'VHW', ("", "T", "", "M", data["m_speed"], "K", str(float(data["m_speed"])*1.6), "K")).render(True, True, True).encode())
        except ValueError as e:
            print(e)
        ser_out.write(pynmea2.MWV('GP', 'MWV', (data["m_wind.apparentAngle"], "R", data["m_wind.apparentSpeed"], "K", "A")).render(True, True, True).encode())
        ser_out.write(pynmea2.HDM('GP', 'HDM', (data["yaw"], "M")).render(True, True, True).encode())
        ser_out.write(pynmea2.DPT('GP', 'DPT', (data["m_depth.depthBelowTransductor"], "0.5", "70.0")).render(True, True, True).encode())
        ser_out.write(pynmea2.MTW('GP', 'MTW', (data["m_speed.waterTemp"], "C")).render(True, True, True).encode())
        #https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:developer_manual:plugins:beta_plugins:nmea_converter
        try:
            angle = float(data["m_wind.apparentAngle"]) / 180.0 * math.pi
            wind_speed = float(data["m_wind.apparentSpeed"])
            boat_speed = float(data["m_speed"])
            ser_out.write(pynmea2.MWV('GP', 'MWV', (str(180.0/math.pi*(angle + math.acos((math.sqrt(wind_speed) + math.sqrt(boat_speed)+math.sqrt(wind_speed)-2 *boat_speed *wind_speed * math.cos(angle)-math.sqrt(boat_speed)) / (2 * math.sqrt(math.sqrt(
                                                                                                                                                   boat_speed) + math.sqrt(wind_speed)-2 *boat_speed * math.cos(angle)) * wind_speed)))),
                                                    "T",
                                                    str(math.sqrt(math.sqrt(boat_speed)+math.sqrt(wind_speed)-2 *boat_speed *wind_speed * math.cos(angle))),
                                                    "N", "A")).render(True, True, True).encode())
        except (ValueError, ZeroDivisionError) as e:
            print(e)
        # TODO: check for units:
        # depth: m_depth.metricUnits
        # Windspeed
        # vhw
        # TODO: add trip and total mileage etc
        # TODO: add gps
        # TODO: add waypoint handling as backward channel


except KeyboardInterrupt:
    print('interrupted!')
    if TEST:
        fh.close()

''''
class MWV(TalkerSentence, ValidStatusFix):
    """ Wind Speed and Angle
    NMEA 0183 standard Wind Speed and Angle, in relation to the vessel's
    bow/centerline.
    """
    fields = (
        ("Wind angle", "wind_angle", Decimal), # in relation to vessel's centerline
        ("Reference", "reference"), # relative (R)/true(T)
        ("Wind speed", "wind_speed", Decimal),
        ("Wind speed units", "wind_speed_units"), # K/M/N
        ("Status", "status"),
    )
    
class DBT(TalkerSentence):
    """ Depth Below Transducer
    """
    fields = (
        ("Depth below surface, feet", "depth_feet", Decimal),
        ("Feet", "unit_feet"),
        ("Depth below surface, meters", "depth_meters", Decimal),
        ("Meters", "unit_meters"),
        ("Depth below surface, fathoms", "depth_fathoms", Decimal),
        ("fathoms", "unit_fathoms"),
    )
    
class MTW(TalkerSentence):
    """ Water Temperature
    """
    fields = (
        ('Water temperature', 'temperature', Decimal),
        ('Unit of measurement', 'units'),
    )
class VLW(TalkerSentence):
    """ Distance Traveled through the Water
    """
    fields = (
        ('Water trip distance', 'trip_distance', Decimal),
        ('Trip distance nautical miles', 'trip_distance_miles'),
        ('Water trip distance since reset', 'trip_distance_reset', Decimal),
        ('Trip distance nautical miles since reset', 'trip_distance_reset_miles'),
    )
class ROT(TalkerSentence, ValidStatusFix):
    """ Rate of Turn
    """
    fields = (
        ("Rate of turn", "rate_of_turn"), #- indicates bow turn to port
        ('Status', 'status'), # contains the 'A' or 'B' flag
    )


class VWR(TalkerSentence):
    fields = (
        ("Degrees Rel", "deg_r", float),
        ("Left/Right", "l_r"),#R means right
        ("Wind speed kn", "wind_speed_kn", float),
        ("Knots", "unit_knots"),#N means knots
        ("Wind Speed m/s", "wind_speed_ms", float),
        ("m/s", "unit_ms"),#M means m/s
        ("Wind Speed Km/h", "wind_speed_km", float),
        ("Knots", "unit_km"), #K means Km
    )

    # TODO
    # getters/setters that normalize units,
    # apply L/R sign, and sync all fields
    # when setting the speed

#VWR - Relative Wind Speed and Angle
# 
#         1  2  3  4  5  6  7  8 9
#         |  |  |  |  |  |  |  | |
# $--VWR,x.x,a,x.x,N,x.x,M,x.x,K*hh<CR><LF>

# Field Number:
#  1) Wind direction magnitude in degrees
#  2) Wind direction Left/Right of bow
#  3) Speed
#  4) N = Knots
#  5) Speed
#  6) M = Meters Per Second
#  7) Speed
#  8) K = Kilometers Per Hour
#  9) Checksum

class OSD(TalkerSentence, ValidStatusFix):
    """ Own Ship Data
    """
    fields = (
        ("True Heading", "heading", Decimal),
        ("Status", "status"), # A / V
        ("Vessel Course true degrees", "course", Decimal),
        ("Course True", "course_true"), # T / R (True / Relative)
        ("Vessel Speed", "speed", Decimal),
        ("Speed Reference", "speed_ref"),
        ("Vessel Set true degrees", "set", Decimal),
        ("Vessel Drift(speed)", "drift", Decimal),
        ("Speed Units", "speed_unit"),
    )
# VWT - NMEA 0183 True wind angle in relation to the vessel's heading, and true wind
# speed referenced to the water.
class VWT(TalkerSentence):
    fields = (
        ('Wind angle relative to the vessel', 'wind_angle_vessel', Decimal),
        ('Direction, L=Left, R=Right, relative to the vessel head', 'direction'),
        ('Wind speed knots', 'wind_speed_knots', Decimal),
        ('Knots', 'knots'), # N = Knots
        ('Wind speed meters/second', 'wind_speed_meters', Decimal),
        ('Meters', 'meters'), # M = Meters/second
        ('Wind speed km/h', 'wind_speed_km', Decimal),
        ('Km', 'km'), # K = km/h
    )
    # DPT - water depth relative to the transducer and offset of the measuring
# transducer
# Used by simrad devices (f.e. EK500)
class DPT(TalkerSentence):
    fields = (
        ('Water depth, in meters', 'depth', Decimal),
        ('Offset from the trasducer, in meters', 'offset', Decimal),
        ('Maximum range scale in use', 'range', Decimal),
    )
'''
