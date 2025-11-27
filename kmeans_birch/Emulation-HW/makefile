#
# this file was created by a computer. trust it.
#

# compiler tools
XILINX_VITIS ?= /tools/Xilinx/Vitis/2023.2

HOST_CXX ?= aarch64-linux-gnu-g++
EMCONFIGUTIL = ${XILINX_VITIS}/bin/emconfigutil --od .
RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM = xilinx_zcu104_base_202320_1
VITIS_PLATFORM_DIR = $(XILINX_VITIS)/base_platforms/xilinx_zcu104_base_202320_1
VITIS_PLATFORM_PATH = $(XILINX_VITIS)/base_platforms/xilinx_zcu104_base_202320_1/xilinx_zcu104_base_202320_1.xpfm

# host compiler global settings
CXXFLAGS += -std=c++1y -DVITIS_PLATFORM=$(VITIS_PLATFORM) -D__USE_XOPEN2K8 -I/tools/Xilinx/Vitis_HLS/2023.2/include/ -I/home/ramu/workspace/xilinx-zynqmp-common-v2023.2_10140544/xilinx-zynqmp-common-v2023.2/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/xrt/ -I/home/ramu/workspace/xilinx-zynqmp-common-v2023.2_10140544/xilinx-zynqmp-common-v2023.2/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include -O0 -g -Wall -c -fmessage-length=0 --sysroot=/home/ramu/workspace/xilinx-zynqmp-common-v2023.2_10140544/xilinx-zynqmp-common-v2023.2/sysroots/cortexa72-cortexa53-xilinx-linux
LDFLAGS += -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ -L/home/ramu/workspace/xilinx-zynqmp-common-v2023.2_10140544/xilinx-zynqmp-common-v2023.2/sysroots/cortexa72-cortexa53-xilinx-linux/usr/lib/ --sysroot=/home/ramu/workspace/xilinx-zynqmp-common-v2023.2_10140544/xilinx-zynqmp-common-v2023.2/sysroots/cortexa72-cortexa53-xilinx-linux

#
# host files
#

HOST_OBJECTS += src/kmeans_birch_host.o

HOST_EXE = kmeans_birch

BUILD_SUBDIRS += src/

EMCONFIG_FILE = emconfig.json

#
# primary build targets
#

.PHONY: all clean
all:  $(HOST_EXE) $(EMCONFIG_FILE)

clean:
	-$(RM) $(HOST_EXE) $(HOST_OBJECTS)

.PHONY: incremental
incremental: all

include makeemconfig.mk

nothing:

#
# host rules
#

src/kmeans_birch_host.o: ../src/kmeans_birch_host.cpp ../src/kmeans_birch_header.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

$(HOST_EXE): $(HOST_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

#
# emulation configuration file
#

$(EMCONFIG_FILE): makeemconfig.mk
	$(EMCONFIGUTIL) --nd $(NUMBER_OF_DEVICES)  --platform $(VITIS_PLATFORM_PATH)
	-@$(RMDIR) TempConfig .Xil/configutil*

