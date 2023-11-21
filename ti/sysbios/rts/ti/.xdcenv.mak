#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = 
override XDCROOT = /db/rtree/library/trees/xdcprod/xdcprod-v06/product/xdccore/Linux/xdctools_3_32_00_06_core
override XDCBUILDCFG = /db/vtree/library/trees/zumaprod/zumaprod-o08/exports/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/bios.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = ti.targets.C28_large="" ti.targets.C28_float="" ti.targets.C674="" ti.targets.arm.elf.Arm9="" ti.targets.arm.elf.A8Fnv="" ti.targets.arm.elf.R4F="" ti.targets.arm.elf.M3="/db/vendors/ti/arm/5.2.5/Linux" ti.targets.arm.elf.M4="" ti.targets.arm.elf.M4F="" ti.targets.elf.C64P="" ti.targets.elf.C66="" ti.targets.elf.C66_big_endian="" ti.targets.elf.C674="" ti.targets.msp430.elf.MSP430X="" ti.targets.msp430.elf.MSP430X_small="" ti.targets.arp32.elf.ARP32="" ti.targets.arp32.elf.ARP32_far="" gnu.targets.arm.M3="/db/vendors/linaro/gcc-arm-none-eabi-4_8-2014q3" gnu.targets.arm.M4="" gnu.targets.arm.M4F="" gnu.targets.arm.A8F="" gnu.targets.arm.A9F="" gnu.targets.arm.A15F="" iar.targets.msp430.MSP430X_small="" iar.targets.arm.M3="/db/vendors/iar/arm/7.50/Linux" iar.targets.arm.M4="" iar.targets.arm.M4F=""
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /db/rtree/library/trees/xdcprod/xdcprod-v06/product/xdccore/Linux/xdctools_3_32_00_06_core/packages;../../../..
HOSTOS = Linux
endif
