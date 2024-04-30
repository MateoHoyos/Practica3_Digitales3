cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(Interrupciones) # nombre de nuestro proyecto

pico_sdk_init()


add_executable(Interrupciones
        main.c
        #led_functions.c
        #read_keypad.c
        #check_password.c
        # keyboard.c
        )

target_link_libraries(Interrupciones pico_stdlib  pico_multicore)


pico_enable_stdio_usb(Interrupciones 1) 
pico_enable_stdio_uart(Interrupciones 0)

pico_add_extra_outputs(Interrupciones)

target_link_libraries(Interrupciones
        pico_multicore)

SET(GCC_EXE_LINKER_FLAGS    "-Wl,--print-memory-usage")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_EXE_LINKER_FLAGS}")

