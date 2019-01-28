#!/bin/bash
if [[ -z "$1" ]]; then
    echo "No password given" >&2
    echo "Usage: ${0##*/} DNS_NAME PASSWORD" >&2
    exit 1
fi

# escape special characters if present
PASSWORD=${1//\\/\\\\}
PASSWORD=${PASSWORD//\"/\\\"}
PASSWORD=${PASSWORD//\'/\\\'}

HASH=$(echo $PASSWORD | md5sum | awk '{print $1}')


echo "-DOTA_PASSWORD=\\\"$PASSWORD\\\""
echo "-DOTA_PASSWORD_HASH=\\\"$HASH\\\""