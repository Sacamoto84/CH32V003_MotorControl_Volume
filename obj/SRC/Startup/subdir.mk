################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../SRC/Startup/startup_ch32v00x.S 

S_UPPER_DEPS += \
./SRC/Startup/startup_ch32v00x.d 

OBJS += \
./SRC/Startup/startup_ch32v00x.o 

DIR_OBJS += \
./SRC/Startup/*.o \

DIR_DEPS += \
./SRC/Startup/*.d \

DIR_EXPANDS += \
./SRC/Startup/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
SRC/Startup/%.o: ../SRC/Startup/%.S
	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -x assembler-with-cpp -I"f:/CH32V003_MotorControl/SRC/Startup" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

