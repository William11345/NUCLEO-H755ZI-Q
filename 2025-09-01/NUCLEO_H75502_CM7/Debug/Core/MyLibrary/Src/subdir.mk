################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/MyLibrary/Src/utility.c 

C_DEPS += \
./Core/MyLibrary/Src/utility.d 

OBJS += \
./Core/MyLibrary/Src/utility.o 


# Each subdirectory must supply rules for building sources it contributes
Core/MyLibrary/Src/%.o Core/MyLibrary/Src/%.su Core/MyLibrary/Src/%.cyclo: ../Core/MyLibrary/Src/%.c Core/MyLibrary/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"C:/Users/WILLIAM/STM32CubeIDE/workspace_1.18.0/NUCLEO_H75502/CM7/Core/MyLibrary/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-MyLibrary-2f-Src

clean-Core-2f-MyLibrary-2f-Src:
	-$(RM) ./Core/MyLibrary/Src/utility.cyclo ./Core/MyLibrary/Src/utility.d ./Core/MyLibrary/Src/utility.o ./Core/MyLibrary/Src/utility.su

.PHONY: clean-Core-2f-MyLibrary-2f-Src

