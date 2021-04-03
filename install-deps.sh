#!/bin/bash

# apt stuff
sudo apt update
sudo apt-get install python3-pip 

# ruby stuff
curl -sSL https://get.rvm.io | bash -s stable
rvm install 2.7
rvm use 2.7 --default
gem install ceedling

# pip stuff
which pip3
python3 -m pip --version
pip3 --version
pip3 install -r requirements.txt