#!/bin/bash
echo "Installing Embedded Linux Health Monitor..."

mkdir -p build && cd build
cmake .. && make -j4

sudo cp health_monitor /usr/local/bin/
sudo mkdir -p /etc/
sudo cp ../config/health_monitor.json /etc/health_monitor.json
sudo cp ../systemd/device-health-monitor.service /etc/systemd/system/

sudo systemctl daemon-reload
echo "Installation complete! Start daemon using: sudo systemctl start device-health-monitor"
