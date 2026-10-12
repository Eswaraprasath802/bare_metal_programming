.syntax unified
.cpu cortex-m0plus
.thumb

#include "pico.h"
#include "pico/platform/cpu_regs.h"
#include "hardware/regs/addressmap.h"
#include "pico/bootrom.h"

.global _reset_handler
.global _entry_point
.extern main
.extern runtime_init

/* Symbols provided by the Pico SDK's standard linker script. */
.extern __StackTop
.extern __etext
.extern __data_start__
.extern __data_end__
.extern __bss_start__
.extern __bss_end__
.extern __scratch_x_source__
.extern __scratch_x_start__
.extern __scratch_x_end__
.extern __scratch_y_source__
.extern __scratch_y_start__
.extern __scratch_y_end__

/* The SDK linker script uses this as the ELF entry for debugger launches. */
.section .text._entry_point, "ax", %progbits
.thumb_func
_entry_point:
    /* Re-enter through the Boot ROM so flash is initialized on debug launch. */
    movs r0, #BOOTROM_VTABLE_OFFSET
    ldr r1, =(PPB_BASE + ARM_CPU_PREFIXED(VTOR_OFFSET))
    str r0, [r1]
    ldmia r0!, {r1, r2}
    msr msp, r1
    bx r2

.section .text._reset_handler, "ax", %progbits
.thumb_func

_reset_handler:
    /* Set the stack explicitly; the SDK vector table uses the same top. */
    ldr r0, =__StackTop
    mov sp, r0

    /* Copy initialized data from flash into its RAM sections. */
    ldr r0, =__etext
    ldr r1, =__data_start__
    ldr r2, =__data_end__
    bl copy_region

    /* The SDK may place initialized data in either scratch bank too. */
    ldr r0, =__scratch_x_source__
    ldr r1, =__scratch_x_start__
    ldr r2, =__scratch_x_end__
    bl copy_region

    ldr r0, =__scratch_y_source__
    ldr r1, =__scratch_y_start__
    ldr r2, =__scratch_y_end__
    bl copy_region

    /* Clear .bss. */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
    movs r3, #0
clear_bss:
    cmp r1, r2
    bcs initialize_runtime
    str r3, [r1]
    adds r1, #4
    b clear_bss

initialize_runtime:
    /* Preserve SDK runtime initializers and C library setup. */
    bl runtime_init
    bl main

hang:
    b hang

/* Copy [r0, r0 + (r2 - r1)) to [r1, r2). */
.thumb_func
copy_region:
    cmp r1, r2
    bcs copy_region_done
copy_region_loop:
    ldr r3, [r0]
    str r3, [r1]
    adds r0, #4
    adds r1, #4
    cmp r1, r2
    bcc copy_region_loop
copy_region_done:
    bx lr

/* Keep the SDK binary metadata's RAM initialization table in sync. */
.section .rodata.data_cpy_table, "a", %progbits
.align 2
.global data_cpy_table
data_cpy_table:
    .word __etext
    .word __data_start__
    .word __data_end__
    .word __scratch_x_source__
    .word __scratch_x_start__
    .word __scratch_x_end__
    .word __scratch_y_source__
    .word __scratch_y_start__
    .word __scratch_y_end__
    .word 0
