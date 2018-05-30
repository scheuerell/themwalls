#!/bin/bash
git config --global user.name scheuerell
git config --global user.email "mikejscheuerell@gmail.com"
git config --global credential.helper cache
git config --global credential.helper 'cache --timeout=86400'
#navigate to desired working directory
#mkdir /home/pi/Projects
cd /home/pi/Projects
git clone https://github.com/scheuerell/walltalk
#sudo apt-get update
sudo apt-get install libgtk2.0-dev
sudo apt-get install libjack-jackd2-dev
sudo apt-get install libsndfile1-dev
sudo apt-get install libreadline-dev  #for timemachine cmd line interface
sudo apt-get install wiringpi #for GPIO
sudo apt-get install libudev-dev # for USB device discovery

sudo apt-get install dnsmasq hostapd
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/dnsmasq.conf /etc/dnsmasq.conf
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/hostapd.conf /etc/hostapd/hostapd.conf
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/hostapd /etc/default/hostapd
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/interfaces /etc/network/interfaces
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/hosts /etc/hosts
sudo apt-get install nginx
sudo cp -p -f /home/pi/Projects/walltalk/usr_setup/access_point/nginx.conf /etc/nginx/nginx.conf

sudo python /home/pi/Projects/walltalk/usr_setup/webpage/setup.py install