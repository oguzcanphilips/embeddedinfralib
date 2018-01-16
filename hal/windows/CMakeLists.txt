ccola_component(windows win: static_library other: noncompile)

    ccola_sources(
        FileSystemWin.cpp
        FileSystemWin.hpp
        SynchronousRandomDataGeneratorWin.hpp
        SynchronousRandomDataGeneratorWin.cpp
    )

    ccola_dependencies(
        interfaces
        synchronous_interfaces
    )

ccola_end_component()
