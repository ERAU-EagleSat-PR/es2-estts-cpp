#!/bin/bash

Install () {
  cwd=$(pwd)
  sourcedir="/tmp/esttsinstall"
  installdir="/usr/bin"
  supportdir="/opt/estts"
  logdir="/opt/estts/log"

  echo "Setting source directory to $sourcedir"
  echo "Setting support directory to $supportdir"
  echo "Setting log directory to $logdir"
  echo "Setting install directory to $installdir"

  # Install dependencies
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

  # Verify that the support directories exists
  if [ ! -d $sourcedir ]
  then
    echo "$sourcedir does not exist. Creating it now"
    mkdir $sourcedir
  else
    echo "$sourcedir exists. Removing it now"
    rm -rf $sourcedir
    mkdir $sourcedir
  fi

  if [ ! -d /tmp/estts ]
  then
    echo "/tmp/estts does not exist. Creating it now"
    mkdir /tmp/estts
  else
    echo "/tmp/estts exists. Removing it now"
    rm -rf /tmp/estts
    mkdir /tmp/estts
  fi

  if [ ! -d $supportdir ]
  then
    echo "$supportdir does not exist. Creating it now"
    mkdir $supportdir
  else
    echo "$supportdir exists. Removing it now"
    rm -rf $supportdir
    mkdir $supportdir
  fi

  if [ ! -d $logdir ]
  then
    echo "$logdir does not exist. creating it now"
    mkdir $logdir
    touch $logdir/estts_errors.log
  else
    echo "$logdir exists. Removing it now"
    rm -rf $logdir
    mkdir $logdir
    touch $logdir/estts_errors.log
  fi

  if [ ! -d $installdir ]
  then
    echo "$installdir does not exist. creating it now"
    mkdir $installdir
  fi

  if [ -d /tmp/estts ]
  then
    rm -rf /tmp/estts
  fi

  if [ -f /etc/systemd/system/estts.service ]
  then
    rm /etc/systemd/system/estts.service
  fi

  # Download dependencies
  echo "Downloading ESTTS"
  git clone https://github.com/ERAU-EagleSat-PR/es2-estts-cpp.git $sourcedir --recurse-submodules || {
    echo "Failed to download ESTTS. Please check your internet connection and try again."
    exit 1
  }

  # Build the project
  echo "Creating build scripts"
  cmake -S "$sourcedir" -B $sourcedir || {
    echo "Failed to create build scripts."
    exit 1
  }

  echo "Building ESTTS. This will take some time.."
  cd $sourcedir || exit
  make -j 4 || {
    echo "Failed to build ESTTS."
    exit 1
  }

  # Create environment file
  echo "Creating environment file"
  touch $supportdir/estts.env
  {
    echo "LL=--log-level";
    echo "LLA=trace";
    echo "CSA=--cosmos-server-addr";
    echo "CSAA=172.19.35.150";
    echo "WLD=--working-log-dir";
    echo "WLDA=/tmp/estts/";
    echo "BGD=--base-git-dir";
    echo "BGDA=/home/sbeve/telemetry";
    echo "TGR=--telemetry-git-repo";
    echo "TGRA=git@github.com:ERAU-EagleSat-PR/eaglesat-2-telemetry.git";
  } >> $supportdir/estts.env

  # Move downloaded files to their locations
  echo "Copying $sourcedir/scripts/estts.service to support directory at $supportdir."
  cp "$sourcedir/scripts/estts.service" $supportdir || {
    echo "Failed to copy estts.service to support directory."
    exit 1
  }

  # Rename binary
  mv "$sourcedir/runtime/estts-runtime" "$sourcedir/estts" || {
    echo "Failed to rename estts-runtime to estts."
    exit 1
  }

  # Copy binary to bin directory
  echo "Copying $sourcedir/estts to bin directory at $installdir."
  cp "$sourcedir/estts" $installdir || {
    echo "Failed to copy estts to install directory."
    exit 1
  }

  # Create symbolic link between $supportdir and /etc/systemd/system
  echo "Creating symbolic link from $supportdir/estts.service to /etc/systemd/system/"
  ln -s $supportdir/estts.service /etc/systemd/system/ || {
    echo "Failed to create symbolic link from $supportdir/estts.service to /etc/systemd/system/"
    exit 1
  }

  echo "Reloading systemd daemon and enabling ESTTS service"
  systemctl daemon-reload
  systemctl enable estts
  systemctl start estts

  echo "Cleaning up"
  rm -rf $sourcedir
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

  if [ -f $installdir/estts ]
  then
    echo "Removing runtime binary"
    rm $installdir/estts
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