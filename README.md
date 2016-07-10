# BBB R2R DAC

uses the PRU to generate a waveform at 2mhz.

## connections

first 8 bits of the PRU gpio bus:

* p0 p9_31
* p1 p9_29
* p2 p9_30
* p3 p9_28
* p4 p9_42
* p5 p9_27
* p6 p9_41
* p7 p9_25

## setup

### Disable HDMI to enable loading of PRU overlay

Edit /boot/uEnv, change line 47 to disable HDMI:

    ##Disable HDMI (v3.8.x)
    cape_disable=capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN

### Build, copy the overlay and add it to slots

    cd overlay
    ./build
    cp EBB-PRU-Example-00A0.dtbo /lib/firmware/
    echo EBB-PRU-Example > /sys/devices/bone_capemgr.9/slots 

There should be no error after the last command.

### Build and test PRU code

    cd prussC
    ./build

### Generate a datafile

Python code generates a 4000 sample saw tooth

    ./generate_saw.py

### Load the datafile

    ./load_data

Now there should be a 3.3v peak to peak ~640Hz sawtooth waveform on the end of the R2R DAC.
