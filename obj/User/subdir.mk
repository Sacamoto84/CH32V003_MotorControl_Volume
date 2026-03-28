################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/buzzer.c \
../User/ch32v00x_it.c \
../User/init.c \
../User/system_ch32v00x.c 

C_DEPS += \
./User/buzzer.d \
./User/ch32v00x_it.d \
./User/init.d \
./User/system_ch32v00x.d 

CPP_SRCS += \
../User/adc.cpp \
../User/main.cpp \
../User/motor.cpp \
../User/screens.cpp 

CPP_DEPS += \
./User/adc.d \
./User/main.d \
./User/motor.d \
./User/screens.d 

OBJS += \
./User/adc.o \
./User/buzzer.o \
./User/ch32v00x_it.o \
./User/init.o \
./User/main.o \
./User/motor.o \
./User/screens.o \
./User/system_ch32v00x.o 

DIR_OBJS += \
./User/*.o \

DIR_DEPS += \
./User/*.d \

DIR_EXPANDS += \
./User/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/User" -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/SRC" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

User/%.o: ../User/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-g++ -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/User" -I"g:/CH32V003_MotorControl-master/CH32V003_MotorControl-master/SRC" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@

