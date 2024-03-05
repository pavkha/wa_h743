add_library(st7735 INTERFACE)

target_sources(st7735 INTERFACE
    st7735.c
    st7735_reg.c
    lcd.c
)

target_include_directories(st7735 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(st7735 INTERFACE
    Libopencm3::stm32h7
)
