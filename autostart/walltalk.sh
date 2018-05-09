#! /bin/bash

#set gpio 2 (wiringpi gpio ID) to high-impedance (no pullup/down)
#gpio mode 2 tri

# set mic volume to max (range is 0-16)
amixer -D hw:Device cset numid=3 16

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

#set timezone
printf "TZ='America/Menominee'; export TZ" >> /home/pi/.profile

#add wifi interface for access point
sudo /sbin/iw phy phy0 interface add ap0 type __ap
sudo /bin/ip link set ap0 address b8:27:eb:79:18:b0

sleep 30
#sudo ifdown --force wlan0 && sudo ifdown --force ap0 && sudo ifup ap0 && sudo ifup wlan0
sudo sysctl -w net.ipv4.ip_forward=1
sudo iptables -t nat -A POSTROUTING -s 192.168.10.0/24 ! -d 192.168.10.0/24 -j MASQUERADE
sudo systemctl restart dnsmasq 