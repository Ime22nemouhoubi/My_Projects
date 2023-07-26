#!/bin/sh
arm-none-eabi-objcopy -O binary pfeMouhoubi_exe pfeMouhoubi.bin
JLinkExe -device ATSAMD21J18 -if SWD -speed 4000 -autoconnect 1 -CommandFile JLink.script
