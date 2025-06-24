################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/image.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/isr.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/main.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/motor.c 

OBJS += \
./user_c/image.o \
./user_c/isr.o \
./user_c/main.o \
./user_c/motor.o 

C_DEPS += \
./user_c/image.d \
./user_c/isr.d \
./user_c/main.d \
./user_c/motor.d 


# Each subdirectory must supply rules for building sources it contributes
user_c/image.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/image.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
user_c/isr.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/isr.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
user_c/main.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/main.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
user_c/motor.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/motor.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

