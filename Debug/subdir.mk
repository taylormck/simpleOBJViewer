################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../io.cpp \
../main.cpp \
../material.cpp \
../mesh.cpp \
../texture.cpp 

OBJ_SRCS += \
../original.obj 

OBJS += \
./io.o \
./main.o \
./material.o \
./mesh.o \
./texture.o 

CPP_DEPS += \
./io.d \
./main.d \
./material.d \
./mesh.d \
./texture.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -DGL_GLEXT_PROTOTYPES -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


