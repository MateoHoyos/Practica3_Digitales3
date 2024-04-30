cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(polling+interrupciones) # nombre de nuestro proyecto

pico_sdk_init()


add_executable(polling+interrupciones
        polling+interrupciones_7seg.c
        # keyboard.c
        )

target_link_libraries(polling+interrupciones pico_stdlib  pico_multicore)


pico_enable_stdio_usb(polling+interrupciones 1) 
pico_enable_stdio_uart(polling+interrupciones 0)

pico_add_extra_outputs(polling+interrupciones)
SET(GCC_EXE_LINKER_FLAGS    "-Wl,--print-memory-usage")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_EXE_LINKER_FLAGS}")
