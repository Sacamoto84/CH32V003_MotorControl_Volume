################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SRC/Debug/debug.c 

C_DEPS += \
./SRC/Debug/debug.d 

OBJS += \
./SRC/Debug/debug.o 

DIR_OBJS += \
./SRC/Debug/*.o \

DIR_DEPS += \
./SRC/Debug/*.d \

DIR_EXPANDS += \
./SRC/Debug/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
SRC/Debug/%.o: ../SRC/Debug/%.c
	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"f:/CH32V003_MotorControl/SRC/Debug" -I"f:/CH32V003_MotorControl/SRC/Core" -I"f:/CH32V003_MotorControl/User" -I"f:/CH32V003_MotorControl/SRC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

