#!/bin/bash

# apt stuff
sudo apt update

# ruby stuff
curl -sSL https://get.rvm.io | bash -s stable
rvm install 2.7
rvm use 2.7 --default
gem install ceedling

# pip stuff
python3 -m pip --version
python3 -m pip3 install -r requirements.txt