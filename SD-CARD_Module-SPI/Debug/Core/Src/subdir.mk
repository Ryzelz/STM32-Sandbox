################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/crc-buffer.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/main.c \
../Core/Src/sd_driver_erase.c \
../Core/Src/sd_driver_init.c \
../Core/Src/sd_driver_read.c \
../Core/Src/sd_driver_secondary.c \
../Core/Src/sd_driver_write.c \
../Core/Src/spi.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/usart.c \
../Core/Src/user_diskio.c 

OBJS += \
./Core/Src/crc-buffer.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/main.o \
./Core/Src/sd_driver_erase.o \
./Core/Src/sd_driver_init.o \
./Core/Src/sd_driver_read.o \
./Core/Src/sd_driver_secondary.o \
./Core/Src/sd_driver_write.o \
./Core/Src/spi.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/usart.o \
./Core/Src/user_diskio.o 

C_DEPS += \
./Core/Src/crc-buffer.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/main.d \
./Core/Src/sd_driver_erase.d \
./Core/Src/sd_driver_init.d \
./Core/Src/sd_driver_read.d \
./Core/Src/sd_driver_secondary.d \
./Core/Src/sd_driver_write.d \
./Core/Src/spi.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/usart.d \
./Core/Src/user_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/crc-buffer.cyclo ./Core/Src/crc-buffer.d ./Core/Src/crc-buffer.o ./Core/Src/crc-buffer.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/sd_driver_erase.cyclo ./Core/Src/sd_driver_erase.d ./Core/Src/sd_driver_erase.o ./Core/Src/sd_driver_erase.su ./Core/Src/sd_driver_init.cyclo ./Core/Src/sd_driver_init.d ./Core/Src/sd_driver_init.o ./Core/Src/sd_driver_init.su ./Core/Src/sd_driver_read.cyclo ./Core/Src/sd_driver_read.d ./Core/Src/sd_driver_read.o ./Core/Src/sd_driver_read.su ./Core/Src/sd_driver_secondary.cyclo ./Core/Src/sd_driver_secondary.d ./Core/Src/sd_driver_secondary.o ./Core/Src/sd_driver_secondary.su ./Core/Src/sd_driver_write.cyclo ./Core/Src/sd_driver_write.d ./Core/Src/sd_driver_write.o ./Core/Src/sd_driver_write.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/user_diskio.cyclo ./Core/Src/user_diskio.d ./Core/Src/user_diskio.o ./Core/Src/user_diskio.su

.PHONY: clean-Core-2f-Src

