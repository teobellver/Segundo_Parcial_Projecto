################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Schomwandt_Bellver_1.c \
../src/cr_startup_lpc84x.c \
../src/crp.c \
../src/funciones.c \
../src/inicializarInfotronic.c \
../src/mtb.c 

S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_DEPS += \
./src/Schomwandt_Bellver_1.d \
./src/cr_startup_lpc84x.d \
./src/crp.d \
./src/funciones.d \
./src/inicializarInfotronic.d \
./src/mtb.d 

OBJS += \
./src/Schomwandt_Bellver_1.o \
./src/aeabi_romdiv_patch.o \
./src/cr_startup_lpc84x.o \
./src/crp.o \
./src/funciones.o \
./src/inicializarInfotronic.o \
./src/mtb.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -D__LPC84X__ -D__REDLIB__ -Os -fno-common -g -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DNDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC84X__ -D__REDLIB__ -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Schomwandt_Bellver_1.d ./src/Schomwandt_Bellver_1.o ./src/aeabi_romdiv_patch.o ./src/cr_startup_lpc84x.d ./src/cr_startup_lpc84x.o ./src/crp.d ./src/crp.o ./src/funciones.d ./src/funciones.o ./src/inicializarInfotronic.d ./src/inicializarInfotronic.o ./src/mtb.d ./src/mtb.o

.PHONY: clean-src

