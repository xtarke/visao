################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
multiplosPWM_v2_20MHz.lss \

SIZEDUMMY += \
sizedummy \


# All Target
all: multiplosPWM_v2_20MHz.elf secondary-outputs

# Tool invocations
multiplosPWM_v2_20MHz.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,multiplosPWM_v2_20MHz.map -mmcu=atmega328p -o "multiplosPWM_v2_20MHz.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

multiplosPWM_v2_20MHz.lss: multiplosPWM_v2_20MHz.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S multiplosPWM_v2_20MHz.elf  >"multiplosPWM_v2_20MHz.lss"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: multiplosPWM_v2_20MHz.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p multiplosPWM_v2_20MHz.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(ELFS)$(OBJS)$(ASM_DEPS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) multiplosPWM_v2_20MHz.elf
	-@echo ' '

secondary-outputs: $(LSS) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
