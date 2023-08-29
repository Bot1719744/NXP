################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/dcd.c \
../board/peripherals.c \
../board/pin_mux.c 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/dcd.d \
./board/peripherals.d \
./board/pin_mux.d 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/dcd.o \
./board/peripherals.o \
./board/pin_mux.o 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MIMXRT1062DVL6A -DCPU_MIMXRT1062DVL6A_cm7 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSDK_OS_FREE_RTOS -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\12291\Desktop\NXP\workspace\new_project\board" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\source" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\freertos\freertos-kernel\include" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\freertos\freertos-kernel\portable\GCC\ARM_CM4F" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\drivers" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\xip" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\drivers\freertos" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\CMSIS" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\device" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\component\serial_manager" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\component\uart" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\utilities" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-board

clean-board:
	-$(RM) ./board/board.d ./board/board.o ./board/clock_config.d ./board/clock_config.o ./board/dcd.d ./board/dcd.o ./board/peripherals.d ./board/peripherals.o ./board/pin_mux.d ./board/pin_mux.o

.PHONY: clean-board

