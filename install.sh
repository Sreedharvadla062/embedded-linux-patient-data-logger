#!/bin/bash

# Patient Data Logger - Installation Script
# Installs the kernel module and user-space application to system directories

set -e

if [ "$EUID" -ne 0 ]; then
    echo "Error: This script must be run with sudo"
    exit 1
fi

echo "Installing Patient Data Logger..."
echo ""

# Install kernel module
echo "Installing kernel module..."
mkdir -p /lib/modules/$(uname -r)/extra
install -m 644 driver/patient_vitals_driver.ko /lib/modules/$(uname -r)/extra/
depmod

# Install user-space application
echo "Installing user-space application..."
install -m 755 app/patient_data_logger /usr/local/bin/

# Create udev rule
echo "Creating udev rule..."
cat > /etc/udev/rules.d/99-patient-vitals.rules << 'EOF'
KERNEL=="patient_vitals", MODE="0666"
EOF

udevadm control --reload-rules
udevadm trigger

echo ""
echo "Installation complete!"
echo ""
echo "To use the driver:"
echo "1. Load the module: sudo modprobe patient_vitals_driver"
echo "2. Run the logger: patient_data_logger"
echo ""
