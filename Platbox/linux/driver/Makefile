DESTDIR	=
MODDIR	= $(DESTDIR)/lib/modules
KVERS	= $(shell uname -r)
KVER	= $(KVERS)
VMODDIR = $(MODDIR)/$(KVER)
KSRC	?= $(VMODDIR)/build

obj-m := kernetix_km.o
kernetix_km-objs += kernetix.o low_level_functions.o

EXTRA_CFLAGS := -I$(src)/../include 

quiet_cmd_nasm64 = NASM_64 $@
      cmd_nasm64 = nasm -f elf64 -o $@ $<

all: kernetix

check_kernel_dir:
	@if [ ! -d $(KSRC) ]; then \
	    echo "Unable to find the Linux source tree."; \
	    exit 1; \
	fi

kernetix:  check_kernel_dir clean	
	make -C $(KSRC) M=$(PWD) modules

clean:	
	make -C $(KSRC) M=$(PWD) clean


$(obj)/low_level_functions.o: $(src)/low_level_functions.asm
	$(call if_changed,nasm64)


