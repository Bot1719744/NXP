################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_assert.c \
../utilities/fsl_debug_console.c \
../utilities/fsl_str.c 

C_DEPS += \
./utilities/fsl_assert.d \
./utilities/fsl_debug_console.d \
./utilities/fsl_str.d 

OBJS += \
./utilities/fsl_assert.o \
./utilities/fsl_debug_console.o \
./utilities/fsl_str.o 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c utilities/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MIMXRT1062DVL6A -DCPU_MIMXRT1062DVL6A_cm7 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSDK_OS_FREE_RTOS -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\12291\Desktop\NXP\workspace\new_project\board" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\source" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\freertos\freertos-kernel\include" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\freertos\freertos-kernel\portable\GCC\ARM_CM4F" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\drivers" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\xip" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\drivers\freertos" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\CMSIS" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\device" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\component\serial_manager" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\component\uart" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\utilities" -I"C:\Users\12291\Desktop\NXP\workspace\new_project\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-utilities

clean-utilities:
	-$(RM) ./utilities/fsl_assert.d ./utilities/fsl_assert.o ./utilities/fsl_debug_console.d ./utilities/fsl_debug_console.o ./utilities/fsl_str.d ./utilities/fsl_str.o

.PHONY: clean-utilities

