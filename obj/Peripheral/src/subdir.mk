################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SRC/Peripheral/src/ch32v00x_adc.c \
../SRC/Peripheral/src/ch32v00x_dbgmcu.c \
../SRC/Peripheral/src/ch32v00x_dma.c \
../SRC/Peripheral/src/ch32v00x_exti.c \
../SRC/Peripheral/src/ch32v00x_flash.c \
../SRC/Peripheral/src/ch32v00x_gpio.c \
../SRC/Peripheral/src/ch32v00x_i2c.c \
../SRC/Peripheral/src/ch32v00x_iwdg.c \
../SRC/Peripheral/src/ch32v00x_misc.c \
../SRC/Peripheral/src/ch32v00x_opa.c \
../SRC/Peripheral/src/ch32v00x_pwr.c \
../SRC/Peripheral/src/ch32v00x_rcc.c \
../SRC/Peripheral/src/ch32v00x_spi.c \
../SRC/Peripheral/src/ch32v00x_tim.c \
../SRC/Peripheral/src/ch32v00x_usart.c \
../SRC/Peripheral/src/ch32v00x_wwdg.c 

C_DEPS += \
./Peripheral/src/ch32v00x_adc.d \
./Peripheral/src/ch32v00x_dbgmcu.d \
./Peripheral/src/ch32v00x_dma.d \
./Peripheral/src/ch32v00x_exti.d \
./Peripheral/src/ch32v00x_flash.d \
./Peripheral/src/ch32v00x_gpio.d \
./Peripheral/src/ch32v00x_i2c.d \
./Peripheral/src/ch32v00x_iwdg.d \
./Peripheral/src/ch32v00x_misc.d \
./Peripheral/src/ch32v00x_opa.d \
./Peripheral/src/ch32v00x_pwr.d \
./Peripheral/src/ch32v00x_rcc.d \
./Peripheral/src/ch32v00x_spi.d \
./Peripheral/src/ch32v00x_tim.d \
./Peripheral/src/ch32v00x_usart.d \
./Peripheral/src/ch32v00x_wwdg.d 

OBJS += \
./Peripheral/src/ch32v00x_adc.o \
./Peripheral/src/ch32v00x_dbgmcu.o \
./Peripheral/src/ch32v00x_dma.o \
./Peripheral/src/ch32v00x_exti.o \
./Peripheral/src/ch32v00x_flash.o \
./Peripheral/src/ch32v00x_gpio.o \
./Peripheral/src/ch32v00x_i2c.o \
./Peripheral/src/ch32v00x_iwdg.o \
./Peripheral/src/ch32v00x_misc.o \
./Peripheral/src/ch32v00x_opa.o \
./Peripheral/src/ch32v00x_pwr.o \
./Peripheral/src/ch32v00x_rcc.o \
./Peripheral/src/ch32v00x_spi.o \
./Peripheral/src/ch32v00x_tim.o \
./Peripheral/src/ch32v00x_usart.o \
./Peripheral/src/ch32v00x_wwdg.o 

DIR_OBJS += \
./Peripheral/src/*.o \

DIR_DEPS += \
./Peripheral/src/*.d \

DIR_EXPANDS += \
./Peripheral/src/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
Peripheral/src/%.o: ../SRC/Peripheral/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"f:/CH32V003_MotorControl/SRC/Debug" -I"f:/CH32V003_MotorControl/SRC/Core" -I"f:/CH32V003_MotorControl/User" -I"f:/CH32V003_MotorControl/SRC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

