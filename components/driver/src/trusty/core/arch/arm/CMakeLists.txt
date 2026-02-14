target_sources(${target} PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
	${CMAKE_CURRENT_LIST_DIR}/dev.c
	${CMAKE_CURRENT_LIST_DIR}/mem.c

    # {{END_TARGET_SOURCES}}
)

target_include_directories(${target} PUBLIC
)
