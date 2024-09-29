// Copyright 3bian Limited, lowRISC, and CHERIoT Contributors.
// SPDX-License-Identifier: Apache-2.0

/**
 * @file rtcc_example.cc
 * @brief Example program to communicate with the DS3231 RTCC module via I2C.
 *
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
#include <platform-i2c.hh>
#include <thread.h>

// Debugging Setup
using Debug = ConditionalDebug<true, "RTCC Example">;

// Command Constants
constexpr uint8_t RTCC_I2C_CLEAR_EOSC_BIT[2]   = { 0x0E, 0b00011100 };
constexpr uint8_t RTCC_I2C_REQUEST_DATETIME[1] = { 0x00 };
constexpr uint8_t RTCC_I2C_ADDRESS             = 0x68;

// Memory-Mapped I/O Template
template <class T>
using Mmio = volatile T*;

/**
 * @brief Converts a BCD-encoded byte to its binary equivalent.
 *
 * @param x BCD-encoded byte.
 * @return Binary equivalent of the BCD byte.
 */
uint8_t bcd2bin(uint8_t x)
{
    return (((x >> 4) & 0x0F) * 10) + (x & 0x0F);
}

/**
 * @brief Initializes the I2C interface and reads time from the DS3231 RTC.
 *
 * This function sets up the I2C peripheral, clears the EOSC bit to ensure the
 * oscillator is enabled, and enters an infinite loop to read and display the
 * current time every second.
 */
[[noreturn]] void __cheri_compartment("rtcc_example") init()
{
    // I2C Configuration
    auto i2cSetup = [](Mmio<OpenTitanI2c> i2c) {
        i2c->reset_fifos();      // Reset I2C FIFOs
        i2c->host_mode_set();    // Set I2C to host mode
        i2c->speed_set(100);     // Set I2C speed to 100 kHz
    };

    // Obtain the memory-mapped I2C0 interface
    auto i2c0 = MMIO_CAPABILITY(OpenTitanI2c, i2c0);

    // Initialize the I2C0 interface
    i2cSetup(i2c0);

    // Buffer to store read data
    uint8_t receive_buffer[7]; 

    // Clear the EOSC bit to enable the oscillator
    Debug::log("Clearing EOSC bit...");
    i2c0->blocking_write(RTCC_I2C_ADDRESS, RTCC_I2C_CLEAR_EOSC_BIT,
                         sizeof(RTCC_I2C_CLEAR_EOSC_BIT), true);

    // Infinite Loop: Read and Display Time Every Second
    while (true)
    {
        // Request time data from RTCC
        Debug::log("Requesting time data from RTCC...");
        i2c0->blocking_write(RTCC_I2C_ADDRESS, RTCC_I2C_REQUEST_DATETIME,
                             sizeof(RTCC_I2C_REQUEST_DATETIME), true);

        // Read 7 bytes of time data from RTCC
        Debug::log("Reading time data from RTCC at address 0x68...");
        size_t bytes_read = i2c0->blocking_read(RTCC_I2C_ADDRESS, receive_buffer,
                                                sizeof(receive_buffer));

        Debug::log("{} bytes received.", bytes_read);

        // Convert BCD Data to Binary
        Debug::log("Converting BCD values to binary...");
        uint8_t seconds = bcd2bin(receive_buffer[0]);
        uint8_t minutes = bcd2bin(receive_buffer[1]);
        uint8_t hours   = bcd2bin(receive_buffer[2]);
        uint8_t weekday = bcd2bin(receive_buffer[3]);
        uint8_t day     = bcd2bin(receive_buffer[4]);
        uint8_t month   = bcd2bin(receive_buffer[5]);
        uint8_t year    = bcd2bin(receive_buffer[6]);

        // Display the current time
        Debug::log("Current Time: {}:{}:{}", hours, minutes, seconds);

        // Wait for 1 second before the next read
        thread_millisecond_wait(1000);
    }
}
