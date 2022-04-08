#!/bin/bash

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