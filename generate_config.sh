#!/bin/bash


if [[ $# -eq 0 ]]; then
    unameOut="$(uname -s)"
    case "${unameOut}" in
        Linux*)
            SSID=$(iwgetid -r | sed "s/\n//g")
            PASSWD=$(sudo grep '^psk=' /etc/NetworkManager/system-connections/${SSID} | sed "s/psk=//")
            REDIS=$(hostname -I | awk '{print $1}' | sed "s/\n//g")
            ;;
        Darwin*)
            SSID=$(/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I | awk '/ SSID/ {print substr($0, index($0, $2))}')
            PASSWD=$(security find-generic-password -wga ${SSID})
            REDIS=$(ifconfig | grep inet[^6] | grep -v "127.0.0.1" | awk '{print $2}')
            ;;
        *)
            echo "Unknown platform." >&2
            ;;
    esac
fi



if [[ ! -z $PASSWD ]]; then

    echo "-D NETWORK_SSID=\\\"$SSID\\\""
    echo "-D NETWORK_PASSWORD=\\\"${PASSWD//\"/\\\"}\\\""
    echo "-D REDIS=\\\"$REDIS:3000\\\""
elif [[ -f hackpsu.config ]]; then
    echo "No password set, using config file..." >&2
    cat hackpsu.config | sed '/;/d'
else
    echo "Unable to generate configuration, please manually enter appropriate info in hackpsu.config" >&2

    echo "; SSID of the network to use"  > hackpsu.config
    echo "  -D NETWORK_SSID=\\\"\\\""     >> hackpsu.config
    echo "; Password for network (\, ',\" will all be escaped" >> hackpsu.config
    echo "  -D NETWORK_PASSWORD=\\\"\\\"" >> hackpsu.config
    echo "; IP address of redis"          >> hackpsu.config
    echo "  -D REDIS=\\\":3000\\\""       >> hackpsu.config

    exit -1
fi
