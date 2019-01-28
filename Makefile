SHELL:=/bin/bash

# This option uploads a device physically connected to the computer
serial:
	platformio run -e dev -t upload

# This option can flash a device over the (local) network
ota:
	platformio run -e dev_ota -t upload

clean:
	platformio run -t clean

install: 
	bash install.sh

# Monitor output of connected device serial port
monitor:
	platformio device monitor
	
# Update platformio libraries/frameworks	
update:
	platformio update