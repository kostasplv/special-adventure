################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bloom_functions.c \
../dy_ngrams.c \
../main.c \
../main1.c \
../st_ngrams.c \
../unit_testing.c 

O_SRCS += \
../bloom_functions.o \
../dy_ngrams.o \
../main.o \
../st_ngrams.o \
../unit_testing.o 

OBJS += \
./bloom_functions.o \
./dy_ngrams.o \
./main.o \
./main1.o \
./st_ngrams.o \
./unit_testing.o 

C_DEPS += \
./bloom_functions.d \
./dy_ngrams.d \
./main.d \
./main1.d \
./st_ngrams.d \
./unit_testing.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


