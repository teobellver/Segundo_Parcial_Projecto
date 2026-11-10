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


BUILD_ARTIFACT_NAME := Schomwandt_Bellver_1
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Schomwandt_Bellver_1.axf

# Tool invocations
Schomwandt_Bellver_1.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\teobe\Downloads\Clase Info 7_5\R1022\biblioteca INFO I\lib" -Xlinker -Map="Schomwandt_Bellver_1.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "Schomwandt_Bellver_1_Debug.ld" -o "Schomwandt_Bellver_1.axf" $(OBJS) $(USER_OBJS) $(LIBS) -lLibInfoI_2025
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Schomwandt_Bellver_1.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Schomwandt_Bellver_1.axf"; # arm-none-eabi-objcopy -v -O binary "Schomwandt_Bellver_1.axf" "Schomwandt_Bellver_1.bin" ; # checksum -p LPC845 -d "Schomwandt_Bellver_1.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
