#!/bin/bash

# Auto deploy script for cpp_server_cppcms
# Check Updates from Github -> Downloads latest code from master -> Build + Install -> Restart
# Author: Terence Chow
# Version 1.2

# Directory variables
CMAKE_DIR="ims_cpp_cmake"
SRC_DIR="ims_cpp"
WWW_DIR="/var/www/ims_cpp"
WWW_APP_NAME="cpp_server_cppcms"


# Check for latest code base
echo "Checking updates..."
git remote update
local_rev=`git rev-parse @{0}`
remote_rev=`git rev-parse origin/master`
base_rev=`git merge-base @{0} origin/master`

if [ ${local_rev} = ${remote_rev} ]; then
    # Already up to date
    echo "No updates available."

elif [ ${remote_rev} = ${base_rev} ]; then
    # Local Branch is ahead of remote
    echo "Local branch is ahead of remote master branch! Consider git stash or git push."
 
elif [ ${local_rev} = ${base_rev} ]; then
    # Update available

    # Create CMake directory
    if [ ! -d "../${CMAKE_DIR}" ]; then
    	mkdir "../${CMAKE_DIR}"
    fi

    # Kill current server process
    pid=`ps -Ao "%p|%a" | grep -v "grep" | grep ${WWW_APP_NAME} | cut -d"|" -f1`
    if [ ! -z ${pid} ]; then
        echo "Killing ${WWW_APP_NAME} at ${pid}..."
        kill ${pid}
    fi
    
    echo "Downloading updates..."
    git pull

    # Build
    echo "Building..."
    cd ../${CMAKE_DIR}
    cmake ../${SRC_DIR}
    if [ $? -eq 0 ]; then
        make 
        # Install
        sudo cp cpp_server_cppcms/${WWW_APP_NAME} ${WWW_DIR}

        # Restart
        echo "Restarting Server..."
        ${WWW_DIR}/${WWW_APP_NAME} -c ${WWW_DIR}/config.js &

    else
        echo "CMake failed!"
    fi
fi
