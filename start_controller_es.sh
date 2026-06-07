#!/bin/bash
set -x
exec > /home/igor/controller_es.log 2>&1

python3 /home/igor/gamepad.py &
CTRL_PID=$!

for i in {1..20}; do
    if [ -e /dev/input/js0 ]; then
        break
    fi
    sleep 0.5
done

env -u WAYLAND_DISPLAY emulationstation

echo "js0 detected, starting EmulationStation"
emulationstation
echo "EmulationStation exited"

kill $CTRL_PID
wait $CTRL_PID
