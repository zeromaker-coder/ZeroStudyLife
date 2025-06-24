################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_clock.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_debug.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_fifo.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_font.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_function.c \
D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_interrupt.c 

OBJS += \
./zf_common/zf_common_clock.o \
./zf_common/zf_common_debug.o \
./zf_common/zf_common_fifo.o \
./zf_common/zf_common_font.o \
./zf_common/zf_common_function.o \
./zf_common/zf_common_interrupt.o 

C_DEPS += \
./zf_common/zf_common_clock.d \
./zf_common/zf_common_debug.d \
./zf_common/zf_common_fifo.d \
./zf_common/zf_common_font.d \
./zf_common/zf_common_function.d \
./zf_common/zf_common_interrupt.d 


# Each subdirectory must supply rules for building sources it contributes
zf_common/zf_common_clock.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_clock.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_common/zf_common_debug.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_debug.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_common/zf_common_fifo.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_fifo.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_common/zf_common_font.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_font.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_common/zf_common_function.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_function.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_common/zf_common_interrupt.o: D:/smartcar/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_common/zf_common_interrupt.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\smartcar\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

