################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := LPC845
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: LPC845.axf

# Tool invocations
LPC845.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"F:\teo\Programacion\c\proyecto_final\Segundo_Parcial_Projecto\Clase Info 7_5\R1022\biblioteca INFO I\lib" -Xlinker -Map="LPC845.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "LPC845_Debug.ld" -o "LPC845.axf" $(OBJS) $(USER_OBJS) $(LIBS) -lLibInfoI_2025
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) LPC845.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC845.axf"; # arm-none-eabi-objcopy -v -O binary "LPC845.axf" "LPC845.bin" ; # checksum -p LPC845 -d "LPC845.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
