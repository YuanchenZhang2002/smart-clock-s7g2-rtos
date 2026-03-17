################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/synergy_gen/clock_alarm.c \
../src/synergy_gen/common_data.c \
../src/synergy_gen/hal_data.c \
../src/synergy_gen/main.c \
../src/synergy_gen/main_thread.c \
../src/synergy_gen/pin_data.c \
../src/synergy_gen/sensor.c 

C_DEPS += \
./src/synergy_gen/clock_alarm.d \
./src/synergy_gen/common_data.d \
./src/synergy_gen/hal_data.d \
./src/synergy_gen/main.d \
./src/synergy_gen/main_thread.d \
./src/synergy_gen/pin_data.d \
./src/synergy_gen/sensor.d 

OBJS += \
./src/synergy_gen/clock_alarm.o \
./src/synergy_gen/common_data.o \
./src/synergy_gen/hal_data.o \
./src/synergy_gen/main.o \
./src/synergy_gen/main_thread.o \
./src/synergy_gen/pin_data.o \
./src/synergy_gen/sensor.o 

SREC += \
S7G2_GUIX.srec 

MAP += \
S7G2_GUIX.map 


# Each subdirectory must supply rules for building sources it contributes
src/synergy_gen/%.o: ../src/synergy_gen/%.c
	$(file > $@.in,-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_SYNERGY_ -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy_cfg/ssp_cfg/bsp" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy_cfg/ssp_cfg/driver" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/bsp" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/bsp/cmsis/Include" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/driver/api" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/driver/instances" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/src" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/src/synergy_gen" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy_cfg/ssp_cfg/framework" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/framework/api" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/framework/instances" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy_cfg/ssp_cfg/framework/el" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/framework/el" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/src/framework/el/tx" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp_supplemental/inc/framework/instances" -I"C:/Users/a2527/e2_studio/workspace/S7G2_GUIX/synergy/ssp/inc/framework/tes" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

