################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/BTHome/BTHome.c 

OBJS += \
./APP/BTHome/BTHome.o 

C_DEPS += \
./APP/BTHome/BTHome.d 


# Each subdirectory must supply rules for building sources it contributes
APP/BTHome/%.o: ../APP/BTHome/%.c
	@	@	riscv-none-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DCLK_OSC32K=0 -I"C:\Users\bitshen\Desktop\WKnob-main\Startup" -I"C:\Users\bitshen\Desktop\WKnob-main\APP\include" -I"C:\Users\bitshen\Desktop\WKnob-main\Profile\include" -I"C:\Users\bitshen\Desktop\WKnob-main\StdPeriphDriver\inc" -I"C:\Users\bitshen\Desktop\WKnob-main\HAL\include" -I"C:\Users\bitshen\Desktop\WKnob-main\Ld" -I"C:\Users\bitshen\Desktop\WKnob-main\LIB" -I"C:\Users\bitshen\Desktop\WKnob-main\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

