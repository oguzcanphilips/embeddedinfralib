#ifndef PACKETCOMMUNICATIONCONFIG_H
#define PACKETCOMMUNICATIONCONFIG_H

#define SIGNAL_SLOT_HEADER "infra\util\public\SignalSlot.hpp"
#define SIGNAL infra::Signal
#define SLOT   infra::Slot

#define RECEIVE_TIMEOUT_MS 10000
#define PCSTRING_LEN       80
// select the message validation type. Validation is bypassed when none of the options is selected.
#define VALIDATION_CRC       // 16 bit CRC is used to validate messages.
#define VALIDATION_CHECKSUM  //  8 bit Checksum is used to validate messages.


#ifdef VALIDATION_CRC
    #undef VALIDATION_CHECKSUM
#endif

// System defines bool, inline
#if !defined(__cplusplus)
    #ifndef _MSC_VER
        #include <stdbool.h>
    #else
        #if !defined(bool)
        #define bool char
        #define false 0
        #define true  1
        #endif
        #if !defined(inline)
        #define inline __inline
        #endif
    #endif /* _MSC_VER */
#endif

#endif /* PACKETCOMMUNICATIONCONFIG_H */