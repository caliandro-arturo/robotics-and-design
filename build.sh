#!/bin/bash
set -e
arduino-cli compile --fqbn arduino:avr:mega "$1"
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:mega "$1"

