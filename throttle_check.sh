#!/bin/bash

# Before running this script, make sure you have sysbench installed:
#           sudo apt-get install sysbench
#
# This script helps you check if your Raspberry pi is correctly powered.
# You can read more about Raspberry pi powering issues here: https://ownyourbits.com/2019/02/02/whats-wrong-with-the-raspberry-pi/


# If you're pi is correctly powered (stable power supply and quality cable), after running the script, you should get something like:
#
# 45.6'C 1400 / 600 MHz 1.3813V -
# 55.3'C 1400 / 1400 MHz 1.3813V -
# 58.0'C 1400 / 1400 MHz 1.3813V -
# 60.2'C 1400 / 1400 MHz 1.3813V -
# 60.2'C 1400 / 1400 MHz 1.3813V -
# 61.1'C 1400 / 1400 MHz 1.3813V -
# 61.1'C 1400 / 1400 MHz 1.3813V -
# 60.8'C 1400 / 1400 MHz 1.3813V -

# If your power supply can't provide a stable 5V 2.5A or if the cable is not good enough, you should get something like:
#
# 39.7'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 48.3'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 52.1'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 54.8'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 55.8'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 56.4'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 57.5'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 58.0'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,
# 59.6'C 1400 / 1400 MHz 1.3875V - Throttling has occurred, Under-voltage has occurred,

function throttleCodeMask {
  perl -e "printf \"%s\", $1 & $2 ? \"$3\" : \"$4\""
}

# Make the throttled code readable
#
# See the `get_throttled` method documentation on: https://www.raspberrypi.org/documentation/raspbian/applications/vcgencmd.md
#

function throttledToText {
  throttledCode=$1
  throttleCodeMask $throttledCode 0x80000 "Soft temperature limit has occurred, " ""
  throttleCodeMask $throttledCode 0x40000 "Throttling has occurred, " ""
  throttleCodeMask $throttledCode 0x20000 "Arm frequency capping has occurred, " ""
  throttleCodeMask $throttledCode 0x10000 "Under-voltage has occurred, " ""
  throttleCodeMask $throttledCode 0x8 "Soft temperature limit active, " ""
  throttleCodeMask $throttledCode 0x4 "Currently throttled, " ""
  throttleCodeMask $throttledCode 0x2 "Arm frequency capped, " ""
  throttleCodeMask $throttledCode 0x1 "Under-voltage detected, " ""
}

# Main script, kill sysbench when interrupted
trap 'kill -HUP 0' EXIT
sysbench --test=cpu --cpu-max-prime=10000000 --num-threads=4 run > /dev/null &
maxfreq=$(( $(awk '{printf ("%0.0f",$1/1000); }' < /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq) -15 ))

# Read sys info, print and loop
while true; do
  temp=$(vcgencmd measure_temp | cut -f2 -d=)
  real_clock_speed=$(vcgencmd measure_clock arm | awk -F"=" '{printf ("%0.0f", $2 / 1000000); }' )
  sys_clock_speed=$(awk '{printf ("%0.0f",$1/1000); }' </sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq)
  voltage=$(vcgencmd measure_volts | cut -f2 -d= | sed 's/000//')
  throttled_text=$(throttledToText $(vcgencmd get_throttled | cut -f2 -d=))
  echo "$temp $sys_clock_speed / $real_clock_speed MHz $voltage - $throttled_text"
  sleep 5
done

