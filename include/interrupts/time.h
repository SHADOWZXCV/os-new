#ifndef TIME_H_
#define TIME_H_
#define CMOS_RTC_INDEX_PORT 0x70
#define CMOS_RTC_REG_PORT 0x71

#include "print.h"
#include "IO/io.h"
#include "types/primitives.h"

extern int century_register;
extern byte second;
extern byte minute;
extern byte hour;
extern byte day;
extern byte month;
extern dword year;

void enable_clock_timer_chip();
void timer_handler();
byte get_RTC_register(int reg);
int get_update_in_progress_flag();

#endif
