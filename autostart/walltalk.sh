#! /bin/bash

#exit

#set timezone
TZ='America/Menominee'; export TZ
#set gpio 2 (wiringpi gpio ID) to high-impedance (no pullup)
#gpio mode 2 tri
# start jack server
#/usr/bin/jackd -dalsa -dhw:C320,0 -r48000 -p1024 -n2 &
/usr/bin/jackd -dalsa -dhw:Device -r48000 -p1024 -n2 &
# wait for jack server to be available
jack_wait -w
# start walltalk
walltalk -c 1 -s -f wav &
# connect mic port to walltalk
sleep 5
/usr/bin/jack_connect system:capture_1 walltalk:in_1
while [ ! $? -eq 0 ]
do
	/usr/bin/jack_connect system:capture_1 walltalk:in_1
	sleep 2
done