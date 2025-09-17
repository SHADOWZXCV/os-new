#ifndef TIME_H_
#define TIME_H_
#define CMOS_RTC_INDEX_PORT 0x70
#define CMOS_RTC_REG_PORT 0x71

#include "print.h"
#include "IO/io.h"

extern int century_register;
extern unsigned char second;
extern unsigned char minute;
extern unsigned char hour;
extern unsigned char day;
extern unsigned char month;
extern unsigned int year;

void enable_clock_timer_chip();
void timer_handler();
unsigned char get_RTC_register(int reg);
int get_update_in_progress_flag();

#endif
