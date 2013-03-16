#!/bin/bash

#################################################################
## bbexec (Bash Background Exec)
## Copyright 2011 FREEVERSE INC. All rights reserved.
## AUTHOR: Jesse Sanford 
## EMAIL: sanford@freeverse.com
##
## DESCRIPTION:
## Takes an executable launches it and sends it to the background
## pipes all output to a specified location
#################################################################

usage()
{
cat << EOF
bbexec usage: $0 [-hv] -d path/to/executable [-p /path/to/pid-file] [-l /path/to/log] [-a "quoted space delimeted list of params"]

This script will daemonize (background) an executable and then pass it on.
NOTE: if the -p (pid-file) parameter is not set this script will attempt
to write one out to the following locations in order:
/var/run/\`basename \$DAEMON\`/\`basename \$DAEMON\`.pid (if parent dir exists)
/usr/local/var/run/\`basename \$DAEMON\`/\`basename \$DAEMON\`.pid (if parent dir exists)
/var/run/\`basename \$DAEMON\`.pid (if run as root)
/usr/local/var/run/\`basename \$DAEMON\`.pid (if not run as root)

OPTIONS:
   -h       Show this message
   -d       The executable to daemonize (background)
   -p       The pid file to output with the new daemons pid
   -l       The location to log stdout and stderr to
   -a       The quoted space delimeted list of arguments to pass through to the executable
   -v       Verbose
   
Example:
$ ./bbexec.sh -d /usr/local/bin/node -p /var/run/node -l /var/log/node -a "app.js --debug-brk"
EOF
}

#node location
DAEMON=""
LOG=""
PID=""
ARGS=""
VERBOSE=0

while getopts "hd:l:p:a:v" OPTION
do
     case $OPTION in
         h)
             usage
             exit 1
             ;;
         d)
             DAEMON=$OPTARG
             ;;
         l)
             LOG=$OPTARG
             ;;
         p)
             PID=$OPTARG
             ;;
         a)
             ARGS=$OPTARG
             ;;
         v)
             VERBOSE=1
             ;;
         ?)
             usage
             exit
             ;;
     esac
done

if [[ -z $DAEMON ]]
then
     usage
     exit 1
fi

#If PID is not set we compute where the pid file should live
#based on user level. Let's just start by making the pid named the same
#as the basename of the daemon
if [[ -z $PID ]]; then
    PID_FILE=`basename $DAEMON`
    #Start by checking obvious locations
    if [ -d "/var/run/$PID_FILE" ]; then
        #if the daemon has a folder in /var/run
        PID_PATH="/var/run/${PID_FILE}/"
    elif [ -d "/usr/local/var/run/$PID_FILE" ]; then
        #if the daemon has a folder in /usr/local/var/run
        PID_PATH="/usr/local/var/run/$PID_FILE/"
    else
        PID_PATH="/"
        if [ ! `id -u` == "0" ]; then
			#if we are not running as root
            PID_PATH="${PID_PATH}usr/local/"
        fi
        PID_PATH="${PID_PATH}var/run/"
	fi
    PID="${PID_PATH}${PID_FILE}.pid"
fi

if [ $VERBOSE -gt 0 ]; then
    echo "STARTING DAEMON..."
fi

#test that the pid file exists and is writable
if [ ! -w $PID ]; then
    #try to create it
    touch $PID
    #check that we created it
    if [$? == 1 ]; then
        #if not die a horrible death
        if [ $VERBOSE -gt 0 ]; then 
            echo "PID FILE NOT WRITABLE: ${PID}"
        fi
        exit 1
    fi
fi

if [ $VERBOSE -gt 0 ]; then
    echo "PID FILE: ${PID}"
fi

#redirect stdout and stderr if a log file path was passed
if [[ ! -z $LOG ]]; then
    exec $DAEMON $ARGS >> $LOG 2>&1 &
else
    #if no log file path then output goes into the void
    exec $DAEMON $ARGS > /dev/null 2>&1 &
fi

#then we overwrite the start-stop-daemon's pid with this processes
#new one. this process is different than the one start stop daemon
#thinks it is since it is forked again
PROC=$!
echo $PROC > $PID

#It's alive!
if [ $VERBOSE -gt 0 ]; then
    echo "DAEMON: ${DAEMON} started with PID: ${PROC}"
fi

#BYE!
exit 0
