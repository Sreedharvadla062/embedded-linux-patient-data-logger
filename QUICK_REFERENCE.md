# Quick Reference Card

## Installation Commands

```bash
# Ubuntu/Debian
sudo apt-get install build-essential linux-headers-$(uname -r)

# Fedora/RHEL  
sudo dnf groupinstall "Development Tools" && sudo dnf install kernel-devel
```

## Build Commands

```bash
make              # Build everything
make driver       # Build kernel module only
make app          # Build application only
make clean        # Remove build artifacts
make help         # Show help
```

## Running the Project

```bash
# Quick start (recommended)
./run.sh

# With custom interval (seconds)
./run.sh 10

# Custom log file
./run.sh 5 /tmp/vitals.log

# Limited number of readings
./run.sh 5 vitals.log 20
```

## Manual Setup

```bash
# Load kernel module
sudo insmod driver/patient_vitals_driver.ko

# Verify device created
ls -l /dev/patient_vitals

# Run application
./app/patient_data_logger

# Unload module
sudo rmmod patient_vitals_driver
```

## Debugging Commands

```bash
# View kernel messages
dmesg | grep -i patient
dmesg -w              # Follow in real-time

# Check device status
cat /proc/devices | grep patient
lsmod | grep patient

# View device info
modinfo driver/patient_vitals_driver.ko

# Manual device read
sudo dd if=/dev/patient_vitals bs=20 count=1
```

## File Locations

| File | Purpose |
|------|---------|
| `driver/patient_vitals_driver.c` | Kernel module |
| `app/patient_data_logger.c` | User application |
| `include/patient_data.h` | Shared headers |
| `Makefile` | Build system |
| `run.sh` | Quick start script |
| `/dev/patient_vitals` | Device node |
| `patient_data.log` | Default log file |

## Vital Signs Values

| Parameter | Unit | Default | Range |
|-----------|------|---------|-------|
| Heart Rate | BPM | 72 | 60-100 |
| Systolic BP | mmHg | 120 | 100-140 |
| Diastolic BP | mmHg | 80 | 60-90 |
| Temperature | °C | 37.00 | 36.50-37.50 |
| SpO2 | % | 98 | 95-100 |

## Common Issues & Fixes

| Issue | Solution |
|-------|----------|
| Device not found | `sudo insmod driver/patient_vitals_driver.ko` |
| Permission denied | Use `./run.sh` or `sudo` |
| Build fails | Check kernel headers: `uname -r` vs `/lib/modules/*/build` |
| Module won't load | Run `make clean && make` to rebuild |
| Incomplete read | Verify device: `cat /proc/devices \| grep patient` |

## Performance Metrics

- **CPU Usage**: < 1%
- **Memory (module)**: ~50 KB
- **Memory (app)**: ~30 KB
- **Read latency**: < 1ms
- **Device throughput**: 20 bytes per read

## Environment Variables

```bash
# Build with custom kernel
make -C /lib/modules/$(uname -r)/build

# Custom compiler flags
CFLAGS="-g -O0" make
EXTRA_CFLAGS="-DDEBUG" make -C driver
```

## File Permissions

```bash
# Make scripts executable
chmod +x run.sh build.sh install.sh

# View device permissions
ls -l /dev/patient_vitals

# Modify device permissions (if needed)
sudo chmod 666 /dev/patient_vitals
```

## Signal Handling

```bash
Ctrl+C    - Stop application and unload module
Ctrl+Z    - Suspend (not recommended)
Ctrl+\    - Quit (forceful)
```

## Useful Utilities

```bash
# Monitor system
watch -n 1 'cat /proc/devices | grep patient'
top -p $(pgrep patient_data_logger)

# Check kernel version
uname -r

# Find device major number
grep patient /proc/devices

# Test device manually
sudo dd if=/dev/patient_vitals bs=20 count=1

# Count log entries
wc -l patient_data.log
```

## Documentation Files

| File | Content |
|------|---------|
| `README.md` | Complete documentation |
| `GETTING_STARTED.md` | 5-minute quick start |
| `ARCHITECTURE.md` | System design & flow |
| `DEVELOPMENT.md` | Development guide |
| `QUICK_REFERENCE.md` | This file |

## Quick Troubleshooting Flowchart

```
Problem?
  ├─ Device not found
  │  └─ Run: sudo insmod driver/patient_vitals_driver.ko
  │
  ├─ Permission denied
  │  └─ Run: ./run.sh (with sudo built-in)
  │
  ├─ Build error
  │  └─ Check: uname -r && ls /lib/modules/$(uname -r)/build
  │     Fix: sudo apt-get install linux-headers-$(uname -r)
  │
  ├─ Module won't load
  │  └─ Run: make clean && make
  │
  └─ Still not working?
     └─ Check: dmesg | grep patient
        View: cat /proc/devices | grep patient
```

## Essential Commands to Remember

```bash
# The only command you need most of the time
./run.sh

# For development
make && ./run.sh 1 test.log 10

# For debugging
dmesg -w &
./run.sh

# For deployment
sudo ./install.sh
patient_data_logger
```

---

**Print this page for quick reference during development!** 📋
