#!/bin/bash

# get the current profile.
CURRENT=$(powerprofilesctl get)

# get ac status. uses expected ac path for the craptop of doom.
AC_STATE=$(cat /sys/class/power_supply/ACAD/online)

# define cycle sequence based on current profile.
if [ $CURRENT = "balanced" ]; then
    if [ $AC_STATE = "1" ]; then
        NEXT="performance"
    elif [ $AC_STATE = "0" ]; then
        NEXT="power-saver"
    fi
elif [ $CURRENT = "performance" ]; then
    NEXT="power-saver"
else
    NEXT="balanced"
fi

# set next profile.
powerprofilesctl set $NEXT

# notify user, needs libnotify (you should have it you fiend).
LOGGEDUSER=$(who | cut -d' ' -f1)
runuser -u $LOGGEDUSER -- notify-send "Power Profile" "Switched to $NEXT." -i battery-profile-balanced
