cmake_minimum_required(VERSION 3.10)

project(amigotest C ASM CXX)

set(srcs0
    lib/bsp/crt.S
    lib/bsp/entry.c
    lib/bsp/entry_user.c
    lib/bsp/interrupt.c
    lib/bsp/locks.c
    lib/bsp/printf.c
    lib/bsp/sleep.c
    lib/bsp/syscalls.c
    lib/drivers/aes.c
    lib/drivers/apu.c
    lib/drivers/clint.c
    lib/drivers/dmac.c
    lib/drivers/dvp.c
    lib/drivers/fft.c
    lib/drivers/fpioa.c
    lib/drivers/gpio.c
    lib/drivers/gpiohs.c
    lib/drivers/i2c.c
    lib/drivers/i2s.c
    lib/drivers/iomem.c
    lib/drivers/kpu.c
    lib/drivers/plic.c
    lib/drivers/pwm.c
    lib/drivers/rtc.c
    lib/drivers/sha256.c
    lib/drivers/spi.c
    lib/drivers/sysctl.c
    lib/drivers/timer.c
    lib/drivers/uart.c
    lib/drivers/uarths.c
    lib/drivers/utils.c
    lib/drivers/wdt.c
    )

set(srcs)
foreach(f ${srcs0})
    list(APPEND srcs kendryte-standalone-sdk/${f})
endforeach()

set(CMAKE_C_FLAGS "-g -Os -std=gnu11 -fstrict-volatile-bitfields -Wall")
set(CMAKE_CXX_FLAGS "-g -Os -std=gnu++11 -fno-exceptions -fno-rtti -fstrict-volatile-bitfields -Wall")
set(CMAKE_EXE_LINKER_FLAGS
    "-Wl,-T -Wl,${CMAKE_CURRENT_LIST_DIR}/kendryte-standalone-sdk/lds/kendryte.ld -Wl,--gc-sections -Wl,-Map,out.map -fuse-ld=bfd -nostdlib"
    )

add_definitions(
    -mcmodel=medany

    # from cmake/common.cmake
    -DCONFIG_LOG_LEVEL=LOG_VERBOSE
    -DCONFIG_LOG_ENABLE
    -DCONFIG_LOG_COLORS
    -DLOG_KERNEL
    -DLV_CONF_INCLUDE_SIMPLE
    -DTCB_SPAN_NO_EXCEPTIONS # xtl
    -DTCB_SPAN_NO_CONTRACT_CHECKING # xtl
    -DNNCASE_TARGET=k210
    -D__riscv64
    )

include_directories(
    kendryte-standalone-sdk/lib/bsp/include
    kendryte-standalone-sdk/lib/drivers/include
    kendryte-standalone-sdk/lib/utils/include
    kendryte-standalone-sdk/lib/nncase/include # for kpu driver
    )
add_library(bsp ${srcs})
add_executable(check 
    main.c
    amigo_powercfg.c
    amigo_lcd_ips.c
    )
target_link_libraries(check bsp c gloss gcc m)

add_custom_command(TARGET check POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
    -O binary $<TARGET_FILE:check> $<TARGET_FILE:check>.bin)

