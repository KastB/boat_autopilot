from pathlib import Path
import math
import pynmea2
import serial
import time

from nmea_proxy.servers import TCPServer

SERIALPORTIN = "/dev/ttyS22"
SERIALPORTIN = "/dev/ttyUSB0"
BAUDRATEIN = 115200
TEST = False
DEBUG = False

HEADER = "Millis,m_currentPosition,m_pressedButtonDebug,m_bytesToSent,CurrentPosition,CurrentDirection,TargetPosition,MSStopped,startButton,stopButton,parkingButton,m_P,m_I,m_D,m_goalType,m_goal,m_lastError,m_errorSum,m_lastFilteredYaw,UI,yaw,pitch,roll,freq,magMin[0],magMin[1],magMin[2],magMax[0],magMax[1],magMax[2],m_speed,m_speed.tripMileage,m_speed.totalMileage,m_speed.waterTemp,m_lampIntensity,m_wind.apparentAngle,m_wind.apparentSpeed,m_wind.displayInKnots,m_wind.displayInMpS,m_depth.anchorAlarm,m_depth.deepAlarm,m_depth.defective,m_depth.depthBelowTransductor,m_depth.metricUnits,m_depth.shallowAlarm,m_depth.unknown,Position"


def decode_data(l):
    dat = l.split("\t")
    if len(dat) < 2:
        dat = l.split(",")
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
    ser.writeTimeout = 5  # timeout for write
    return ser


def convert_and_send_as_nmea(out, data):
    # https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:opencpn_user_manual:advanced_features:nmea_sentences
    # https://github.com/OpenCPN/OpenCPN/blob/c4fb6a0ad0205501ae902b57f9c64b7d0262a199/plugins/dashboard_pi/src/dashboard_pi.cpp
    try:
        out.write(pynmea2.MWV('II', 'MWV', (str(float(data["m_wind.apparentAngle"])), "R", data["m_wind.apparentSpeed"], "K", "A")).render(True, True, True))
        out.write(pynmea2.HDM('II', 'HDM', (data["yaw"], "M")).render(True, True, True))
        out.write(pynmea2.DPT('II', 'DPT', (data["m_depth.depthBelowTransductor"], "0.5", "70.0")).render(True, True, True))
        out.write(pynmea2.MTW('II', 'MTW', (data["m_speed.waterTemp"], "C")).render(True, True, True))
        out.write(pynmea2.VTG('II', 'VTG', ("0.0", "T", "0.0", "M", data["m_speed"], "N", str(float(data["m_speed"]) * 1.8), "K")).render(True, True, True))
        out.write(pynmea2.VHW('II', 'VHW', ("0.0", "T", "0.0", "M", data["m_speed"], "N", str(float(data["m_speed"]) * 1.8), "K")).render(True, True, True))

        out.write(pynmea2.XDR('II', 'XDR', ("A", data["pitch"], "", "PITCH")).render(True, True, True))
        out.write(pynmea2.XDR('II', 'XDR', ("A", data["roll"], "", "ROLL")).render(True, True, True))

        angle = str(math.sin(int(data["m_currentPosition"]) / 600))
        out.write(pynmea2.RSA('II', 'RSA', (angle, "R", angle, "R")).render(True, True, True))
        position_information = data["Position"].split("##")
        if len(position_information) > 0:
            for p in position_information:
                out.write(p)
        if DEBUG:
            print("roll:" + data["roll"])
            print("pitch:" + data["pitch"])
            print("yaw:" + data["yaw"])
            print("freq:" + data["freq"])

    except Exception as e:
        print(e)
    # https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:developer_manual:plugins:beta_plugins:nmea_converter
    try:
        wind_direction = float(data["m_wind.apparentAngle"]) / 180.0 * math.pi
        wind_speed = float(data["m_wind.apparentSpeed"])
        boat_speed = float(data["m_speed"])

        boat_direction = 0.0
        u = boat_speed * math.sin(boat_direction) - wind_speed * math.sin(wind_direction)
        v = boat_speed * math.cos(boat_direction) - wind_speed * math.cos(wind_direction)

        tws = str(math.sqrt(u * u + v * v))

        twd = 180.0 + math.atan2(u, v) * 180.0 / math.pi
        if twd < 0.0:
            twd += 360.0
        twd = str(twd)

        out.write(pynmea2.MWV('II', 'MWV', (twd, "T", tws, "N", "A")).render(True, True, True))
    except (ValueError, ZeroDivisionError, KeyError) as e:
        print(e)

    # TODO: check for units:
    # depth: m_depth.metricUnits
    # Windspeed
    # vhw
    # TODO: add trip and total mileage etc
    # TODO: add waypoint handling as backward channel


def run():
    ser_in = None
    if not TEST:
        ser_in = new_serial(SERIALPORTIN, BAUDRATEIN)
    # out_nmea = UDPServer("127.0.0.1", 2947)
    out_nmea = TCPServer("0.0.0.0", 2947)
    out_raw = TCPServer("0.0.0.0", 2948)

    print('Starting Up Serial Monitor')
    fh = None
    try:
        if TEST:
            fh = open(str(Path.home()) + "/src/boat_autopilot/data/data.csv")
        while True:
            try:
                # get data
                if TEST:
                    line = fh.readline()
                    time.sleep(1)
                else:
                    line = ser_in.readline().decode()
                # print(line)
                data = decode_data(line)
                # print(data)
                convert_and_send_as_nmea(out_nmea, data)
                out_raw.write(line)
            except Exception as e:
                print(e)


            ret = out_raw.get_out_buffer() + out_nmea.get_out_buffer()
            if len(ret) > 0:
                print (ret)
                if not TEST:
                    try:
                        for l in ret:
                            ser_in.write("{}\n".format(l).encode())
                    except Exception as e:
                        print(e)

    except KeyboardInterrupt:
        print('interrupted!')
        out_nmea.close()
        out_raw.close()
        if TEST:
            fh.close()


''''
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

run()
