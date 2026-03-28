################################################################################
# MRS Version: 2.4.0
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
./SRC/Peripheral/src/ch32v00x_adc.d \
./SRC/Peripheral/src/ch32v00x_dbgmcu.d \
./SRC/Peripheral/src/ch32v00x_dma.d \
./SRC/Peripheral/src/ch32v00x_exti.d \
./SRC/Peripheral/src/ch32v00x_flash.d \
./SRC/Peripheral/src/ch32v00x_gpio.d \
./SRC/Peripheral/src/ch32v00x_i2c.d \
./SRC/Peripheral/src/ch32v00x_iwdg.d \
./SRC/Peripheral/src/ch32v00x_misc.d \
./SRC/Peripheral/src/ch32v00x_opa.d \
./SRC/Peripheral/src/ch32v00x_pwr.d \
./SRC/Peripheral/src/ch32v00x_rcc.d \
./SRC/Peripheral/src/ch32v00x_spi.d \
./SRC/Peripheral/src/ch32v00x_tim.d \
./SRC/Peripheral/src/ch32v00x_usart.d \
./SRC/Peripheral/src/ch32v00x_wwdg.d 

OBJS += \
./SRC/Peripheral/src/ch32v00x_adc.o \
./SRC/Peripheral/src/ch32v00x_dbgmcu.o \
./SRC/Peripheral/src/ch32v00x_dma.o \
./SRC/Peripheral/src/ch32v00x_exti.o \
./SRC/Peripheral/src/ch32v00x_flash.o \
./SRC/Peripheral/src/ch32v00x_gpio.o \
./SRC/Peripheral/src/ch32v00x_i2c.o \
./SRC/Peripheral/src/ch32v00x_iwdg.o \
./SRC/Peripheral/src/ch32v00x_misc.o \
./SRC/Peripheral/src/ch32v00x_opa.o \
./SRC/Peripheral/src/ch32v00x_pwr.o \
./SRC/Peripheral/src/ch32v00x_rcc.o \
./SRC/Peripheral/src/ch32v00x_spi.o \
./SRC/Peripheral/src/ch32v00x_tim.o \
./SRC/Peripheral/src/ch32v00x_usart.o \
./SRC/Peripheral/src/ch32v00x_wwdg.o 

DIR_OBJS += \
./SRC/Peripheral/src/*.o \

DIR_DEPS += \
./SRC/Peripheral/src/*.d \

DIR_EXPANDS += \
./SRC/Peripheral/src/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
SRC/Peripheral/src/%.o: ../SRC/Peripheral/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

