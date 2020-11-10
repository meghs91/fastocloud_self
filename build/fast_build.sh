#!/bin/bash
set -ex

# exports
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig

# variables
USER=fastocloud

# update system
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    if [ -n "$(command -v yum)" ]; then
      yum update
      yum install -y git python3-setuptools python3-pip
    elif [ -n "$(command -v apt-get)" ]; then
      apt-get update
      apt-get install -y git python3-setuptools python3-pip --no-install-recommends
    else
:
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
:
elif [[ "$OSTYPE" == "cygwin" ]]; then
:
elif [[ "$OSTYPE" == "msys" ]]; then
:
elif [[ "$OSTYPE" == "win32" ]]; then
:
elif [[ "$OSTYPE" == "freebsd"* ]]; then
:
else
:
fi

# sync modules
git submodule update --init --recursive

# install pyfastogt
git clone https://github.com/fastogt/pyfastogt
cd pyfastogt
python3 setup.py install
cd ../
rm -rf pyfastogt

# build env for service
./env/build_env.py

# build service
./build.py release

# add user
useradd -m -U -d /home/$USER $USER -s /bin/bash
