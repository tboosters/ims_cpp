#!/bin/sh

# Dependency installation script for IMS_CPP server
# Installs CPPCMS, Boost, RoutingKit
# Run in sudo
# Prerequisite: CMake 3.10+, GNU GCC Compiler, Make, Git, wget
# Author: Terence Chow
# Version 1.0

# Create working directory
if [ ! -d "temp" ]
then
	mkdir temp
fi
cd temp

# CPPCMS
echo "Installing CPPCMS..."
apt install libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python
wget https://nchc.dl.sourceforge.net/project/cppcms/cppcms/1.2.1/cppcms-1.2.1.tar.bz2
tar --bzip2 -xf cppcms-1.2.1.tar.bz2
cd cppcms-1.2.1
mkdir build
cd build
cmake ..
make -j4
make install
cd ../../

# Boost
echo "Installing Boost..."
wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2
tar --bzip2 -xf boost_1_69_0.tar.bz2
cd boost_1_69_0
./bootstrap.sh --with-libraries=serialization
if [ $? -eq 0 ]
then
	./b2 install
else
	echo "Boostrapping failed!"
fi
cd ../
echo ""

# RoutingKit
echo "Installing RoutingKit..."
apt install zlib1g-dev
git clone https://github.com/RoutingKit/RoutingKit.git
cd RoutingKit
make
cp -r include/* /usr/local/include
cp -r lib/* /usr/local/lib
echo""

echo "Done."
