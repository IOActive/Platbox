#!/bin/bash

rmmod kernetix_km
make clean
make
insmod kernetix_km.ko
chmod 777 /dev/KernetixDriver0

echo "All done..."
