# Tentaculo - Python & C++ Integration

Python on Raspberry Pi with C++ code running on an ESP32. Python handles triggering the Arduino and RNBO patch generating synth sounds running on the Pi and starts up on boot. 

- **Python**: Runs on the Pi, controls automation, talks to the ESP32.
- **C++**: Handles LED animations on the ESP32.
- **RNBO**: Runs on the Pi, generates synth sounds.

## Overview
- Python starts automatically using systemd.
- ESP32 handles the hardware, receiving commands from the Pi.
- RNBO on the Pi manages sound generation.

Check [this Gist](https://gist.github.com/rumihumphrey/40d0b301491706b9d7bba81f53a6031f) for details of startup service.

