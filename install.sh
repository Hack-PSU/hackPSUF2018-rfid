#!/bin/bash

if [[ ! $(command -v platformio) ]]; then
	# Install platformio
	python -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/develop/scripts/get-platformio.py)"
fi

if [[ ! -f /etc/udev/rules.d/99-platformio-udev.rules ]]; then
	sudo bash -c 'curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/scripts/99-platformio-udev.rules > /etc/udev/rules.d/99-platformio-udev.rules'
fi

