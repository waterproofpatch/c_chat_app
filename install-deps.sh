#!/bin/bash

# apt stuff
sudo apt update
sudo apt-get install python3
sudo apt-get install python3-pip

# ruby stuff
curl -sSL https://get.rvm.io | bash -s stable
rvm install 2.7
rvm use 2.7 --default
gem install ceedling

# pip stuff
python3 -m pip install --upgrade pip
python3 -m pip install -r requirements.txt