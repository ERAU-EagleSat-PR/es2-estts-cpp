#! /bin/sh
echo "Hi Hayden"
ls -la
version=`cat VERSION`
echo "$version"
hub release show $version
