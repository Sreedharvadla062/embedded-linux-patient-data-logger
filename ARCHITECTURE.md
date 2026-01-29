# Architecture & Design

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│             Embedded Linux Patient Data Logger              │
└─────────────────────────────────────────────────────────────┘
                              │
                    ┌─────────┴─────────┐
                    │                   │
        ┌───────────▼──────────┐  ┌────▼──────────────┐
        │  Kernel Space (Ring 0)│  │ User Space (Ring 3)│
        │                      │  │                   │
        │  Character Device    │  │ Data Logger App   │
        │  Driver              │  │                   │
        │                      │  │ - Read device     │
        │  - Simulate vitals   │  │ - Display data    │
        │  - Provide /dev node │  │ - Log to file     │
        │  - Handle I/O ops    │  │ - Signal handling │
        └──────────┬───────────┘  └─────┬─────────────┘
                   │                    │
                   │ read() system call │
                   └────────┬───────────┘
                            │
                   ┌────────▼────────┐
                   │  /dev/patient   │
                   │  _vitals        │
                   └─────────────────┘
```

## Component Architecture

### 1. Kernel Module (driver/)

**File**: `patient_vitals_driver.c`

**Responsibilities**:
- Character device management
- Vital signs simulation
- Data structure management
- Kernel logging

**Key Functions**:
```c
device_open()          // Called when device is opened
device_release()       // Called when device is closed
device_read()          // Fetches current vital signs
update_patient_vitals()// Simulates realistic variations
```

**Device Registration**:
- Dynamically allocates device number
- Creates `/dev/patient_vitals`
- Registers file operations

### 2. User-Space Application (app/)

**File**: `patient_data_logger.c`

**Responsibilities**:
- Device communication
- Data display and logging
- Signal handling
- File I/O

**Key Functions**:
```c
main()             // Application entry point
display_vitals()   // Prints vital signs
log_to_file()      // Writes timestamped data to file
signal_handler()   // Handles Ctrl+C gracefully
```

### 3. Shared Header (include/)

**File**: `patient_data.h`

**Contains**:
- Device naming constants
- Patient vital signs structure
- Shared data types

## Data Flow Diagram

```
┌──────────────────────┐
│  Application Start   │
└──────┬───────────────┘
       │
       ▼
┌──────────────────────────────────┐
│ Parse Command Line Arguments     │
│ - interval (default: 5 sec)      │
│ - log file (default: stdout.log) │
│ - max reads (default: infinite)  │
└──────┬───────────────────────────┘
       │
       ▼
┌──────────────────────────────────┐
│ Open /dev/patient_vitals         │
└──────┬───────────────────────────┘
       │
       ▼
┌────────────────────────────────────┐
│ Main Loop (until Ctrl+C)           │
│                                    │
│  ┌─────────────────────────────┐  │
│  │ 1. read() from device       │  │
│  │    (kernel updates vitals)  │  │
│  │                             │  │
│  │ 2. Display on console       │  │
│  │    ┌──────────────────────┐ │  │
│  │    │ HR: 72 BPM           │ │  │
│  │    │ BP: 120/80 mmHg      │ │  │
│  │    │ Temp: 37.00°C        │ │  │
│  │    │ SpO2: 98%            │ │  │
│  │    └──────────────────────┘ │  │
│  │                             │  │
│  │ 3. Log to file (timestamped)│  │
│  │    [2026-01-29 14:30:45]    │  │
│  │    HR: 72 BPM | BP: ...     │  │
│  │                             │  │
│  │ 4. Sleep for interval       │  │
│  │    (default: 5 seconds)     │  │
│  │                             │  │
│  └─────────────────────────────┘  │
└────────────────────────────────────┘
       │
       ▼
┌──────────────────────────────────┐
│ Signal Handler (Ctrl+C received) │
│ - Set exit flag                  │
│ - Close device                   │
│ - Display statistics             │
└──────┬───────────────────────────┘
       │
       ▼
┌──────────────────────────────────┐
│ Cleanup and Exit                 │
└──────────────────────────────────┘
```

## Kernel Module Flow

```
┌─────────────────────────────┐
│ patient_vitals_init()       │
│ (Called on module load)     │
└────────┬────────────────────┘
         │
         ├─► alloc_chrdev_region()
         │   Allocate device number
         │
         ├─► cdev_init()
         │   Initialize character device
         │
         ├─► cdev_add()
         │   Register device with kernel
         │
         ├─► class_create()
         │   Create device class
         │
         └─► device_create()
             Create /dev node
             
┌──────────────────────────┐
│ Device Operations        │
│ (During runtime)         │
└────────┬─────────────────┘
         │
    ┌────┴────┐
    │          │
    ▼          ▼
