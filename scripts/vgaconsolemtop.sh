#!/bin/bash
#
# To get mtop to start up automatically on virtual console tty1
#
# sudo systemctl edit getty@tty1
#
# [Service]
# ExecStart=
# ExecStart=-/sbin/agetty --noclear --skip-login --login-program /[path to]/vgaconsolemtop.sh %I $TERM

setterm -powersave off -powerdown 0 -blank 0
setfont Uni3-Terminus12x6
/usr/local/bin/mtop
