#ifndef PATIENT_DATA_H
#define PATIENT_DATA_H

#define DEVICE_NAME "patient_vitals"
#define CLASS_NAME "patient_data"

/* Patient vital signs structure */
typedef struct {
    int heart_rate;      /* BPM */
    int systolic;        /* mmHg */
    int diastolic;       /* mmHg */
    int temperature;     /* Celsius * 100 */
    int spo2;            /* Oxygen saturation % */
} patient_vitals_t;

#endif /* PATIENT_DATA_H */