device_open() device_read()
    │          │
    │          ├─► update_patient_vitals()
    │          │   Simulate realistic variations
    │          │
    │          └─► copy_to_user()
    │              Send data to user space
    │
    └─► Log to kernel ring buffer
        (viewable with dmesg)

┌─────────────────────────────┐
│ patient_vitals_exit()       │
│ (Called on module unload)   │
└────────┬────────────────────┘
         │
         ├─► device_destroy()
         │
         ├─► class_destroy()
         │
         ├─► cdev_del()
         │
         └─► unregister_chrdev_region()
```

## Data Structure

### patient_vitals_t

```c
typedef struct {
    int heart_rate;      /* BPM (60-100) */
    int systolic;        /* mmHg (100-140) */
    int diastolic;       /* mmHg (60-90) */
    int temperature;     /* Celsius * 100 (3650-3750) */
    int spo2;            /* Oxygen saturation % (95-100) */
} patient_vitals_t;
```

**Size**: 20 bytes (5 integers)
**Location**: Shared between kernel and user-space via read()

## Build System Architecture

```
Makefile (top-level)
    │
    ├─► make driver
    │   └─► driver/Makefile
    │       └─► Kernel build system (/lib/modules/*/build)
    │           └─► patient_vitals_driver.ko
    │
    └─► make app
        └─► app/Makefile
            └─► gcc
                └─► patient_data_logger (executable)
```

## Communication Protocol

### System Call Sequence

```
User Space                      Kernel Space
───────────                      ────────────

open("/dev/...")  ──────────┐
                             ├──► device_open()
                             │    • Check permissions
                             │    • Set up file struct
read() ───────────────────┐  │
      (20 bytes expected) │  ├──► device_read()
                          │  │    • update_patient_vitals()
                          │  │    • Generate data with variations
                          │  │    • copy_to_user()
                          │  │    • Log to kernel buffer
      ◄──────────────────┘
   (data returned)

close() ───────────────────┐
                           ├──► device_release()
                           │    • Cleanup resources
                           │    • Log close event
```

## Memory Layout

### User-Space Application

```
┌─────────────────────┐
│   Stack (grows down)│
├─────────────────────┤
│                     │
│   Heap (grows up)   │
├─────────────────────┤
│   BSS (uninitialized)
├─────────────────────┤
│   Data (initialized)│
│   - device_fd       │
│   - vitals struct   │
├─────────────────────┤
│   Text (code)       │
│   - main()          │
│   - read_from_dev() │
│   - log_to_file()   │
└─────────────────────┘
    ~30-50 KB total
```

### Kernel Module

```
┌─────────────────────────┐
│  Kernel Memory Space    │
├─────────────────────────┤
│  Module Code            │
│  - device_read()        │
│  - device_open()        │
├─────────────────────────┤
│  Module Data            │
│  - device_number        │
│  - patient_data struct  │
│  - cdev structure       │
├─────────────────────────┤
│  Device Registration    │
│  - In device table      │
├─────────────────────────┤
│  Class & Device Nodes   │
│  - Sysfs entries        │
└─────────────────────────┘
    ~50-100 KB total
```

## Interrupt & Context Handling

```
User Context (read operation):
├─► copy_to_user()  ← Requires user context
├─► File I/O ready
└─► Scheduling allowed

Kernel Context (module operations):
├─► Can sleep safely
├─► Can use mutex locks
└─► Full kernel services available
```

## Error Handling Paths

```
┌─────────────────────────────────────┐
│ Potential Failure Points            │
└────────────┬────────────────────────┘
             │
    ┌────────┼────────┐
    │        │        │
    ▼        ▼        ▼
alloc_chrdev_region   cdev_add   device_create
failure               failure    failure
│                     │          │
└──────────┬──────────┴──────────┘
           │
    ┌──────▼──────┐
    │  Rollback   │
    │  cleanup    │
    │  return ERR │
    └─────────────┘
```

## Performance Characteristics

| Metric | Value | Notes |
|--------|-------|-------|
| Device Open | <1ms | Minimal initialization |
| Read Operation | <1ms | Simple data copy |
| Write Operation | Error | Not supported |
| CPU Usage | <1% | Mostly sleeping |
| Memory (module) | ~50KB | Resident in memory |
| Memory (app) | ~30KB | User-space heap |
| Data Rate | 20 bytes/read | Fixed structure size |
| Latency | <10ms | End-to-end timing |

---

For implementation details, see source files:
- [driver/patient_vitals_driver.c](driver/patient_vitals_driver.c)
- [app/patient_data_logger.c](app/patient_data_logger.c)
