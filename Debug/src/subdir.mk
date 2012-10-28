################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CModem.cpp \
../src/CRfid.cpp \
../src/CRtc.cpp \
../src/CServer.cpp \
../src/Controller.cpp \
../src/iopins.cpp \
../src/main.cpp 

OBJS += \
./src/CModem.o \
./src/CRfid.o \
./src/CRtc.o \
./src/CServer.o \
./src/Controller.o \
./src/iopins.o \
./src/main.o 

CPP_DEPS += \
./src/CModem.d \
./src/CRfid.d \
./src/CRtc.d \
./src/CServer.d \
./src/Controller.d \
./src/iopins.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Users\Byron\workspace\sim300\inc" -I"C:\Users\Byron\workspace\sim300\cmn" -Wall -g3 -gdwarf-2 -O0 -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega1280 -DF_CPU=14745600UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


