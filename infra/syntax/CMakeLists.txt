ccola_component(syntax static_library)

    ccola_sources(
        Asn1.cpp
        Asn1Formatter.cpp
        Json.cpp
        JsonFormatter.cpp
        Asn1.hpp
        Asn1Formatter.hpp
        Json.hpp
        JsonFormatter.hpp
    )

    ccola_dependencies(
        stream
    )

ccola_end_component()

ccola_subdirectories(
    test
)
