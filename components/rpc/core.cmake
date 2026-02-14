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
if(CONFIG_MODEM_LTE_LIB)
set(libname rpc_core_modem_lte_lib)
else()
set(libname rpc_core)
endif()
set(import_lib ${out_lib_dir}/lib${libname}.a)
# solve platform issue by fibocom,20240924,begin
if(CONFIG_SOC_8910 AND ((CONFIG_NVM_VARIANTS STREQUAL "UIS8910DM_C3_DM_novolte_cus") OR (CONFIG_NVM_VARIANTS STREQUAL "UIS8910DM_C2_DM_novolte_cus_lite")))
configure_file(core/${CONFIG_SOC}/8910dm_c3/lib${libname}.a ${import_lib} COPYONLY)
else()
configure_file(core/${CONFIG_SOC}/lib${libname}.a ${import_lib} COPYONLY)
endif()
# solve platform issue by fibocom,20240924,end
add_app_libraries(${import_lib})
add_library(${target} STATIC IMPORTED GLOBAL)
set_target_properties(${target} PROPERTIES
    IMPORTED_LOCATION ${import_lib}
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
