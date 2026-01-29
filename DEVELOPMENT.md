# Development Guide

## Setting Up Development Environment

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential linux-headers-$(uname -r) \
    git vim valgrind gdb cscope ctags git-flow

# Fedora/RHEL
sudo dnf groupinstall "Development Tools"
sudo dnf install kernel-devel kernel-debuginfo valgrind gdb git
```

### Editor Setup (Optional)

#### VS Code

```bash
# Install extensions
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode-linux.systemd-unit-file
```

#### Vim/Neovim

```bash
# Install LSP support
# Use clangd for C/C++ support
```

## Project Structure

```
embedded-linux-patient-data-logger/
├── driver/                      # Kernel module
│   ├── patient_vitals_driver.c  # Main driver code
│   └── Makefile
├── app/                         # User application
│   ├── patient_data_logger.c    # Main application code
│   └── Makefile
├── include/                     # Shared headers
│   └── patient_data.h
├── Makefile                     # Top-level build
├── config.mk                    # Build configuration
├── run.sh                       # Quick start
├── build.sh                     # Build script
├── install.sh                   # Installation script
├── README.md                    # Main documentation
├── ARCHITECTURE.md              # Design documentation
└── GETTING_STARTED.md           # Quick start guide
```

## Building for Development

### Debug Build

```bash
# Add debugging symbols
CFLAGS="-g -O0" make clean make

# Build driver with debug info
cd driver
make EXTRA_CFLAGS="-g -O0"
```

### Testing the Build

```bash
# Check for compilation warnings
make clean
make 2>&1 | grep -i warning

# Analyze driver size
nm driver/patient_vitals_driver.ko | sort

# Check application for symbols
nm app/patient_data_logger
```

## Development Workflow

### 1. Make Changes

```bash
# Example: Modify vital signs ranges
vim driver/patient_vitals_driver.c

# Edit the patient_data initialization:
# Change heart_rate = 72 to 80
# Change systolic = 120 to 130
```

### 2. Rebuild

```bash
# Rebuild driver
cd driver
make clean
make
cd ..

# Or rebuild everything
make clean
make
```

### 3. Test Changes

```bash
# Quick test
./run.sh 2 test.log 5

# With kernel debugging
./run.sh 1 debug.log 3

# View kernel logs
dmesg | tail -20
```

### 4. Commit Changes

```bash
git add -A
git commit -m "feat: modify vital signs ranges for realism"
git push origin main
```

## Debugging Techniques

### Kernel Module Debugging

#### Using dmesg

```bash
# View all kernel messages
dmesg

# View only patient module messages
dmesg | grep -i patient

# Follow in real-time
dmesg -f

# Clear kernel log
sudo dmesg -C
```

#### Using printk

Add debug statements to the driver:

```c
printk(KERN_DEBUG "Patient vitals: HR=%d\n", patient_data.heart_rate);
```

Compile with debug level:

```bash
cd driver
make EXTRA_CFLAGS="-DDEBUG"
```

#### Using GDB for User-Space App

```bash
# Compile with debug symbols
CFLAGS="-g -O0" make -C app

# Run with GDB
gdb -args ./app/patient_data_logger

# In gdb:
(gdb) break main
(gdb) run
(gdb) step
(gdb) print vitals
(gdb) continue
```

#### Using Valgrind for Memory Issues

```bash
# Check for memory leaks
valgrind --leak-check=full ./app/patient_data_logger 5 test.log 3

# Detailed memory analysis
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all \
    ./app/patient_data_logger
```

### Kernel Module Debugging with kdb

```bash
# Enable kernel debugger
sudo echo 1 > /proc/sys/kernel/sysrq

# Access kdb (if compiled in)
sudo sysrq-trigger
```

## Code Quality

### Static Analysis

```bash
# Check for issues with cppcheck
cppcheck driver/
cppcheck app/

# With strict settings
cppcheck --strict --enable=all driver/
```

### Code Coverage

```bash
# Compile with coverage support
CFLAGS="-coverage -fprofile-arcs -ftest-coverage" make -C app

# Run application
./app/patient_data_logger 1 test.log 10

# Generate coverage report
gcov app/patient_data_logger.c
cat app/patient_data_logger.c.gcov
```

## Adding New Features

### Example: Add Blood Oxygen Trend

#### Step 1: Modify Header

```c
// include/patient_data.h
typedef struct {
    int heart_rate;
    int systolic;
    int diastolic;
    int temperature;
    int spo2;
    int spo2_trend;  // NEW: -1 (falling), 0 (stable), +1 (rising)
} patient_vitals_t;
```

#### Step 2: Update Driver

```c
// driver/patient_vitals_driver.c
static void update_patient_vitals(void)
{
    // ... existing code ...
    
    // NEW: Calculate SpO2 trend
    if (new_spo2 > prev_spo2)
        patient_data.spo2_trend = 1;
    else if (new_spo2 < prev_spo2)
        patient_data.spo2_trend = -1;
    else
        patient_data.spo2_trend = 0;
    
    prev_spo2 = patient_data.spo2;
}
```

#### Step 3: Update Application

```c
// app/patient_data_logger.c
void display_vitals(const patient_vitals_t *vitals)
{
    // ... existing code ...
    
    // NEW: Display trend
    printf("SpO2 Trend:      ");
    if (vitals->spo2_trend > 0)
        printf("↑ (Rising)\n");
    else if (vitals->spo2_trend < 0)
        printf("↓ (Falling)\n");
    else
        printf("→ (Stable)\n");
}
```

#### Step 4: Test

```bash
make clean
make
./run.sh 2 trend_test.log 10
```

## Performance Profiling

### Measuring Read Latency

```bash
# Add timing code to app
// Create a modified version with measurements

