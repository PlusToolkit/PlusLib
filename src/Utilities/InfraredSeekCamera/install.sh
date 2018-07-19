#!/bin/bash 

if [[ $EUID -ne 0 ]]; then 
    echo "You must be root to do this." 1>&2 
    exit 100 
fi

echo "Copy files"
cp ./seekcam.rules /etc/udev/rules.d

echo "Restart UDEV"
/etc/init.d/udev restart

echo "Restart Service"

service udev restart

echo "Finish"


