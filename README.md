# Incident Management System - C++ Server and SDK

(Yet another) Code repository for HKUCS FYP - TMY

## Initial Setup
* Install git on your machine.
* Pull this repository with ```git pull https://github.com/tboosters/ims_cpp.git```.
* Follow the following instructions.

## Dependencies Installation
* This package depends on CPPCMS, RoutingKit and the Serialization Module of Boost and is tested on Ubuntu 18.04 LTS.
* Your machine has to be on Ubuntu / Debian for the ```apt``` command. (TODO: Add Mac support)
* Make sure cmake (version >= 3.10), g++, make, git, wget are available on your machine.
* Run ```install-dependencies.sh``` to install all the libraries on your machine.

## Deploying CPP_SERVER_CPPCMS
* CPP_SERVER_CPPCMS is the web server for serving routing functionalities.
* Run ```deploy.sh``` in the git root directory after each update being pushed to this repository to rebuild and restart the server.
* The server should now run at localhost:8080.
* Read [here](https://www.digitalocean.com/community/tutorials/how-to-set-up-a-node-js-application-for-production-on-ubuntu-16-04#set-up-nginx-as-a-reverse-proxy-server) for the set-up of reverse proxy on Nginx servers to serve CPP_SERVER_CPPCMS to the Internet.

## Example
```bash
git pull https://github.com/tboosters/ims_cpp.git
cd ims_cpp
sudo ./install-dependcies.sh
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
