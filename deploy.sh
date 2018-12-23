#!/bin/bash

# Auto deploy script for cpp_server_cppcms
# Downloads latest code from github -> Build + Install -> Restart
# Author: Terence Chow
# Version 1.0

# Create CMake directory
if [ ! -d "../ims_cpp_cmake" ]
then
	mkdir ims_cpp_cmake
fi

# Download latest code base
echo "Downloading updates..."
git pull

# Build
echo "Building..."
cd ../ims_cpp_cmake
cmake ../ims_cpp
if [ $? -eq 0 ]
then
	make

    # Install
    sudo cp cpp_server_cppcms/cpp_server_cppcms /var/www/ims_cpp

    # Restart
    echo "Restarting Server..."
    pid=`ps -Ao "%p|%a" | grep -v "grep" | grep ims_cpp | cut -d"|" -f1`
    kill ${pid}
    ./ims_cpp -c config.js &

else
	echo "CMake failed!"
fi
