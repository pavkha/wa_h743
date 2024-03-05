add_library(freertos INTERFACE)

set(FreeRTOSDir ${CMAKE_SOURCE_DIR}/../FreeRTOS/FreeRTOS)
set(FreeRTOSConfigDir ${CMAKE_SOURCE_DIR}/src)

target_sources(freertos INTERFACE
    ${FreeRTOSDir}/Source/croutine.c
    ${FreeRTOSDir}/Source/event_groups.c
    ${FreeRTOSDir}/Source/list.c
    ${FreeRTOSDir}/Source/queue.c
    ${FreeRTOSDir}/Source/stream_buffer.c
    ${FreeRTOSDir}/Source/tasks.c
    ${FreeRTOSDir}/Source/timers.c
    ${FreeRTOSDir}/Source/portable/MemMang/heap_4.c
    ${FreeRTOSDir}/Source/portable/GCC/ARM_CM7/r0p1/port.c
)

target_include_directories(freertos INTERFACE
    ${FreeRTOSDir}/Source/include
    ${FreeRTOSDir}/Source/portable/GCC/ARM_CM7/r0p1
)

target_include_directories(freertos INTERFACE
    ${FreeRTOSConfigDir}
)
