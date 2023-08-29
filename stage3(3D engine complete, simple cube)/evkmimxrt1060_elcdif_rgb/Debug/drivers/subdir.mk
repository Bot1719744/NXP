################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_elcdif.c \
../drivers/fsl_gpio.c \
../drivers/fsl_lpuart.c 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_elcdif.d \
./drivers/fsl_gpio.d \
./drivers/fsl_lpuart.d 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_elcdif.o \
./drivers/fsl_gpio.o \
./drivers/fsl_lpuart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DXIP_BOOT_HEADER_DCD_ENABLE=1 -DSKIP_SYSCLK_INIT -DDATA_SECTION_IS_CACHEABLE=1 -DCPU_MIMXRT1062DVL6A -DSDK_DEBUGCONSOLE=1 -DPRINTF_FLOAT_ENABLE=1 -DPRINTF_ADVANCED_ENABLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DAPP_LCDIF_DATA_BUS=kELCDIF_DataBus16Bit -DMCUXPRESSO_SDK -DCPU_MIMXRT1062DVL6A_cm7 -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\source" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\utilities" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\drivers" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\device" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\component\uart" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\component\lists" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\xip" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\CMSIS" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\board" -I"C:\Users\12291\Desktop\NXP\workspace\evkmimxrt1060_elcdif_rgb\evkmimxrt1060\driver_examples\elcdif\rgb" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_elcdif.d ./drivers/fsl_elcdif.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_lpuart.d ./drivers/fsl_lpuart.o

.PHONY: clean-drivers

