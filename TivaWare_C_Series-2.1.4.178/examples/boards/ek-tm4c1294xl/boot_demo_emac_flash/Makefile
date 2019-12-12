#******************************************************************************
#
# Makefile - Rules for building the boot_demo_emac_flash example.
#
# Copyright (c) 2016-2017 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=TM4C1294NCPDT

#
# The base directory for TivaWare.
#
ROOT=../../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../drivers
VPATH+=../../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=..
IPATH+=../../../..
IPATH+=../../../../third_party/lwip-1.4.1/ports/tiva-tm4c129/include
IPATH+=../../../../third_party/lwip-1.4.1/src/include
IPATH+=../../../../third_party/lwip-1.4.1/src/include/ipv4

#
# The default rule, which causes the boot_demo_emac_flash example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/boot_demo_emac_flash.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the boot_demo_emac_flash example.
#
${COMPILER}/boot_demo_emac_flash.axf: ${COMPILER}/boot_demo_emac_flash.o
${COMPILER}/boot_demo_emac_flash.axf: ${COMPILER}/lwiplib.o
${COMPILER}/boot_demo_emac_flash.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/boot_demo_emac_flash.axf: ${COMPILER}/swupdate.o
${COMPILER}/boot_demo_emac_flash.axf: ${COMPILER}/ustdlib.o
${COMPILER}/boot_demo_emac_flash.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/boot_demo_emac_flash.axf: boot_demo_emac_flash.ld
SCATTERgcc_boot_demo_emac_flash=boot_demo_emac_flash.ld
ENTRY_boot_demo_emac_flash=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C129_RA2 -DUSE_FLASH_BOOT_LOADER

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
