// Copyright 3bian Limited, lowRISC and CHERIoT Contributors.
// SPDX-License-Identifier: Apache-2.0

/**
 * This example demonstrates how to initialize the I2C interface, communicate
 * with the DS3231 RTC, read the current time, and display it using debug logs.
 *
 * Requirements:
 * - DS3231 Real Time Clock module connected to the qwiic0 connector.
 *   (https://www.adafruit.com/product/3013)
 */

#include <compartment.h>
#include <debug.hh>
#include <ds3231-control.hh>
#include <ds3231-datetime.hh>
#include <ds3231-temperature.hh>
#include <platform-i2c.hh>
#include <thread.h>

/// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "RTCC Example">;

/// Thread entry point.
[[noreturn]] void __cheri_compartment("rtcc_example") init()
{
	// Obtain the memory-mapped I2C0 interface.
	auto i2c0 = MMIO_CAPABILITY(OpenTitanI2c, i2c0);

	// Configure the I2C interface.
	i2c0->reset_fifos();
	i2c0->host_mode_set();
	i2c0->speed_set(100);  // Set I2C speed to 100 kHz.

	// Start the oscillator.
	DS3231::Control<OpenTitanI2c> control(i2c0);
	control.retrieve();
	control.set_disable_oscillator(false);
	control.store();

	DS3231::DateTime<OpenTitanI2c> datetime(i2c0);

	// Set date and time
	//datetime.set_weekday(DS3231::Weekday::Wednesday);
	//datetime.set_day(2);
	//datetime.set_month(10);
	//datetime.set_century(true);
	//datetime.set_year(24);
	//datetime.set_international_time(false);
	//datetime.set_hours(9);
	//datetime.set_minutes(30);
	//datetime.set_seconds(0);
	//datetime.set_meridian(DS3231::Meridian::AM);
	//datetime.store();

	DS3231::Temperature<OpenTitanI2c> temperature(i2c0);

 	// Infinite loop: Read and display time every second.
	while (true)
	{
		// Display the current time.
		if (datetime.retrieve())
		{
			Debug::log("hours           : {}", datetime.hours());
			Debug::log("minutes         : {}", datetime.minutes());
			Debug::log("seconds         : {}", datetime.seconds());
			Debug::log("meridian        : {}", datetime.meridian());
			Debug::log("weekday         : {}", datetime.weekday());
			Debug::log("day             : {}", datetime.day());
			Debug::log("month           : {}", datetime.month());
			Debug::log("year            : {}", datetime.year());
		}
		else
		{
			Debug::log("Unable to read datetime");
		}

		// Display the current temperature.
		if (temperature.retrieve())
		{
			Debug::log("temp degrees    : {}", temperature.degrees());
			Debug::log("temp quarters   : {}", temperature.quarters());
		}
		else
		{
			Debug::log("Unable to read temperature");
		}

		thread_millisecond_wait(1000);
	}
}