#!/bin/bash
set -ex

# update system
apt-get update
apt-get install -y nginx
cp env/nginx/fastocloud /etc/nginx/sites-available/fastocloud
systemctl restart nginx
