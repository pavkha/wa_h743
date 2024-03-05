#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>

#include <libopencm3/stm32/dbgmcu.h>

extern "C" {
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

void hard_fault_handler(void) {
    while (true) {
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    while (true) {
    }
}

void delay_nop(uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

uint32_t get_dev_idc(void) {
    return DBGMCU_IDCODE;
}
}
