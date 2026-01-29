#!/bin/bash

# Patient Data Logger - Quick Start Script
# Loads the kernel module and starts the data logger

if [ ! -f "driver/patient_vitals_driver.ko" ]; then
    echo "Error: Kernel module not found. Build the project first: make"
    exit 1
fi

if [ ! -f "app/patient_data_logger" ]; then
    echo "Error: User application not found. Build the project first: make"
    exit 1
fi

echo "Loading kernel module..."
sudo insmod driver/patient_vitals_driver.ko

if [ ! -e "/dev/patient_vitals" ]; then
    echo "Error: Device /dev/patient_vitals was not created"
    echo "Unloading module..."
    sudo rmmod patient_vitals_driver
    exit 1
fi

echo "Device /dev/patient_vitals created successfully"
echo ""
echo "Starting Patient Data Logger..."
echo "Press Ctrl+C to stop"
echo ""

./app/patient_data_logger "$@"

# Cleanup
echo ""
echo "Unloading kernel module..."
sudo rmmod patient_vitals_driver
echo "Done!"
