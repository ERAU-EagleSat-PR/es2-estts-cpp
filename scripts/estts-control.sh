#!/bin/bash

Install () {
  cwd=$(pwd)
  sourcedir="$cwd/.."
  builddir="/tmp/estts_build"
  installdir="/usr/bin"
  supportdir="/opt/estts"
  logdir="/opt/estts/log"

  echo "Setting source directory to $sourcedir"
  echo "Setting build directory to $builddir"
  echo "Setting support directory to $supportdir"
  echo "Setting log directory to $logdir"

  echo "Running updates"
  apt update &> /dev/null
  apt upgrade -y &> /dev/null

  ldconfig -p | grep libboost_system >/dev/null 2>&1 && {
    echo "Boost is installed on system."
  } || {
    echo "Boost is not installed. Installing it now"
    apt install libboost-all-dev -y &> /dev/null
  }

  if ! command -v cmake &> /dev/null
  then
      echo "CMake is not installed. Installing it now"
      apt install cmake -y &> /dev/null
  else
     echo "Found CMake on system."
  fi

  if ! command -v clang &> /dev/null
    then
        echo "Clang is not installed. Installing it now"
        apt install clang -y &> /dev/null
    else
       echo "Found Clang on system."
    fi

  if [ ! -d $supportdir ]
  then
    echo "$supportdir does not exist. Creating it now"
    mkdir $supportdir
  fi

  if [ ! -d $logdir ]
  then
    echo "$logdir does not exist. creating it now"
    mkdir $logdir
    touch $logdir/estts_errors.log
  fi

  cp "$sourcedir/scripts/estts.service" $supportdir

  echo "Creating build scripts"
  cmake -S "$sourcedir" -B $builddir >/dev/null 2>&1

  echo "Building ESTTS. This will take some time.."
  cd $builddir || exit
  make >/dev/null 2>&1

  if [ ! -d $installdir ]
  then
    echo "$installdir does not exist. creating it now"
    mkdir $installdir
  fi

  echo "Copying $builddir/runtime/estts-runtime to install directory at $installdir."
  cp "$builddir/runtime/estts-runtime" $installdir

  if [ -f /etc/systemd/system/estts.service ]
  then
    rm /etc/systemd/system/estts.service
  fi

  echo "Creating symbolic link from $supportdir/estts.service to /etc/systemd/system/"
  ln -s $supportdir/estts.service /etc/systemd/system/

  echo "Reloading systemd daemon and enabling ESTTS service"
  systemctl daemon-reload
  systemctl enable estts
  systemctl start estts
}

Uninstall () {
  echo "Uninstalling ESTTS"

  systemctl stop estts
  systemctl disable estts

  installdir="/usr/bin"
  supportdir="/opt/estts"

  if [ -f /etc/systemd/system/estts.service ]
  then
    echo "Removing symbolic link to service file"
    rm /etc/systemd/system/estts.service
  fi

  if [ -f /usr/lib/systemd/system/estts.service ]
  then
    echo "Removing symbolic link to lib service file"
    rm /usr/lib/systemd/system/estts.service
  fi

  if [ -f $installdir/estts-runtime ]
  then
    echo "Removing runtime binary"
    rm $installdir/estts-runtime
  fi

  if [ -d $supportdir ]
  then
    echo "Removing estts support directory"
    rm -rf $supportdir
  fi

  systemctl daemon-reload
  systemctl reset-failed

  echo "ESTTS is uninstalled"
}

if [ "$EUID" -ne 0 ]
  then echo "estts-control must be run as root"
  exit
fi

if [ $# -eq 0 ]
  then
    echo "No arguments supplied."
    echo "Usage: ./estts-control.sh [action]"
    echo "Possible actions: install - installs ESTTS and creates systemd service; uninstall - uninstalls ESTTS"
fi

case "$1" in
  install) Install ;;
  uninstall) Uninstall ;;
esac