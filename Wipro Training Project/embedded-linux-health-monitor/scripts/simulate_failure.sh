#!/bin/bash
echo "Simulating service failure to test Auto-Recovery Agent..."

# Test stopping a background service
service_name="cron"
echo "Stopping $service_name service..."
sudo systemctl stop $service_name

echo "Running Health Monitor to trigger auto-recovery..."
./build/health_monitor config/health_monitor.json --once

echo "Simulation test complete. Check health_monitor.log for recovery events!"
