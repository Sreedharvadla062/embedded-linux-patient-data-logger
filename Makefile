.PHONY: all driver app clean help

all: driver app
	@echo "Build complete!"

driver:
	@echo "Building kernel module..."
	@cd driver && make

app:
	@echo "Building user-space application..."
	@cd app && make

clean:
	@echo "Cleaning build artifacts..."
	@cd driver && make clean
	@cd app && make clean

help:
	@echo "Embedded Linux Patient Data Logger - Build System"
	@echo "=================================================="
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build both kernel module and user-space app (default)"
	@echo "  driver    - Build kernel module only"
	@echo "  app       - Build user-space application only"
	@echo "  clean     - Remove all build artifacts"
	@echo "  help      - Display this help message"
	@echo ""
	@echo "After building:"
	@echo "  1. Load the kernel module: sudo insmod driver/patient_vitals_driver.ko"
	@echo "  2. Run the application: ./app/patient_data_logger [interval] [logfile] [max_reads]"
	@echo "  3. Unload the module: sudo rmmod patient_vitals_driver"
