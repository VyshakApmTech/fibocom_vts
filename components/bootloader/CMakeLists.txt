# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

configure_file(include/boot_config.h.in ${out_inc_dir}/boot_config.h)
install_headers(include/image_header.h OUTPUT ${out_inc_dir})

if(CONFIG_FIBOCOM_BASE)
    set(target fibo_boot_cfg)
    set (LIB_SOURCE_FILE
        fibo_boot_config/src/fibo_boot_cfg.c
    )

    add_library(${target} STATIC ${LIB_SOURCE_FILE})
    target_include_directories(${target} PUBLIC include fibo_boot_config/inc)
    target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BOOT)
    target_link_libraries(${target} PRIVATE ${libc_file_name} ${libgcc_file_name})
    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
endif()

if(CONFIG_SOC_8910)
set(target fibo_8910_bootloader)
    add_library(${target} STATIC

        src/boot_fdl.c

    )
    if(CONFIG_LCD_SUPPORT)
        set(LIB_SOURCE_FILE ${LIB_SOURCE_FILE}
            src/boot_lcd.c
        )
    endif()


    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
    target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BOOT)
    target_include_directories(${target} PUBLIC include)
    target_include_directories(${target} PUBLIC ../kernel/include/)
    if(CONFIG_FIBOCOM_BASE)
        target_include_directories(${target} PUBLIC fibo_boot_config/inc)
        target_link_libraries(${target} PRIVATE fibo_boot_cfg)
    endif()
    target_link_libraries(${target} PRIVATE calclib boot_calclib_crc)
    target_include_targets(${target} PRIVATE hal fs sffs bdev driver fsmount nvm cpio_parser fibo_fsmount)
if(CONFIG_LCD_SUPPORT)
    target_include_targets(${target} PRIVATE lcdpanel)
endif()
include(bootloader.cmake)


set(rmarun_ld ldscripts/ramrun_8910.ld)

set(target boot2)
    add_uimage(${target} ${rmarun_ld} 8910/boot2_start.c)
    target_compile_definitions(${target}_ldscript PRIVATE
        RAMRUN_IMAGE_START=${CONFIG_BOOT2_IMAGE_START}
        RAMRUN_IMAGE_SIZE=${CONFIG_BOOT2_IMAGE_SIZE}
        RAMRUN_MMU_ENABLE=1
)
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BOOT)
    target_link_whole_archive(${target} PRIVATE bootloader fibo_8910_bootloader)
target_link_group(${target} PRIVATE hal osi_lib fupdate fupdate_security_boot fsmount fibo_fsmount sffs bdev bdev_core)
target_include_targets(${target} PRIVATE hal fs sffs bdev fupdate fupdate_security_boot driver )
target_link_libraries(${target} PRIVATE ${libc_file_name} ${libgcc_file_name})

set(boot2_img ${out_hex_dir}/boot2.img)
set(boot2_lzma ${boot2_img}.lzmar)
    add_custom_command(OUTPUT ${boot2_lzma}
        COMMAND ${cmd_lzmare2} ${boot2_img} ${boot2_lzma}
        DEPENDS ${boot2_img}
)

set(target boot)
    add_uimage(${target} ${rmarun_ld} 8910/boot_start.c)
    target_compile_definitions(${target}_ldscript PRIVATE
        RAMRUN_IMAGE_START=${CONFIG_BOOT_IMAGE_START}
        RAMRUN_IMAGE_SIZE=${CONFIG_BOOT_IMAGE_SIZE}
        RAMRUN_FLASH_SIZE=${CONFIG_BOOT_UNSIGN_IMAGE_SIZE}
        RAMRUN_MMU_ENABLE=0
)
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BOOT)
    target_link_whole_archive(${target} PRIVATE bootloader fibo_8910_bootloader)
target_link_group(${target} PRIVATE hal calclib boot_calclib_crc)
target_include_targets(${target} PRIVATE kernel)
target_link_libraries(${target} PRIVATE ${libc_file_name} ${libgcc_file_name})
target_incbin(${target} ${boot2_lzma} gBoot2CompData BALIGN 8)

set(target fdl1)
    add_uimage(${target} ${rmarun_ld} 8910/fdl1.c)
    target_compile_definitions(${target}_ldscript PRIVATE
        RAMRUN_IMAGE_START=${CONFIG_FDL1_IMAGE_START}
        RAMRUN_IMAGE_SIZE=${CONFIG_FDL1_IMAGE_SIZE}
        RAMRUN_FLASH_SIZE=${CONFIG_BOOT_UNSIGN_IMAGE_SIZE}
        RAMRUN_MMU_ENABLE=0
)
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BOOT)
    target_link_whole_archive(${target} PRIVATE bootloader fibo_8910_bootloader)
target_link_group(${target} PRIVATE hal osi_lib)
target_include_targets(${target} PRIVATE driver)
target_link_libraries(${target} PRIVATE ${libc_file_name} ${libgcc_file_name})

set(target fdl2)
    add_uimage(${target} ${rmarun_ld} 8910/fdl2.c)
    target_compile_definitions(${target}_ldscript PRIVATE
        RAMRUN_IMAGE_START=${CONFIG_FDL2_IMAGE_START}
        RAMRUN_IMAGE_SIZE=${CONFIG_FDL2_IMAGE_SIZE}
        RAMRUN_MMU_ENABLE=1
)
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_BOOT)
target_include_targets(${target} PRIVATE fs fsmount nvm calclib cpio_parser driver)
    target_link_whole_archive(${target} PRIVATE bootloader cpio_parser fibo_8910_bootloader)
target_link_group(${target} PRIVATE hal calclib boot_calclib_crc sffs fsmount fibo_fsmount bdev bdev_core osi_lib nvm lz4 fs)
target_link_libraries(${target} PRIVATE ${libc_file_name} ${libgcc_file_name})
target_incbin_size(${target} ${CMAKE_CURRENT_SOURCE_DIR}/8910/calibparam.bin gDefCalibParam gDefCalibParamSize)
endif()

if(CONFIG_8910)
    target_link_libraries(${target} PRIVATE calclib boot_calclib_crc)
    target_include_targets(${target} PRIVATE hal fs sffs bdev driver fsmount 
            nvm cpio_parser hal_open calclib boot_calclib_crc)
endif()

if(CONFIG_SOC_8850)
    include(bootloader.cmake)
    include(img.cmake)
endif()
