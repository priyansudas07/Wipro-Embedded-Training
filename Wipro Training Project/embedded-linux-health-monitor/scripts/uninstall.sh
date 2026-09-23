#!/bin/bash
echo "Uninstalling Embedded Linux Health Monitor..."

sudo systemctl stop device-health-monitor 2>/dev/null
sudo systemctl disable device-health-monitor 2>/dev/null
sudo rm -f /usr/local/bin/health_monitor
sudo rm -f /etc/health_monitor.json
sudo rm -f /etc/systemd/system/device-health-monitor.service
sudo systemctl daemon-reload

echo "Uninstallation complete."
