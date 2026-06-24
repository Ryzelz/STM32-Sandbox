################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/UserDrivers/mpu6050.c 

OBJS += \
./Core/UserDrivers/mpu6050.o 

C_DEPS += \
./Core/UserDrivers/mpu6050.d 


# Each subdirectory must supply rules for building sources it contributes
Core/UserDrivers/%.o Core/UserDrivers/%.su Core/UserDrivers/%.cyclo: ../Core/UserDrivers/%.c Core/UserDrivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Peripherals -I../Core/UserDrivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-UserDrivers

clean-Core-2f-UserDrivers:
	-$(RM) ./Core/UserDrivers/mpu6050.cyclo ./Core/UserDrivers/mpu6050.d ./Core/UserDrivers/mpu6050.o ./Core/UserDrivers/mpu6050.su

.PHONY: clean-Core-2f-UserDrivers

