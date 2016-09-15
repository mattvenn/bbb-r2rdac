# can run `make Q=` to see commands as they run
Q := @

CFLAGS += --std=gnu99 -O2 -Wall

CC := $(Q)$(CC)
RM := $(Q)$(RM)
DTC := $(Q)dtc

.PHONY: all clean install

TARGETS := load_data dac.bin r2r-dac-00A0.dtbo

all: $(TARGETS)

clean:
	$(RM) $(TARGETS) *.o *.data *.text *.bin *.map *.obj *.out

install: r2r-dac-00A0.dtbo
	$(Q)install -v $^ /lib/firmware

%.out: dac.c
	clpru -i /usr/share/ti/cgt-pru/lib/ --silicon_version=3 dac.c -z AM3359_PRU.cmd -o $@ -m dac.map 

%.bin: %.out
	hexpru bin.cmd $^
	mv text.bin dac.bin

load_data: load_data.o
	$(CC) -o $@ $^ -l prussdrv

%.dtbo: %.dts
	$(DTC) -I dts -b0 -O dtb -@ -o $@ $^

