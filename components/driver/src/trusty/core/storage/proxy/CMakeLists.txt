target_sources(${target} PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
	${CMAKE_CURRENT_LIST_DIR}/ipc.c
	${CMAKE_CURRENT_LIST_DIR}/proxy.c
	${CMAKE_CURRENT_LIST_DIR}/storage.c

    # {{END_TARGET_SOURCES}}
)

target_include_directories(${target} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/../interface/include/trusty/interface
)
