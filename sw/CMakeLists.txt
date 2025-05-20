cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico-mgr_board C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# add_compile_options(-Wall -Wextra)

pico_sdk_init()

add_executable(${PROJECT_NAME}
  mgr_board.c
  MCP48CV.c
  ADS1148.c
  pwm.c
  mux.c
  )

pico_add_extra_outputs(${PROJECT_NAME})

add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

target_link_libraries(${PROJECT_NAME}
  pico_stdlib
  hardware_timer
  hardware_adc
  hardware_uart
  hardware_pwm
  hardware_spi
  FatFs_SPI
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)


