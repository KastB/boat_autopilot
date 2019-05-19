#!/bin/bash
DIS=2
rm -f /tmp/.X${DIS}-lock
rm -f /tmp/.X11-unix/X${DIS}
unset XAUTHORITY
unset DISPLAY
killall ssh-agent
# start ssh-agent
ssh-agent > $HOME/ssh-agent.sh
source      $HOME/ssh-agent.sh
rm -f       $HOME/ssh-agent.sh
#vncserver -geometry 1024x768 -depth 24 -xstartup /home/pi/src/boat_autopilot/xstartup :$DIS 
vncserver-x11
