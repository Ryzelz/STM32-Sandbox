################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Peripherals/periph_i2c.c \
../Core/Peripherals/periph_uart.c 

OBJS += \
./Core/Peripherals/periph_i2c.o \
./Core/Peripherals/periph_uart.o 

C_DEPS += \
./Core/Peripherals/periph_i2c.d \
./Core/Peripherals/periph_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Peripherals/%.o Core/Peripherals/%.su Core/Peripherals/%.cyclo: ../Core/Peripherals/%.c Core/Peripherals/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/Peripherals -I../Core/UserDrivers -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Peripherals

clean-Core-2f-Peripherals:
	-$(RM) ./Core/Peripherals/periph_i2c.cyclo ./Core/Peripherals/periph_i2c.d ./Core/Peripherals/periph_i2c.o ./Core/Peripherals/periph_i2c.su ./Core/Peripherals/periph_uart.cyclo ./Core/Peripherals/periph_uart.d ./Core/Peripherals/periph_uart.o ./Core/Peripherals/periph_uart.su

.PHONY: clean-Core-2f-Peripherals

