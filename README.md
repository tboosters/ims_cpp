# Incident Management System - C++ Server and SDK (UNIX only)

(Yet another) Code repository for HKUCS FYP - TMY

## Pulling Code
* Install git on your machine.
* Pull this repository with ```git pull https://github.com/tboosters/ims_cpp.git```.
* Follow the following instructions.

## Installing Dependencies
* This package depends on CPPCMS, RoutingKit and the Serialization Module of Boost and is tested on Ubuntu 18.04 LTS.
* Your machine has to be on Ubuntu / Debian for the ```apt``` command. (TODO: Add Mac support)
* Make sure cmake (version >= 3.10), g++, make, git, wget are available on your machine.
* Run ```sudo ./install-dependencies.sh``` to install all the libraries on your machine.

## Building Binaries
* Make sure CMake is installed.
* Create a folder ```ims_cpp_cmake``` at the same directory of ```ims_cpp```.
* ```cd``` into ```ims_cpp_cmake```
* Initialize the CMake files: ```cmake ../ims_cpp```
* Build the binaries: ```make```
* The binary files will be under the folder of each module.

## Using Graph Builder
* Graph Builder is a CLI tool for converting and serializing a OSM PBF map file to the MapGraph data structure used in CPP_SERVER_CPPCMS.
* A serialized MapGraph file is required to start CPP_SERVER_CPPCMS, therefore this tool must be run at least once to provide the required file for the server.
* Refer to docs in Google Drive for more details.

## Deploying (Changes to) CPP_SERVER_CPPCMS
* CPP_SERVER_CPPCMS is the web server for serving routing functionalities.
* For initial deployment: Create MapGraph file ```HK.graph``` with Graph Builder. Copy ```HK.graph``` and ```config.js``` under ```ims_cpp/cpp_server_cppcms``` to ```/var/www/ims_cpp```.
* Run ```sudo ./deploy.sh``` in the git root directory after each update being pushed to this repository to rebuild and restart the server. No rebuild will occur if the code base is already up-to-date. (TODO: Add option for user defined MapGraph file path)
* The server should now run at localhost:8080.

## Example
```bash
# Pulling Code
git pull https://github.com/tboosters/ims_cpp.git

# Installing Dependencies
cd ims_cpp
sudo ./install-dependcies.sh

# Building Binaries
cd ../
mkdir ims_cpp_cmake
cd ims_cpp_cmake
cmake ../ims_cpp
make

# Using Graph Builder
cd graph_builder
./graph_builder # Create MapGraph file with UI inside, HK.graph placed in same directory

# Deploying CPP_SERVER_CPPCMS
sudo cp HK.graph /var/www/ims_cpp
sudo cp ../../ims_cpp/cpp_server_cppcms/config.js /var/www/ims_cpp
sudo ./deploy.sh
```

## Setting Up Reverse Proxy Server on Nginx
* ```sudo vim /etc/nginx/sites-available/default```
* Modify the follows
```
. . .
    location / {
        proxy_pass http://localhost:<Port of CPP_SERVER_CPPCMS>;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }
}
```
* Syntax check with ```sudo nginx -t```
* Restart Nginx with ```sudo systemctl restart nginx```
