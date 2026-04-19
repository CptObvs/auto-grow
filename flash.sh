#!/bin/bash
# ESPHome Flash Script
# Usage: ./flash.sh [COM_PORT]

# Default COM port
COM_PORT=${1:-COM4}

# Activate venv if available
if [ -d ".venv/Scripts" ]; then
    source .venv/Scripts/activate
fi


# Kompiliere ESPHome nativ im Bash-Terminal (Best Practice)
echo "Compiling firmware (native Bash) ..."
export MSYSTEM=
export MSYS=
export MINGW_PREFIX=
export MINGW_CHOST=
cd "$(dirname "$0")"
if ! .venv/Scripts/esphome.exe compile auto-grow.yaml; then
    echo "Compilation failed. Aborting flash."
    exit 1
fi

echo "Flashing auto-grow.yaml to $COM_PORT ..."
esphome upload auto-grow.yaml --device $COM_PORT
