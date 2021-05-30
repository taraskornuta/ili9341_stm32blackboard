#!/bin/bash
pkill fbcp

rmmod ili9341_spi_fb.ko
make
insmod ili9341_spi_fb.ko
#cp ../sample_pis.bmp /dev/fb1
cd ../rpi-fbcp/build
./fbcp