#!/bin/bash



unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
		SSID=$(iwgetid -r | sed "s/\n//g")
		PASSWD=$(sudo grep '^psk=' /etc/NetworkManager/system-connections/${SSID} | sed "s/psk=//")
		;;
    Darwin*)
		SSID=$(/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I | awk '/ SSID/ {print substr($0, index($0, $2))}')
        PASSWD=$(security find-generic-password -wga ${SSID})
		;;
    *)
	echo "Unknown platform." >&2
		;;
esac



if [[ ! -z $PASSWD ]]; then
	REDIS=$(hostname -I | awk '{print $1}' | sed "s/\n//g")

	echo "-D NETWORK_SSID=\\\"$SSID\\\""
	echo "-D NETWORK_PASSWORD=\\\"${PASSWD//\"/\\\"}\\\""
	echo "-D REDIS=\\\"$REDIS:3000\\\""
elif [[ -f hackpsu.config ]]; then
	echo "No password set, using config file..." >&2
	cat hackpsu.config | sed '/;/d'
else
	echo "Unable to generate configuration, please manually enter appropriate info in hackpsu.config" >&2

	echo "-D NETWORK_SSID=\\\"\\\""      > hackpsu.config
	echo "-D NETWORK_PASSWORD=\\\"\\\"" >> hackpsu.config
	echo "-D REDIS=\\\":3000\\\""       >> hackpsu.config

	exit -1
fi
