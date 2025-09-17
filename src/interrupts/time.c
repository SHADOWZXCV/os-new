#include "time.h"

int century_register = 0x00; 
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;
unsigned char M[3];

void enable_clock_timer_chip() {
    __asm__("cli");
    // enable the clock timer chip
    out(0x70, 0x8B);
    unsigned char prev = in(0x71);
    out(0x70, 0x8B);
    out(0x71, prev | 0x40); // set the 7th bit of register B ( enable the interrupt chip )
    // reduce the frequency to 64 Hz
    out(0x70, 0x8A);
    prev = in(0x71);
    out(0x70, 0x8A);
    out(0x71, (prev & 0xF0) | 0x0A); // set the rate
    __asm__("sti");
}

int get_update_in_progress_flag() {
      out(CMOS_RTC_INDEX_PORT, 0x0A);
      return (in(CMOS_RTC_REG_PORT) & 0x80);
}

unsigned char get_RTC_register(int reg) {
      out(CMOS_RTC_INDEX_PORT, reg);
      return in(CMOS_RTC_REG_PORT);
}

void timer_handler() {
	unsigned char century;
	unsigned char last_second;
	unsigned char last_minute;
	unsigned char last_hour;
	unsigned char last_day;
	unsigned char last_month;
	unsigned char last_year;
	unsigned char last_century;
	unsigned char registerB;

	// Note: This uses the "read registers until you get the same values twice in a row" technique
	//       to avoid getting dodgy/inconsistent values due to RTC updates

	while (get_update_in_progress_flag());                // Make sure an update isn't in progress
	second = get_RTC_register(0x00);
	minute = get_RTC_register(0x02);
	hour = get_RTC_register(0x04);
	day = get_RTC_register(0x07);
	month = get_RTC_register(0x08);
	year = get_RTC_register(0x09);
	if(century_register != 0) {
		century = get_RTC_register(century_register);
	}

	do {
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;
		last_century = century;

		while (get_update_in_progress_flag());           // Make sure an update isn't in progress
		second = get_RTC_register(0x00);
		minute = get_RTC_register(0x02);
		hour = get_RTC_register(0x04);
		day = get_RTC_register(0x07);
		month = get_RTC_register(0x08);
		year = get_RTC_register(0x09);
		if(century_register != 0) {
				century = get_RTC_register(century_register);
		}
	} while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
			(last_day != day) || (last_month != month) || (last_year != year) ||
			(last_century != century) );

	registerB = get_RTC_register(0x0B);

	// Convert BCD to binary values if necessary

	if (!(registerB & 0x04)) {
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
		if(century_register != 0) {
				century = (century & 0x0F) + ((century / 16) * 10);
		}
	}

	// Convert 12 hour clock to 24 hour clock if necessary

	if (!(registerB & 0x02) && (hour & 0x80)) {
		hour = ((hour & 0x7F) + 12) % 24;
	}

	// Calculate the full (4-digit) year

	if(century_register != 0) {
		year += century * 100;
	} else {
		year += 2000;
		if(year < 2000) year += 100;
	}
}
