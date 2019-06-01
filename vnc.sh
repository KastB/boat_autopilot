#!/bin/bash
xhost +
declare -x DISPLAY=":0"
x11vnc -forever -nopw -display :0 &
opencpn -fullscreen &


