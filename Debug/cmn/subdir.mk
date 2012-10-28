################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cmn/CFIFO.cpp \
../cmn/CI2c.cpp \
../cmn/CTimer.cpp \
../cmn/CUART.cpp \
../cmn/crc.cpp 

OBJS += \
./cmn/CFIFO.o \
./cmn/CI2c.o \
./cmn/CTimer.o \
./cmn/CUART.o \
./cmn/crc.o 

CPP_DEPS += \
./cmn/CFIFO.d \
./cmn/CI2c.d \
./cmn/CTimer.d \
./cmn/CUART.d \
./cmn/crc.d 


# Each subdirectory must supply rules for building sources it contributes
cmn/%.o: ../cmn/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Users\Byron\workspace\sim300\inc" -I"C:\Users\Byron\workspace\sim300\cmn" -Wall -g3 -gdwarf-2 -O0 -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega1280 -DF_CPU=14745600UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


