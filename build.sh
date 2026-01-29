#!/bin/bash

# Patient Data Logger - Build and Installation Script
# This script compiles the kernel module and user-space application

set -e

echo "=========================================="
echo "Patient Data Logger - Build Script"
echo "=========================================="
echo ""

# Check if make is installed
if ! command -v make &> /dev/null; then
    echo "Error: make is not installed"
    exit 1
fi

# Build the project
echo "[1/3] Building kernel module..."
cd driver
make
cd ..

echo "[2/3] Building user-space application..."
cd app
make
cd ..

echo "[3/3] Build complete!"
echo ""
echo "=========================================="
echo "Build successful!"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. Load the kernel module:"
echo "   sudo insmod driver/patient_vitals_driver.ko"
echo ""
echo "2. Verify the device was created:"
echo "   ls -l /dev/patient_vitals"
echo ""
echo "3. Run the data logger:"
echo "   ./app/patient_data_logger"
echo ""
echo "4. To stop, press Ctrl+C"
echo ""
echo "5. Unload the module:"
echo "   sudo rmmod patient_vitals_driver"
echo ""
