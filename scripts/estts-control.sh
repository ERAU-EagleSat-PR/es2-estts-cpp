#!/bin/bash

Install () {
   echo "Install"

   cwd=$(pwd)
   sourcedir="$cwd/.."
   builddir="/tmp/estts_build/"
   installdir="/usr/bin"

   echo "Setting source directory to $sourcedir"
   echo "Setting build directory to $builddir"

   if [ ! -d $builddir ]
   then
     mkdir $builddir
   fi

   cmake -S "$sourcedir" -B $builddir
   cd $builddir || exit
   make

   if [ ! -d $installdir ]
   then
     mkdir $installdir
   fi

   cp "$builddir/runtime/estts-runtime" $installdir
}

Uninstall () {
   echo "Uninstall"
}

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
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
