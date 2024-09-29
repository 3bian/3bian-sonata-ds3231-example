-- Copyright 3bian Limited, lowRISC and CHERIoT Contributors.
-- SPDX-License-Identifier: Apache-2.0

set_project("RTCC Example")

sdkdir = "third_party/cheriot_rtos/sdk"
set_toolchains("cheriot-clang")

includes("third_party/cheriot_rtos/sdk",
         "third_party/cheriot_rtos/sdk/lib")

option("board")
    set_default("sonata")

compartment("rtcc_example")
    add_deps("debug",
             "freestanding")
    add_files("src/rtcc_example.cc")

firmware("rtcc-example")
    add_deps("rtcc_example")
    on_load(function(target)
        target:values_set("board", "$(board)")
        target:values_set("threads", {
            {
                compartment = "rtcc_example",
                priority = 1,
                entry_point = "init",
                stack_size = 0x400,
                trusted_stack_frames = 1
            }
        }, {expand = false})
    end)
