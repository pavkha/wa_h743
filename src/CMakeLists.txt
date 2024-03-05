set(TARGET_ wa743)

link_directories(${Libopencm3_LINKER_DIRS})

add_executable(${TARGET_})

set_target_properties(${TARGET_}
    PROPERTIES
        SUFFIX ".elf"
)

target_sources(${TARGET_} PRIVATE
    syscalls.cpp
    spi_h743.cpp
    rtc_h743.cpp
    init.cpp
    system.cpp
    led_blinker.cpp
    lcd.cpp
    uart_stream_in.cpp
    uart_stream_out.cpp
    cmd_path.cpp
    cmd_shell.cpp
    rtc_sys_time.cpp
    logger.cpp
    application.cpp
    main.cpp
)

target_compile_definitions(${TARGET_} PUBLIC
    STM32H7
)

set(LDSCRIPT
    ${CMAKE_SOURCE_DIR}/src/${TARGET_}.ld
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${TARGET_} PUBLIC 
        -g3
        -O0
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${TARGET_} PUBLIC
        NDEBUG
    )
    target_compile_options(${TARGET_} PUBLIC
        -O3
    )
endif()

target_link_libraries(${TARGET_}
    --static
    -nostartfiles

    --specs=nosys.specs
    --specs=nano.specs

    -Xlinker --gc-sections
    -Wl,-Map=${TARGET_}.map
    -Wl,-cref

    -T${LDSCRIPT}

    Libopencm3::stm32h7
    freertos
    freertos_addons
    st7735
)

include(bin_generator)

generate_exes(${TARGET_} 
    EXE_TYPES bin hex
)

print_size(${TARGET_})
