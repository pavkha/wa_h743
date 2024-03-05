#!/bin/bash

./build.sh release

openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
    -c 'program ./build/Release/src/wa743.hex verify reset exit'
