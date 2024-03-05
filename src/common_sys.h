#pragma once

static inline void disable_interrupts() {
    __asm__ volatile("CPSID I\n");
}

static inline void enable_interrupts() {
    __asm__ volatile("CPSIE I\n");
}

#define REG8(addr) (*(volatile uint8_t*)(addr))
#define REG32(addr) (*(volatile uint32_t*)(addr))
