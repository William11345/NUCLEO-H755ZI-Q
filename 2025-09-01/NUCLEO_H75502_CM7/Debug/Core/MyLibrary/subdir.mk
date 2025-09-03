################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MyLibrary/utility.c 

C_DEPS += \
./Core/MyLibrary/utility.d 

OBJS += \
./Core/MyLibrary/utility.o 


# Each subdirectory must supply rules for building sources it contributes
Core/MyLibrary/%.o Core/MyLibrary/%.su Core/MyLibrary/%.cyclo: ../Core/MyLibrary/%.c Core/MyLibrary/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"/NUCLEO_H75502_CM7/Core/MyLibrary" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-MyLibrary

clean-Core-2f-MyLibrary:
	-$(RM) ./Core/MyLibrary/utility.cyclo ./Core/MyLibrary/utility.d ./Core/MyLibrary/utility.o ./Core/MyLibrary/utility.su

.PHONY: clean-Core-2f-MyLibrary

