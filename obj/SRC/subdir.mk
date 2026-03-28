################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SRC/ch32v00x_adc.c \
../SRC/ch32v00x_dbgmcu.c \
../SRC/ch32v00x_dma.c \
../SRC/ch32v00x_exti.c \
../SRC/ch32v00x_flash.c \
../SRC/ch32v00x_gpio.c \
../SRC/ch32v00x_i2c.c \
../SRC/ch32v00x_iwdg.c \
../SRC/ch32v00x_misc.c \
../SRC/ch32v00x_opa.c \
../SRC/ch32v00x_pwr.c \
../SRC/ch32v00x_rcc.c \
../SRC/ch32v00x_spi.c \
../SRC/ch32v00x_tim.c \
../SRC/ch32v00x_usart.c \
../SRC/ch32v00x_wwdg.c \
../SRC/core_riscv.c \
../SRC/debug.c 

C_DEPS += \
./SRC/ch32v00x_adc.d \
./SRC/ch32v00x_dbgmcu.d \
./SRC/ch32v00x_dma.d \
./SRC/ch32v00x_exti.d \
./SRC/ch32v00x_flash.d \
./SRC/ch32v00x_gpio.d \
./SRC/ch32v00x_i2c.d \
./SRC/ch32v00x_iwdg.d \
./SRC/ch32v00x_misc.d \
./SRC/ch32v00x_opa.d \
./SRC/ch32v00x_pwr.d \
./SRC/ch32v00x_rcc.d \
./SRC/ch32v00x_spi.d \
./SRC/ch32v00x_tim.d \
./SRC/ch32v00x_usart.d \
./SRC/ch32v00x_wwdg.d \
./SRC/core_riscv.d \
./SRC/debug.d 

S_UPPER_SRCS += \
../SRC/startup_ch32v00x.S 

S_UPPER_DEPS += \
./SRC/startup_ch32v00x.d 

OBJS += \
./SRC/ch32v00x_adc.o \
./SRC/ch32v00x_dbgmcu.o \
./SRC/ch32v00x_dma.o \
./SRC/ch32v00x_exti.o \
./SRC/ch32v00x_flash.o \
./SRC/ch32v00x_gpio.o \
./SRC/ch32v00x_i2c.o \
./SRC/ch32v00x_iwdg.o \
./SRC/ch32v00x_misc.o \
./SRC/ch32v00x_opa.o \
./SRC/ch32v00x_pwr.o \
./SRC/ch32v00x_rcc.o \
./SRC/ch32v00x_spi.o \
./SRC/ch32v00x_tim.o \
./SRC/ch32v00x_usart.o \
./SRC/ch32v00x_wwdg.o \
./SRC/core_riscv.o \
./SRC/debug.o \
./SRC/startup_ch32v00x.o 

DIR_OBJS += \
./SRC/*.o \

DIR_DEPS += \
./SRC/*.d \

DIR_EXPANDS += \
./SRC/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
SRC/%.o: ../SRC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/User" -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/SRC" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

SRC/%.o: ../SRC/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

