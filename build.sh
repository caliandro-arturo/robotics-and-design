#!/bin/bash
set -e
# include libraries
libraries=$(find libs -path libs -o -prune | paste -sd, -)
platform="mega"
# compile and upload
arduino-cli compile \
	--build-cache-path "build" \
	--fqbn arduino:avr:"$platform" \
	--libraries "$libraries" \
	--output-dir "build" \
	robotics-and-design.ino
if [[ $(arduino-cli board list) = "No boards found." ]]; then
	echo "No boards found, skip upload"
	exit
fi
arduino-cli upload robotics-and-design.ino 
