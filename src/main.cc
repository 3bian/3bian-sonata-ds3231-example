// Copyright 3bian Limited, lowRISC, and CHERIoT Contributors.
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
#include <ctype.h>
#include <debug.hh>
#include <ds3231.hh>
#include <platform-i2c.hh>
#include <thread.h>

/// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "RTCC Example">;

/// Thread entry point.
[[noreturn]] void __cheri_compartment("rtcc_example") init()
{
    DS3231::DateTime datetime;
    DS3231::Temperature temperature;

    // Obtain the memory-mapped I2C0 interface.
    auto i2c0 = MMIO_CAPABILITY(OpenTitanI2c, i2c0);

    // Configure the I2C interface.
    i2c0->reset_fifos();
    i2c0->host_mode_set();
    i2c0->speed_set(100);  // Set I2C speed to 100 kHz.

    // Infinite loop: Read and display time every second.
    while (true)
    {
        // Display the current time.
        if (DS3231::read_datetime(i2c0, &datetime))
        {
            Debug::log("Hours:   tens:{} units:{}", datetime.hour_tens, datetime.hour_units);
            Debug::log("Minutes: tens:{} units:{}", datetime.minute_tens, datetime.minute_units);
            Debug::log("Seconds: tens:{} units:{}", datetime.second_tens, datetime.second_units);
        }

        // Display the current temperature.
        if (DS3231::read_temperature(i2c0, &temperature))
        {
            Debug::log("Temperature: degrees:{} quarters: {}", temperature.degrees, temperature.quarters);
        }

        // Wait for 1 second before the next read.
        thread_millisecond_wait(1000);
    }
}
