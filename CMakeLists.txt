cmake_minimum_required(VERSION 3.15)
project(Project_17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "-g -Os")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(AVR_DIR "/home/maivuphong/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7")
set(GCC_LIB_DIR "/home/maivuphong/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/lib/gcc/avr/7.3.0")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER ${AVR_DIR}/bin/avr-gcc)
set(CMAKE_CXX_COMPILER ${AVR_DIR}/bin/avr-g++)
set(CMAKE_ASM_COMPILER ${AVR_DIR}/bin/avr-gcc)
set(OBJCOPY ${AVR_DIR}/bin/avr-objcopy)
set(OBJDUMP ${AVR_DIR}/bin/avr-objdump)

set(MCU atmega328p)
set(PROG_TYPE arduino)
set(PORT /dev/ttyUSB0)
set(F_CPU 16000000L)
set(BAUD_RATE 115200)

set(AVRDUDE "/home/maivuphong/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude")
set(AVRDUDE_CONF "/home/maivuphong/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf")

add_executable(elf main.c)
set_target_properties(elf PROPERTIES
  OUTPUT_NAME ${PROJECT_NAME}.elf
)
target_include_directories(elf PRIVATE
  ${AVR_DIR}/avr/include
  include
)
target_link_directories(elf PRIVATE
  ${AVR_DIR}/lib
  ${AVR_DIR}/avr/lib
  ${GCC_LIB_DIR}
)
target_link_libraries(elf PRIVATE
  c
  m
  gcc
  gcov
)
target_compile_options(elf PRIVATE
  -mmcu=${MCU}
  -Wall
  -Wextra
  -Werror
  -ffunction-sections
  -fdata-sections
  -flto
  -fno-fat-lto-objects
  -MMD
  -save-temps
  -fverbose-asm
)
target_compile_definitions(elf PRIVATE
  -DF_CPU=${F_CPU}
)
target_link_options(elf PRIVATE
  -mmcu=${MCU}
  -Wall
  -Wextra
  -Werror
  -flto
  -fuse-linker-plugin
  -Wl,--gc-sections
  # -Wl,--verbose
)
add_custom_command(TARGET elf POST_BUILD
  COMMAND ${OBJDUMP} -h -S ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lst
  BYPRODUCTS ${PROJECT_NAME}.lst
)
set_target_properties(elf PROPERTIES
  ADDITIONAL_CLEAN_FILES "*.s;*.i"
)

add_custom_target(hex ALL
  DEPENDS elf
  COMMAND ${OBJCOPY} -R .eeprom -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
)

add_custom_target(eeprom ALL
  DEPENDS elf
  COMMAND ${OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.eeprom
)

add_custom_target(flash
  DEPENDS hex
  COMMAND ${AVRDUDE} -v -V -D -C${AVRDUDE_CONF} -p${MCU} -c${PROG_TYPE} -P${PORT} -b${BAUD_RATE} -Uflash:w:${PROJECT_NAME}.hex:i
)
