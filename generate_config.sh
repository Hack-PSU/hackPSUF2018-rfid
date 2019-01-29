#!/bin/bash

SSID=$(iwgetid -r | sed "s/\n//g")
if [[ ! -z $SSID ]]; then
	PASSWD=$(sudo grep '^psk=' /etc/NetworkManager/system-connections/$(iwgetid -r) | sed "s/psk=//")
fi
REDIS=$(hostname -I | awk '{print $1}' | sed "s/\n//g")

if [[ -z "$PASSWD" ]]; then
	if [[ -f hackpsu.config ]]; then
		echo "No password set, using config file..." >&2
		cat hackpsu.config | sed '/;/d'
	else
		echo "Invalid password, please manually define password" >&2
		exit -1
	fi
else 
	echo "-DNETWORK_SSID=\\\"$SSID\\\""
	echo "-DNETWORK_PASSWORD=\\\"$PASSWD\\\""
	echo "-DREDIS=\\\"$REDIS:3000\\\""
fi

