if(${CCOLA_ARCH} STREQUAL win)
    ccola_package(gmock)

        ccola_subdirectories(
            googletest
            googlemock
        )

    ccola_end_package()

endif()

ccola_pack_directory(gmock_pack)
