# Getting Started Guide

## Quick Start (5 minutes)

### Prerequisites Check

Verify your system has the necessary tools:

```bash
# Check if kernel headers are installed
uname -r
ls /lib/modules/$(uname -r)/build

# Check compiler
gcc --version
make --version
```

### Build and Run

```bash
# 1. Clone the repository (already done)
cd embedded-linux-patient-data-logger

# 2. Build the project
make

# 3. Run the data logger
./run.sh
```

That's it! You should see patient vital signs being logged every 5 seconds.

## Step-by-Step Setup

### 1. System Setup

#### For Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install build-essential linux-headers-$(uname -r)
```

#### For Fedora/RHEL:

```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install kernel-devel
```

### 2. Clone Repository

```bash
git clone https://github.com/Sreedharvadla062/embedded-linux-patient-data-logger.git
cd embedded-linux-patient-data-logger
```

### 3. Build the Project

```bash
# Build both components
make

# Or build individually
make driver    # Kernel module only
make app       # User application only
```

### 4. Run the Application

**Option A: Quick start (recommended)**

```bash
./run.sh
```

**Option B: Manual steps**

```bash
# Terminal 1: Load module and start application
sudo insmod driver/patient_vitals_driver.ko
./app/patient_data_logger

# Terminal 2: View kernel messages (optional)
dmesg -w

# When done, unload the module
sudo rmmod patient_vitals_driver
```

## Understanding the Output

### Console Output

```
=== Patient Vital Signs ===
Heart Rate:      72 BPM
Blood Pressure:  120/80 mmHg
Temperature:     37.00°C
SpO2:            98%
===========================
```

### Log File

```
[2026-01-29 14:30:45] HR: 72 BPM | BP: 120/80 mmHg | Temp: 37.00°C | SpO2: 98%
```

## Common Tasks

### Change Logging Interval

```bash
# Log every 10 seconds
./run.sh 10

# Log every 1 second (fast mode)
./run.sh 1

# Log every 60 seconds
./run.sh 60
```

### Save to Custom File

```bash
# Save logs to /tmp/vitals.log
./run.sh 5 /tmp/vitals.log

# Take 20 readings and save
./run.sh 5 my_vitals.log 20
```

### View Kernel Messages

```bash
# Real-time kernel logs
sudo dmesg -f

# Or use specific filter
sudo dmesg | grep -i patient

# Follow in real-time
sudo dmesg -w
```

### Check Device Status

```bash
# List character devices
cat /proc/devices | grep patient

# Check device file
ls -l /dev/patient_vitals

# Test read manually
sudo dd if=/dev/patient_vitals bs=20 count=1
```

## Troubleshooting

### "Device not found" Error

```bash
# Make sure module is loaded
sudo lsmod | grep patient_vitals

# If not loaded, load it
sudo insmod driver/patient_vitals_driver.ko

# Verify device was created
ls -l /dev/patient_vitals
```

### Build Fails

```bash
# Check kernel headers
uname -r
ls /lib/modules/$(uname -r)/build

# If missing, install them
sudo apt-get install linux-headers-$(uname -r)

# Clean and rebuild
make clean
make
```

### Permission Denied

```bash
# Run with sudo
sudo ./app/patient_data_logger

# Or use the run.sh script which handles this
./run.sh
```

## Project Files

| File | Purpose |
|------|---------|
| `driver/patient_vitals_driver.c` | Kernel module source code |
| `app/patient_data_logger.c` | User-space application source code |
| `include/patient_data.h` | Shared data structures |
| `Makefile` | Top-level build system |
| `driver/Makefile` | Kernel module build configuration |
| `app/Makefile` | User application build configuration |
| `run.sh` | Quick start script |
| `build.sh` | Build script |
| `install.sh` | System installation script |

## Next Steps

1. **Customize**: Modify the vital signs ranges in [driver/patient_vitals_driver.c](driver/patient_vitals_driver.c)
2. **Integrate**: Add real sensor support
3. **Extend**: Implement IOCTL commands for device control
4. **Deploy**: Install to system with `sudo ./install.sh`

## Performance Testing

### Measure CPU Usage

```bash
# Start application
./run.sh &

# Monitor in another terminal
top -p $(pgrep patient_data_logger)
```

### Generate Large Log File

```bash
# Take 1000 readings with 1-second interval
./run.sh 1 large_log.log 1000

# Check file size
wc -l large_log.log
du -h large_log.log
```

## Security Notes

- The kernel module runs with full privileges
- Device is world-readable by default
- For production use, implement access controls via udev rules

## Support & Help

- Check [README.md](README.md) for detailed documentation
- Review kernel logs: `dmesg`
- Test on a clean Linux system if issues persist

---

**Happy Monitoring!** 🏥
