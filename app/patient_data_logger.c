#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "../include/patient_data.h"

static int device_fd = -1;
static volatile int should_exit = 0;

void signal_handler(int sig)
{
    printf("\nShutting down patient data logger...\n");
    should_exit = 1;
}

int log_to_file(const char *filename, const patient_vitals_t *vitals)
{
    FILE *file;
    time_t now;
    struct tm *timeinfo;
    char timestamp[100];

    file = fopen(filename, "a");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    fprintf(file, "[%s] HR: %d BPM | BP: %d/%d mmHg | Temp: %.2f°C | SpO2: %d%%\n",
            timestamp, vitals->heart_rate, vitals->systolic, vitals->diastolic,
            vitals->temperature / 100.0, vitals->spo2);

    fclose(file);
    return 0;
}

void display_vitals(const patient_vitals_t *vitals)
{
    printf("\n=== Patient Vital Signs ===\n");
    printf("Heart Rate:      %d BPM\n", vitals->heart_rate);
    printf("Blood Pressure:  %d/%d mmHg\n", vitals->systolic, vitals->diastolic);
    printf("Temperature:     %.2f°C\n", vitals->temperature / 100.0);
    printf("SpO2:            %d%%\n", vitals->spo2);
    printf("===========================\n\n");
}

int main(int argc, char *argv[])
{
    patient_vitals_t vitals;
    ssize_t bytes_read;
    int interval = 5;  /* Default 5 seconds */
    const char *log_file = "patient_data.log";
    int read_count = 0;
    int max_reads = 0;  /* 0 = infinite */

    /* Parse command line arguments */
    if (argc > 1) {
        interval = atoi(argv[1]);
        if (interval < 1) interval = 5;
    }
    if (argc > 2) {
        log_file = argv[2];
    }
    if (argc > 3) {
        max_reads = atoi(argv[3]);
    }

    printf("Patient Data Logger - Real-time Healthcare Monitoring\n");
    printf("=====================================================\n");
    printf("Opening device: /dev/%s\n", DEVICE_NAME);

    /* Open device */
    device_fd = open("/dev/" DEVICE_NAME, O_RDONLY);
    if (device_fd < 0) {
        perror("open");
        fprintf(stderr, "Error: Could not open /dev/%s\n", DEVICE_NAME);
        fprintf(stderr, "Make sure the kernel module is loaded: sudo insmod patient_vitals_driver.ko\n");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully\n");
    printf("Logging interval: %d seconds\n", interval);
    printf("Log file: %s\n", log_file);
    if (max_reads > 0)
        printf("Max reads: %d\n", max_reads);
    printf("Press Ctrl+C to exit\n\n");

    /* Setup signal handler */
    signal(SIGINT, signal_handler);

    /* Main loop */
    while (!should_exit) {
        /* Read patient vitals from device */
        bytes_read = read(device_fd, &vitals, sizeof(patient_vitals_t));

        if (bytes_read < 0) {
            perror("read");
            break;
        }

        if (bytes_read == sizeof(patient_vitals_t)) {
            read_count++;
            display_vitals(&vitals);
            log_to_file(log_file, &vitals);

            if (max_reads > 0 && read_count >= max_reads) {
                printf("Maximum reads (%d) reached. Exiting.\n", max_reads);
                break;
            }

            sleep(interval);
        } else {
            fprintf(stderr, "Error: Incomplete read from device\n");
            break;
        }
    }

    /* Cleanup */
    if (device_fd >= 0) {
        close(device_fd);
        printf("\nDevice closed\n");
    }

    printf("Total readings logged: %d\n", read_count);
    printf("Data saved to: %s\n", log_file);

    return EXIT_SUCCESS;
}