// Before read
gettimeofday(&start, NULL);

// Read operation
read(device_fd, &vitals, sizeof(patient_vitals_t));

// After read
gettimeofday(&end, NULL);
elapsed = (end.tv_sec - start.tv_sec) * 1000 +
          (end.tv_usec - start.tv_usec) / 1000;

printf("Read latency: %ld ms\n", elapsed);
```

### CPU Profiling

```bash
# Use perf tool
sudo perf record -g ./app/patient_data_logger 1 test.log 100
sudo perf report

# Or with top
top -p $(pgrep patient_data_logger)
```

## Memory Analysis

### Memory Leaks

```bash
# Run with valgrind
valgrind --leak-check=full --track-origins=yes \
    ./app/patient_data_logger 1 test.log 50
```

### Kernel Module Memory

```bash
# Check module memory usage
cat /proc/modules | grep patient_vitals

# Detailed info
cat /sys/module/patient_vitals_driver/sections/
```

## Testing Strategy

### Unit Testing

Create `tests/` directory:

```bash
mkdir tests
cat > tests/test_vitals.c << 'EOF'
#include <assert.h>
#include "../include/patient_data.h"

void test_vitals_initialization() {
    patient_vitals_t vitals;
    vitals.heart_rate = 72;
    assert(vitals.heart_rate >= 60 && vitals.heart_rate <= 100);
}

int main() {
    test_vitals_initialization();
    printf("All tests passed!\n");
    return 0;
}
EOF
```

### Integration Testing

```bash
# Test 1: Normal operation
./run.sh 2 test1.log 5

# Test 2: Rapid reads
./run.sh 1 test2.log 20

# Test 3: Long running
./run.sh 10 test3.log 360  # 1 hour of data

# Verify logs
wc -l test*.log
```

## Code Style

### Kernel Code Style (driver/)

Follow Linux kernel coding style:

```c
// Good
int get_heart_rate(void)
{
    return patient_data.heart_rate;
}

// Bad
int get_heart_rate() { return patient_data.heart_rate; }
```

### User-Space Code Style (app/)

Follow POSIX C conventions:

```c
// Good
int result = read(device_fd, &vitals, sizeof(patient_vitals_t));
if (result < 0) {
    perror("read");
    return -1;
}

// Bad
int result=read(device_fd,&vitals,sizeof(patient_vitals_t));
if(result<0){perror("read");return -1;}
```

## Documentation

### Adding Code Comments

```c
/**
 * update_patient_vitals - Simulate realistic vital sign variations
 *
 * This function updates patient vital signs with small random variations
 * to simulate realistic medical monitoring scenarios. Variations are
 * constrained to ensure values stay within normal medical ranges.
 */
static void update_patient_vitals(void)
{
    // Implementation
}
```

### Updating README

After adding features, update relevant sections:

```markdown
## New Feature

### Usage

```bash
command example
```

### Parameters

- param1: Description
- param2: Description
```

## Version Management

### Semantic Versioning

```
MAJOR.MINOR.PATCH

- MAJOR: Incompatible API changes
- MINOR: New features, backward compatible
- PATCH: Bug fixes
```

### Creating a Release

```bash
# Tag a release
git tag -a v1.1.0 -m "Add new features"
git push origin v1.1.0

# View tags
git tag -l
```

## Contributing to the Project

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Make changes and test thoroughly
4. Commit with descriptive messages
5. Push to your fork
6. Create a Pull Request

### Commit Message Format

```
type: subject

body

footer
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding tests

**Example:**
```
feat: add blood oxygen trend calculation

Calculate the trend of oxygen saturation levels
over time to help identify deteriorating patient
conditions early.

Relates-to: #42
```

## Troubleshooting Common Development Issues

### Module Won't Load

```bash
# Check for version mismatch
modinfo driver/patient_vitals_driver.ko
uname -r

# Rebuild if versions don't match
make clean
make
```

### Symbol Not Found

```bash
# Check exported symbols
nm driver/patient_vitals_driver.ko | grep symbol_name

# Ensure function is declared properly
```

### Permission Issues During Test

```bash
# Use run.sh which handles sudo
./run.sh

# Or manually add user to group
sudo usermod -aG kvm $(whoami)
sudo usermod -aG dialout $(whoami)
```

## Resources

- [Linux Device Drivers, 3rd Edition](https://lwn.net/Kernel/LDD3/)
- [Linux Kernel Documentation](https://www.kernel.org/doc/html/)
- [Linux Kernel Coding Style](https://www.kernel.org/doc/html/latest/process/coding-style.html)
- [Character Devices in Linux](https://www.kernel.org/doc/html/latest/driver-api/basics.html)

---

**Happy Developing!** 🚀
