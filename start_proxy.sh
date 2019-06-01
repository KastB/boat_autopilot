#!/bin/bash

base_path=/home/pi/src/boat_autopilot
declare -x PYTHONPATH=$PYTHONPATH:$base_path

/usr/bin/rfcomm bind -r 98:D3:31:FB:0C:2F
/usr/bin/python3 $base_path/nmea_proxy/proxy.py&
/bin/sleep 30 && /usr/bin/python3 $base_path/nmea_proxy/logging_client.py

