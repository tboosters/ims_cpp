#!/bin/bash

# Auto deploy script for cpp_server_cppcms
# Downloads latest code from github -> Build + Install -> Restart
# Author: Terence Chow
# Version 1.1

# Directory variables
CMAKE_DIR="ims_cpp_cmake"
SRC_DIR="ims_cpp"
WWW_DIR="/var/www/ims_cpp"
WWW_APP_NAME="ims_cpp"

# Create CMake directory
if [ ! -d "../${CMAKE_DIR}" ]
then
	mkdir "../${CMAKE_DIR}"
fi

# Download latest code base
echo "Downloading updates..."
git pull

# Build
echo "Building..."
cd ../${CMAKE_DIR}
cmake ../${SRC_DIR}
if [ $? -eq 0 ]
then
    make

    # Install
    sudo cp cpp_server_cppcms/cpp_server_cppcms ${WWW_DIR}/${WWW_APP_NAME}

    # Restart
    echo "Restarting Server..."
<<<<<<< Updated upstream
    pid=`ps -Ao "%p|%a" | grep -v "grep" | grep ${WWW_APP_NAME} | cut -d"|" -f1`
    if [ ! -z ${pid} ]
    then
	kill ${pid}
    fi
    ${WWW_DIR}/${WWW_APP_NAME} -c ${WWW_DIR}/config.js &
=======
    pid=`ps -Ao "%p|%a" | grep -v "grep" | grep ims_cpp | cut -d"|" -f1`
    kill ${pid}
    /var/www/ims_cpp -c config.js &
>>>>>>> Stashed changes

else
	echo "CMake failed!"
fi
