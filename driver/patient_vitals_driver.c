#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/jiffies.h>
#include "../include/patient_data.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Project");
MODULE_DESCRIPTION("Patient Vitals Simulator - Kernel Character Device Driver");
MODULE_VERSION("1.0");

static dev_t device_number;
static struct cdev character_device;
static struct class *device_class;
static struct device *device_instance;

/* Simulated patient vitals data */
static patient_vitals_t patient_data = {
    .heart_rate = 72,
    .systolic = 120,
    .diastolic = 80,
    .temperature = 3700,  /* 37.00 Celsius */
    .spo2 = 98
};

/* Simulate patient vitals with slight variations */
static void update_patient_vitals(void)
{
    int variation;

    get_random_bytes(&variation, sizeof(int));
    variation = (variation % 10) - 5;  /* -5 to +5 variation */

    patient_data.heart_rate = 72 + variation;
    if (patient_data.heart_rate < 60)
        patient_data.heart_rate = 60;
    if (patient_data.heart_rate > 100)
        patient_data.heart_rate = 100;

    patient_data.systolic = 120 + (variation / 2);
    patient_data.diastolic = 80 + (variation / 2);
    patient_data.temperature = 3700 + (variation * 5);
    patient_data.spo2 = 98 + (variation / 3);
}

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Patient Vitals Driver: Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Patient Vitals Driver: Device closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer,
                           size_t count, loff_t *offset)
{
    int result;

    if (count < sizeof(patient_vitals_t))
        return -EINVAL;

    /* Update vitals with simulation */
    update_patient_vitals();

    /* Copy data to user space */
    result = copy_to_user(user_buffer, &patient_data, sizeof(patient_vitals_t));
    if (result != 0)
        return -EFAULT;

    printk(KERN_INFO "Patient Vitals: HR=%d, BP=%d/%d, Temp=%.2f°C, SpO2=%d%%\n",
           patient_data.heart_rate, patient_data.systolic, patient_data.diastolic,
           patient_data.temperature / 100.0, patient_data.spo2);

    return sizeof(patient_vitals_t);
}

static ssize_t device_write(struct file *file, const char __user *user_buffer,
                            size_t count, loff_t *offset)
{
    printk(KERN_WARNING "Patient Vitals Driver: Write operation not supported\n");
    return -EACCES;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init patient_vitals_init(void)
{
    int result;

    printk(KERN_INFO "Initializing Patient Vitals Driver...\n");

    /* Allocate device number */
    result = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);
    if (result < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return result;
    }

    /* Initialize and add character device */
    cdev_init(&character_device, &fops);
    character_device.owner = THIS_MODULE;
    result = cdev_add(&character_device, device_number, 1);
    if (result < 0) {
        printk(KERN_ERR "Failed to add character device\n");
        unregister_chrdev_region(device_number, 1);
        return result;
    }

    /* Create device class */
    device_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(device_class)) {
        printk(KERN_ERR "Failed to create device class\n");
        cdev_del(&character_device);
        unregister_chrdev_region(device_number, 1);
        return PTR_ERR(device_class);
    }

    /* Create device instance */
    device_instance = device_create(device_class, NULL, device_number, NULL, DEVICE_NAME);
    if (IS_ERR(device_instance)) {
        printk(KERN_ERR "Failed to create device instance\n");
        class_destroy(device_class);
        cdev_del(&character_device);
        unregister_chrdev_region(device_number, 1);
        return PTR_ERR(device_instance);
    }

    printk(KERN_INFO "Patient Vitals Driver initialized successfully\n");
    printk(KERN_INFO "Device /dev/%s created with major number %d\n", DEVICE_NAME, MAJOR(device_number));

    return 0;
}

static void __exit patient_vitals_exit(void)
{
    device_destroy(device_class, device_number);
    class_destroy(device_class);
    cdev_del(&character_device);
    unregister_chrdev_region(device_number, 1);
    printk(KERN_INFO "Patient Vitals Driver unloaded\n");
}

module_init(patient_vitals_init);
module_exit(patient_vitals_exit);
