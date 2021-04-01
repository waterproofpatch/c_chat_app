#!/bin/bash
sudo apt update

curl -sSL https://get.rvm.io | bash -s stable
rvm install 2.7
rvm use 2.7 --default

gem install ceedling

apt-get install python3-pip
pip3 install gcovr
pip install -r requirements.txt