MAKEFILE_DIR:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
REPO_DIR := $(shell dirname ${MAKEFILE_DIR})

TARGET = kernel.elf
OBJS = main.o

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone \
            -fno-exceptions -fno-rtti -std=c++17
LDFLAGS  += --entry KernelMain -z norelro --image-base 0x100000 --static

LOCAL_CPPFLAGS 	  = -I$(REPO_DIR)/libs/x86_64-elf/include \
					-I$(REPO_DIR)/libs/x86_64-elf/include/c++/v1 \
					-z separate-code \
					-nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS


.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

kernel.elf: $(OBJS) Makefile
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS)

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) $(LOCAL_CPPFLAGS) -c $<