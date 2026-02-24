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

configure_file(include/cust_app.h.in ${out_inc_dir}/cust_app.h)
set(target appimg)

if(CONFIG_FIBOCOM_BASE)
    add_library(${target} STATIC
        src/app_codes.c
        src/app_at_cmd.c
        src/flash_partinfo.c
        src/fibo_config.c
        src/fibo_vendor_config.c
        src/fibo_old_api.c)

    target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_APPSTART)
    set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
    target_include_targets(${target} PRIVATE
    atr ats kernel hal driver ml fs sffs fsmount mal service
    cfw net nvm diag audio apploader fibocom)
    target_include_directories(${target} PRIVATE ../cust_app/uartdebug/debug/include)
    target_include_directories(${target} PRIVATE ../components/tts/include)
    target_include_directories(${target} PRIVATE ../fibocom/opencpu)
    target_include_directories(${target} PRIVATE ../fibocom/api)
    target_include_directories(${target} PRIVATE ./include)
    target_include_directories(${target} PRIVATE ../components/firmwareupdate/include)
    target_include_directories(${target} PRIVATE ../components/calclib/include)
    target_include_directories(${target} PUBLIC  ../components/net/lwip/src/include/lwip)
    target_include_directories(${target} PUBLIC  ../components/net/lwip/src/include)
    target_include_directories(${target} PUBLIC  ../components/net/lwip/include)
    target_include_directories(${target} PUBLIC  ../components/net/libexpat/include)
    target_include_directories(${target} PUBLIC  ../components/net/json/cJSON)
    target_include_directories(${target} PRIVATE ../fibocom/ble/inc)
    target_include_directories(${target} PRIVATE ../fibocom/fibo-framework/lib/include)
    target_include_directories(${target} PRIVATE ../fibocom/fibo-framework/plateform/unisoc/include)
    target_include_directories(${target} PRIVATE ../components/atr/src)
    target_include_directories(${target} PRIVATE ../components/atr/include)
    target_include_directories(${target} PRIVATE ../components/driver/include)
    target_include_directories(${target} PRIVATE ../components/driver/include/usb)

if(CONFIG_FIBOCOM_XIAONIU_FEATURE)
    target_include_directories(${target} PRIVATE ../components/fibocustom/NIU/)
endif()
    target_link_libraries(${BUILD_TARGET} PRIVATE all_libs ${target} all_libs)
endif()

add_subdirectory_if_exist(didi)
add_subdirectory_if_exist(jio_custom)

if(CONFIG_APPIMG_LOAD_FLASH)
    set(target appimg_flash)

    if(CONFIG_APPIMG_APP_DEMO)
    message(STATUS "app demo enbale")
    add_definitions(-DCONFIG_APPIMG_APP_DEMO)
    file(GLOB APP_DEMO_SOURCES_FILE ../components/apploader/pack/fibo_opencpu_demo/app_demo/*.c)
    endif()
    if(CONFIG_APPIMG_APP_TEST)
        set(target testapi)
        add_appimg_flash_example(${target} src/app_ver.c)
        target_link_libraries(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../target/${BUILD_TARGET}/libs/libtestcase.a ${libc_file_name} ${libm_file_name} ${libgcc_file_name})
    else()
        set(target appimg_flash_Operator_Name)
        add_appimg_flash_example(${target} Project/main.c src/app_ver.c uartdebug/debug/src/uartdebug.c
        ${APP_DEMO_SOURCES_FILE})
         # Add service module
        target_include_directories(${target} PRIVATE Project/service/inc)
        target_sources(${target} PRIVATE Project/service/sim.c)
        target_sources(${target} PRIVATE Project/service/gps.c)
        target_sources(${target} PRIVATE Project/service/sms.c)
        target_sources(${target} PRIVATE Project/service/tcp.c)
    endif()

    target_include_targets(${target} PRIVATE
    atr ats kernel hal driver ml fs sffs fsmount mal service
    cfw net nvm diag audio apploader fibocom)
    target_include_directories(${target} PRIVATE ../cust_app/uartdebug/debug/include)
    target_include_directories(${target} PRIVATE ../components/tts/include)
    target_include_directories(${target} PRIVATE ../fibocom/opencpu)
    target_include_directories(${target} PRIVATE ../fibocom/api)
    target_include_directories(${target} PRIVATE ./include)
    target_include_directories(${target} PRIVATE ../components/firmwareupdate/include)
    target_include_directories(${target} PRIVATE ../components/calclib/include)
    target_include_directories(${target} PUBLIC  ../components/net/lwip/src/include/lwip)
    target_include_directories(${target} PUBLIC  ../components/net/lwip/src/include)
    target_include_directories(${target} PUBLIC  ../components/net/lwip/include)
    target_include_directories(${target} PUBLIC  ../components/net/libexpat/include)
    target_include_directories(${target} PUBLIC  ../components/net/json/cJSON)
    target_include_directories(${target} PRIVATE ../fibocom/ble/inc)
    target_include_directories(${target} PRIVATE ../fibocom/fibo-framework/lib/include)
    target_include_directories(${target} PRIVATE ../fibocom/fibo-framework/plateform/unisoc/include)
    target_include_directories(${target} PRIVATE ../components/atr/src)
    target_include_directories(${target} PRIVATE ../components/atr/include)
if(CONFIG_FIBOCOM_XIAONIU_FEATURE)
    target_include_directories(${target} PRIVATE ../components/fibocustom/NIU/)
endif()
if(CONFIG_APPIMG_APP_DEMO)
target_include_directories(${target} PRIVATE ../components/apploader/pack/fibo_opencpu_demo/app_demo)
endif()
endif()

if(CONFIG_APPIMG_LOAD_FILE)
    set(target appimg_file)
    add_appimg_file_example(${target} src/app_image.c)
    target_include_targets(${target} PRIVATE kernel)
endif()
relative_glob(srcs include/*.h src/*.c src/*.h *.c)
beautify_c_code(${target} ${srcs})