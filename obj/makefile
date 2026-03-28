################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include SRC/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Standby_Mode.hex \

SECONDARY_LIST += \
Standby_Mode.lst \

SECONDARY_SIZE += \
Standby_Mode.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: Standby_Mode.elf secondary-outputs

# Tool invocations
Standby_Mode.elf: $(OBJS) $(USER_OBJS_ESCAPE)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv-none-embed-g++ -march=rv32ecxw -mabi=ilp32e -mtune=size -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "g:\CH32V003_MotorControl-master\CH32V003_MotorControl-master\SRC\Link.ld" -nostartfiles -Xlinker --gc-sections -L"g:\CH32V003_MotorControl-master\CH32V003_MotorControl-master\SRC" -Wl,-Map,"Standby_Mode.map" --specs=nano.specs --specs=nosys.specs -o "Standby_Mode.elf" $(OBJS) $(LIBS) -lprintf
	@echo 'Finished building target: $@'
	@
Standby_Mode.hex: Standby_Mode.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	@	riscv-none-embed-objcopy -O ihex "Standby_Mode.elf" "Standby_Mode.hex"
	@echo 'Finished building $@'
	@
Standby_Mode.lst: Standby_Mode.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "Standby_Mode.elf" > "Standby_Mode.lst"
	@echo 'Finished building $@'
	@
Standby_Mode.siz: Standby_Mode.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv-none-embed-size --format=berkeley "Standby_Mode.elf"
	@echo 'Finished building $@'
	@

# Other Targets
clean:
	-$(RM) $(DIR_OBJS) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE) Standby_Mode.elf
	-$(RM) $(DIR_EXPANDS) $(CALLGRAPH_DOT)
	-$(RM) $(DIR_DEPS)
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets