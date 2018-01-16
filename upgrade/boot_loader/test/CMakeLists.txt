ccola_component(boot_loader_test gmock_test)

    ccola_sources(
        MockDecryptor.cpp
        MockDecryptor.hpp
        MockDiComm.cpp
        MockDiComm.hpp
        TestDecryptorAes.cpp
        TestDiCommUart.cpp
        TestImageUpgraderDiComm.cpp
        TestImageUpgraderInternalFlash.cpp
        TestImageUpgraderSkip.cpp
        TestPackUpgrader.cpp
        TestSecondStageToRamLoader.cpp
    )

    ccola_dependencies(
        boot_loader
        infra.stream
        hal.synchronous_test_doubles
    )

ccola_end_component()
