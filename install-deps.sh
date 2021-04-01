#!/bin/bash

# apt stuff
sudo apt update
sudo apt-get install python3.6
# sudo apt-get install python3-pip
python --version
python3 --version
pip --version
pip3 --version
exit 1

# ruby stuff
curl -sSL https://get.rvm.io | bash -s stable
rvm install 2.7
rvm use 2.7 --default
gem install ceedling

# pip stuff
pip3 --version
pip3 install --upgrade pip
pip3 --version
pip3 install -r requirements.txt