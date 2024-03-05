add_library(w25qxx INTERFACE)

target_sources(w25qxx INTERFACE
    w25qxx.c
    w25qxx_qspi.c
)

target_include_directories(w25qxx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
