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
#include <ds3231.hh>
#include <platform-i2c.hh>
#include <thread.h>

/// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "RTCC Example">;

/// Thread entry point.
[[noreturn]] void __cheri_compartment("rtcc_example") init()
{
    DS3231::Control control;
    DS3231::DateTime datetime;
    DS3231::Temperature temperature;

    // Obtain the memory-mapped I2C0 interface.
    auto i2c0 = MMIO_CAPABILITY(OpenTitanI2c, i2c0);

    // Configure the I2C interface.
    i2c0->reset_fifos();
    i2c0->host_mode_set();
    i2c0->speed_set(100);  // Set I2C speed to 100 kHz.

    // Start the oscillator.
    DS3231::read_control(i2c0, control);
    control.set_disable_oscillator(false);
    DS3231::write_control(i2c0, control);

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
	//DS3231::write_datetime(i2c0, datetime);

    // Infinite loop: Read and display time every second.
    while (true)
    {
        // Display the current time.
        if (DS3231::read_datetime(i2c0, datetime))
        {
            Debug::log("hours           : {}", datetime.get_hours());
            Debug::log("minutes         : {}", datetime.get_minutes());
            Debug::log("seconds         : {}", datetime.get_seconds());
			Debug::log("meridian        : {}", datetime.get_meridian());
			Debug::log("weekday         : {}", datetime.get_weekday());
			Debug::log("day             : {}", datetime.get_day());
			Debug::log("month           : {}", datetime.get_month());
			Debug::log("year            : {}", datetime.get_year());
        }
        else
        {
            Debug::log("Unable to read datetime");
        }

        // Display the current temperature.
        if (DS3231::read_temperature(i2c0, temperature))
        {
            Debug::log("temp degrees    : {}", temperature.get_degrees());
            Debug::log("temp quarters   : {}", temperature.get_quarters());
        }
        else
        {
            Debug::log("Unable to read temperature");
        }

        // Wait for 1 second before the next read.
        thread_millisecond_wait(1000);
    }
}
