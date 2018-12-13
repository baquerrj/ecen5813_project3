################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS_driver/fsl_i2c_cmsis.c \
../CMSIS_driver/fsl_lpsci_cmsis.c \
../CMSIS_driver/fsl_spi_cmsis.c \
../CMSIS_driver/fsl_uart_cmsis.c 

OBJS += \
./CMSIS_driver/fsl_i2c_cmsis.o \
./CMSIS_driver/fsl_lpsci_cmsis.o \
./CMSIS_driver/fsl_spi_cmsis.o \
./CMSIS_driver/fsl_uart_cmsis.o 

C_DEPS += \
./CMSIS_driver/fsl_i2c_cmsis.d \
./CMSIS_driver/fsl_lpsci_cmsis.d \
./CMSIS_driver/fsl_spi_cmsis.d \
./CMSIS_driver/fsl_uart_cmsis.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS_driver/%.o: ../CMSIS_driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../CMSIS -I../utilities -I../CMSIS_driver -I../startup -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


