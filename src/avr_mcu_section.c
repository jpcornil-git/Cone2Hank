#include <Arduino.h>
#include "avr_mcu_section.h"

extern void *__brkval;

AVR_MCU (F_CPU, "atmega32u4" );
AVR_MCU_VOLTAGES(3300, 3300, 3300);
AVR_MCU_VCD_FILE("simavr.vcd", 10000000);

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
//  { AVR_MCU_VCD_SYMBOL ( " UDR0 " ) , . what = ( void *) & UDR0, },
//  { AVR_MCU_VCD_SYMBOL ( " UDRE0 " ) , . mask = (1 << UDRE0) , . what = ( void *) & UCSR0A , } ,
  { AVR_MCU_VCD_SYMBOL("Encoder"), .what = (void*) &PIND, .mask=(1<<PIND2), },
  { AVR_MCU_VCD_SRAM_16("Stack"), .what = (void*) &SP, },
	{ AVR_MCU_VCD_SRAM_16("Heap"), .what = (void*)(&__brkval), },
};