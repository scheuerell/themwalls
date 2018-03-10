

tzselect >> test.txt
TZSTR=$(tzselect)

#printf "TZ='America/Menominee'; export TZ" >> /home/pi/.profile
#grep TZ= /home/pi/.profile
date

# remove pre-existion assignments to TZ in .profile
grep -v "TZ=" /home/pi/.profile > temp && mv temp /home/pi/.profile

# select and assign the new timezone using tzselect
TZNEW=$(tzselect)
printf "\nTZ='$TZNEW'; export TZ" >> /home/pi/.profile


in case ....
-------
# ~/.profile: executed by the command interpreter for login shells.
# This file is not read by bash(1), if ~/.bash_profile or ~/.bash_login
# exists.
# see /usr/share/doc/bash/examples/startup-files for examples.
# the files are located in the bash-doc package.

# the default umask is set in /etc/profile; for setting the umask
# for ssh logins, install and configure the libpam-umask package.
#umask 022

# if running bash
if [ -n "$BASH_VERSION" ]; then
    # include .bashrc if it exists
    if [ -f "$HOME/.bashrc" ]; then
	. "$HOME/.bashrc"
    fi
fi

# set PATH so it includes user's private bin if it exists
if [ -d "$HOME/bin" ] ; then
    PATH="$HOME/bin:$PATH"
fi

