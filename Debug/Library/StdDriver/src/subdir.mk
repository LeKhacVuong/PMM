################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/StdDriver/src/bpwm.c \
../Library/StdDriver/src/canfd.c \
../Library/StdDriver/src/clk.c \
../Library/StdDriver/src/crc.c \
../Library/StdDriver/src/eadc.c \
../Library/StdDriver/src/fmc.c \
../Library/StdDriver/src/gpio.c \
../Library/StdDriver/src/i2c.c \
../Library/StdDriver/src/pdma.c \
../Library/StdDriver/src/retarget.c \
../Library/StdDriver/src/rtc.c \
../Library/StdDriver/src/spi.c \
../Library/StdDriver/src/sys.c \
../Library/StdDriver/src/timer.c \
../Library/StdDriver/src/uart.c \
../Library/StdDriver/src/usbd.c \
../Library/StdDriver/src/usci_i2c.c \
../Library/StdDriver/src/usci_spi.c \
../Library/StdDriver/src/usci_uart.c \
../Library/StdDriver/src/wdt.c \
../Library/StdDriver/src/wwdt.c 

OBJS += \
./Library/StdDriver/src/bpwm.o \
./Library/StdDriver/src/canfd.o \
./Library/StdDriver/src/clk.o \
./Library/StdDriver/src/crc.o \
./Library/StdDriver/src/eadc.o \
./Library/StdDriver/src/fmc.o \
./Library/StdDriver/src/gpio.o \
./Library/StdDriver/src/i2c.o \
./Library/StdDriver/src/pdma.o \
./Library/StdDriver/src/retarget.o \
./Library/StdDriver/src/rtc.o \
./Library/StdDriver/src/spi.o \
./Library/StdDriver/src/sys.o \
./Library/StdDriver/src/timer.o \
./Library/StdDriver/src/uart.o \
./Library/StdDriver/src/usbd.o \
./Library/StdDriver/src/usci_i2c.o \
./Library/StdDriver/src/usci_spi.o \
./Library/StdDriver/src/usci_uart.o \
./Library/StdDriver/src/wdt.o \
./Library/StdDriver/src/wwdt.o 

C_DEPS += \
./Library/StdDriver/src/bpwm.d \
./Library/StdDriver/src/canfd.d \
./Library/StdDriver/src/clk.d \
./Library/StdDriver/src/crc.d \
./Library/StdDriver/src/eadc.d \
./Library/StdDriver/src/fmc.d \
./Library/StdDriver/src/gpio.d \
./Library/StdDriver/src/i2c.d \
./Library/StdDriver/src/pdma.d \
./Library/StdDriver/src/retarget.d \
./Library/StdDriver/src/rtc.d \
./Library/StdDriver/src/spi.d \
./Library/StdDriver/src/sys.d \
./Library/StdDriver/src/timer.d \
./Library/StdDriver/src/uart.d \
./Library/StdDriver/src/usbd.d \
./Library/StdDriver/src/usci_i2c.d \
./Library/StdDriver/src/usci_spi.d \
./Library/StdDriver/src/usci_uart.d \
./Library/StdDriver/src/wdt.d \
./Library/StdDriver/src/wwdt.d 


# Each subdirectory must supply rules for building sources it contributes
Library/StdDriver/src/%.o: ../Library/StdDriver/src/%.c Library/StdDriver/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"../Library/CMSIS/Include" -I"../Library/Device/Nuvoton/M253/Include" -I"../Library/StdDriver/inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


