HEADER = "Millis,m_currentPosition,m_pressedButtonDebug,m_bytesToSent,CurrentPosition,CurrentDirection,TargetPosition,MSStopped,startButton,stopButton,parkingButton,m_P,m_I,m_D,m_goalType,m_goal,m_lastError,m_errorSum,m_lastFilteredYaw,UI,yaw,pitch,roll,freq,magMin[0],magMin[1],magMin[2],magMax[0],magMax[1],magMax[2],m_speed,m_speed.tripMileage,m_speed.totalMileage,m_speed.waterTemp,m_lampIntensity,m_wind.apparentAngle,m_wind.apparentSpeed,m_wind.displayInKnots,m_wind.displayInMpS,m_depth.anchorAlarm,m_depth.deepAlarm,m_depth.defective,m_depth.depthBelowTransductor,m_depth.metricUnits,m_depth.shallowAlarm,m_depth.unknown,Position"


def decode_data(l):
    dat = l.split("\t")
    if len(dat) < 2:
        dat = l.split(",")
    data = dict()
    for e, d in zip(HEADER.split(','), dat):
        data[e] = d
    return data
