#
# this file was created by a computer. trust it.
#

# compiler tools
XILINX_VITIS ?= /tools/Xilinx/Vitis/2023.2
XILINX_VIVADO ?= /tools/Xilinx/Vivado/2023.2
XILINX_HLS ?= $(XILINX_VITIS)/Vivado_HLS

VPP ?= ${XILINX_VITIS}/bin/v++
RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM = xilinx_zcu104_base_202320_1
VITIS_PLATFORM_DIR = $(XILINX_VITIS)/base_platforms/xilinx_zcu104_base_202320_1
VITIS_PLATFORM_PATH = $(XILINX_VITIS)/base_platforms/xilinx_zcu104_base_202320_1/xilinx_zcu104_base_202320_1.xpfm

TARGET = hw_emu
VPP_OPTS = --target $(TARGET)

#
# Hw kernel files
#


BUILD_SUBDIRS += build
HW_KERNEL_OBJS += build/birch_distance.xo
SRC_KERNEL_OBJS += build/birch_distance.xo

ALL_MESSAGE_FILES = $(subst .xo,.o,.mdb,$(SRC_KERNEL_OBJS))

#
# primary build targets
#

.PHONY: all clean
all: $(HW_KERNEL_OBJS)


clean:
	-$(RM) $(SRC_KERNEL_OBJS) $(ALL_MESSAGE_FILES) 
	-$(RMDIR) $(BUILD_SUBDIRS)
	-$(RMDIR) .Xil

.PHONY: incremental
incremental: all


nothing:

#
# Hw Kernels compile;
#

build/birch_distance.xo: ../src/kmeans_birch.cpp birch_distance-compile.cfg
	-@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) $(VPP_OPTS) --compile -I"$(<D)" -I"../../kmeans_birch/src" --config birch_distance-compile.cfg -o"$@" "$<"

