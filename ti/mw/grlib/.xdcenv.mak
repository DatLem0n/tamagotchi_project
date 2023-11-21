#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /db/vtree/library/trees/zumaprod/zumaprod-o08/exports/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/packages;
override XDCROOT = /db/rtree/library/trees/xdcprod/xdcprod-v06/product/xdccore/Linux/xdctools_3_32_00_06_core
override XDCBUILDCFG = /db/vtree/library/trees/zumaprod/zumaprod-o08/exports/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/drivers.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = ti.targets.arm.elf.M3=/db/vendors/ti/arm/5.2.5/Linux ti.targets.arm.elf.M4= ti.targets.arm.elf.M4F= ti.targets.msp430.elf.MSP430X= gnu.targets.arm.M3=/db/vendors/linaro/gcc-arm-none-eabi-4_8-2014q3 gnu.targets.arm.M4= gnu.targets.arm.M4F= iar.targets.msp430.MSP430X_small= iar.targets.arm.M3=/db/vendors/iar/arm/7.50/Linux iar.targets.arm.M4= iar.targets.arm.M4F= CC13XXWARE=/db/vtree/library/trees/zumaprod/zumaprod-o08/exports/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc13xxware_2_04_03_17272 CC26XXWARE=/db/vtree/library/trees/zumaprod/zumaprod-o08/exports/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc26xxware_2_24_03_17272 CC32XXWARE= MSP430DRIVERLIB= MSP430GRLIB= MSP430USBLIB= MSP432DRIVERLIB= MWARE= TIVAWARE= MSP430HEADERS= MSP432HEADERS= IAR_MSP430HEADERS= IAR_MSP432HEADERS= MSP430DEVLIST= MSP432DEVLIST=
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /db/vtree/library/trees/zumaprod/zumaprod-o08/exports/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/packages;/db/rtree/library/trees/xdcprod/xdcprod-v06/product/xdccore/Linux/xdctools_3_32_00_06_core/packages;../../..
HOSTOS = Linux
endif
