#!/bin/bash

base_path=/home/pi/src/boat_autopilot
declare -x PYTHONPATH=$PYTHONPATH:$base_path

/usr/bin/python3 $base_path/nmea_proxy/logging_client.py&

