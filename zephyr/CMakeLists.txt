if(CONFIG_COBS)
    zephyr_library()

    zephyr_library_sources(
        ../src/cobs.c
        ../src/framing.c
    )

    zephyr_include_directories(
        ../include
    )
endif()
