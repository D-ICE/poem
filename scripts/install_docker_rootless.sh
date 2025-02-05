#!/usr/bin/env bash

sudo apt update

## Install Docker
# https://docs.docker.com/engine/install/ubuntu/

# Add Docker's official GPG key:
sudo apt-get update
sudo apt-get install -y ca-certificates curl
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc

# Add the repository to Apt sources:
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update

# Install the Docker packages
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

# Verify that the installation is successful by running the hello-world image
sudo docker run hello-world

## Install Docker ROOTLESS
# https://docs.docker.com/engine/security/rootless/

sudo apt-get install -y dbus-user-session
sudo apt-get install -y uidmap

sudo systemctl disable --now docker.service docker.socket
sudo rm /var/run/docker.sock

dockerd-rootless-setuptool.sh install

docker run hello-world

pip install cibuildwheel==2.22.0
