#!/bin/bash

# apt stuff
sudo apt update

# ruby stuff
curl -sSL https://get.rvm.io | bash -s stable
rvm install 2.7
rvm use 2.7 --default
gem install ceedling

# pip stuff
which pip3
python3.6 -m pip3 --version