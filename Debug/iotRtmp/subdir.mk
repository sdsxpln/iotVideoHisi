################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../iotRtmp/iotRtmp.c 

OBJS += \
./iotRtmp/iotRtmp.o 

C_DEPS += \
./iotRtmp/iotRtmp.d 


# Each subdirectory must supply rules for building sources it contributes
iotRtmp/%.o: ../iotRtmp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-hisiv300-linux-uclibcgnueabi-gcc -Dhi3518e -DNO_CRYPTO -Dhi3518ev200 -DISP_V2 -DHICHIP=0x3518E200 -DHI_XXXX -DHI_ACODEC_TYPE_INNER -DHI_RELEASE -DCHIP_ID=CHIP_HI3518E_V200 -DSENSOR_TYPE=APTINA_AR0130_DC_720P_30FPS -I"/home/lichen/eclipse-workspace/iotVideoHisi/iotRtmp" -I"/home/lichen/eclipse-workspace/iotVideoHisi/iotMqtt" -I"/home/lichen/eclipse-workspace/iotVideoHisi/iotMqtt/libmqtt" -I"/home/lichen/eclipse-workspace/iotVideoHisi/iotMqtt/client" -I"/home/lichen/eclipse-workspace/iotVideoHisi/hi3518/include" -I"/home/lichen/eclipse-workspace/iotVideoHisi/hi3518/common" -I"/home/lichen/eclipse-workspace/iotVideoHisi/src" -I"/home/lichen/eclipse-workspace/iotVideoHisi/misc" -I"/home/lichen/eclipse-workspace/iotVideoHisi/cJSON" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


