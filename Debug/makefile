################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include synergy/ssp_supplemental/touch_drivers/touch_panel_chip_sx8654/subdir.mk
-include synergy/ssp/src/framework/tes/dave2d/subdir.mk
-include synergy/ssp/src/framework/sf_touch_panel_v2/subdir.mk
-include synergy/ssp/src/framework/sf_tes_2d_drw/subdir.mk
-include synergy/ssp/src/framework/sf_jpeg_decode/subdir.mk
-include synergy/ssp/src/framework/sf_i2c/subdir.mk
-include synergy/ssp/src/framework/sf_external_irq/subdir.mk
-include synergy/ssp/src/framework/sf_el_gx/subdir.mk
-include synergy/ssp/src/driver/r_sci_spi/subdir.mk
-include synergy/ssp/src/driver/r_sci_i2c/subdir.mk
-include synergy/ssp/src/driver/r_rtc/subdir.mk
-include synergy/ssp/src/driver/r_riic/subdir.mk
-include synergy/ssp/src/driver/r_jpeg_decode/subdir.mk
-include synergy/ssp/src/driver/r_jpeg_common/subdir.mk
-include synergy/ssp/src/driver/r_ioport/subdir.mk
-include synergy/ssp/src/driver/r_icu/subdir.mk
-include synergy/ssp/src/driver/r_gpt/subdir.mk
-include synergy/ssp/src/driver/r_glcd/subdir.mk
-include synergy/ssp/src/driver/r_elc/subdir.mk
-include synergy/ssp/src/driver/r_dtc/subdir.mk
-include synergy/ssp/src/driver/r_cgc/subdir.mk
-include synergy/ssp/src/bsp/mcu/s7g2/subdir.mk
-include synergy/ssp/src/bsp/mcu/all/subdir.mk
-include synergy/ssp/src/bsp/cmsis/Device/RENESAS/S7G2/Source/subdir.mk
-include synergy/board/s7g2_sk/subdir.mk
-include src/synergy_gen/subdir.mk
-include src/hardware/subdir.mk
-include src/gui/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
S7G2_GUIX.srec \

SECONDARY_SIZE += \
S7G2_GUIX.siz \


# All Target
all: S7G2_GUIX.elf secondary-outputs

# Tool invocations
S7G2_GUIX.elf: $(OBJS) $(USER_OBJS) $(LINKER_SCRIPT)
	$(file > S7G2_GUIX.elf.in,-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -T "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\script/r7fs7g27h3a01cfc.ld" -Xlinker --gc-sections -L "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\synergy\\ssp\\src\\bsp\\cmsis\\DSP_Lib\\cm4_gcc" -L "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\synergy\\ssp\\src\\bsp\\cmsis\\NN_Lib\\cm4_gcc" -L "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\synergy\\ssp\\src\\driver\\r_fmi\\libs" -L "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\synergy\\ssp\\src\\bsp\\mcu\\s7g2" -L "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\synergy\\ssp\\src\\framework\\el\\tx\\cm4_gcc" -L "C:\\Users\\a2527\\e2_studio\\workspace\\S7G2_GUIX\\synergy\\ssp\\src\\framework\\el\\gx\\cm4_gcc" -Wl,-Map,"S7G2_GUIX.map" --specs=nano.specs --specs=rdimon.specs -o "S7G2_GUIX.elf" -Wl,--start-group $(OBJS) $(USER_OBJS) $(LIBS) -Wl,--end-group)
	@echo Building target: $@ && arm-none-eabi-gcc @"S7G2_GUIX.elf.in"
S7G2_GUIX.srec: S7G2_GUIX.elf
	arm-none-eabi-objcopy -O srec "S7G2_GUIX.elf"  "S7G2_GUIX.srec"
S7G2_GUIX.siz: S7G2_GUIX.elf
	arm-none-eabi-size --format=berkeley "S7G2_GUIX.elf"
# Other Targets
clean:
	-$(file > clean_file.tmp,  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SECONDARY_SIZE)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(SRC_DEPS)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(SX_DEPS)  $(ASM_UPPER_DEPS)  $(SX_UPPER_DEPS)  $(OBJS)  $(SECONDARY_FLASH)  $(ASM_DEPS)  $(SREC)  $(S_UPPER_DEPS)  $(MAP) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  S7G2_GUIX.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
