//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "stm32f0xx_conf.h"

#include "analog.h"
#include "lcd.h"


// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[]) {
  // By customizing __initialize_args() it is possible to pass arguments,
  // for example when running tests with semihosting you can pass various
  // options to the test.
  // trace_dump_args(argc, argv);

  // Send a greeting to the trace device (skipped on Release).
  trace_puts("Hello ARM World!");

  // The standard output and the standard error should be forwarded to
  // the trace device. For this to work, a redirection in _write.c is
  // required.

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %u Hz\n", SystemCoreClock);

  trace_printf("%s", "Initializing ADC...");
  adc_init();
  adc_enable_pot(1);
  trace_puts("Done");

  trace_printf("%s", "Initializing DAC...");
  dac_init();
  trace_puts("Done");

  trace_printf("%s", "Initializing LCD...");
  lcd_init();
  trace_puts("Done");

  trace_printf("%s", "Initializing frequency monitor...");
  freq_init();
  trace_puts("Done");

  // Infinite loop, wait for interrupts to do anything
  while (1) {

  }
  // Infinite loop, never return.
  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
