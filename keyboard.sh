#!/bin/bash

# https://github.com/fcrespo82/acer-lighting-daemon
# built for ENE K513 embedded keyboard controller. has load order issues against usb hid devices, so it needs to be enumerated. acer-rgb values can be whatever i just like white, nice and muted.

enumeration=$(for f in /sys/class/hidraw/hidraw*; do echo -n "${f##*/}: "; cat "$f/device/uevent" | grep HID_NAME | cut -d '=' -f2; done | grep "ENEK5130:00 0CF2:5130" | cut -c 1-7)
acer-rgb SET hidraw=/dev/$enumeration dev=keyboard effect=static bright=100 r=255 g=255 b=255 zone-all
acer-rgb SET hidraw=/dev/$enumeration dev=lid effect=static bright=100 r=255 g=255 b=255
