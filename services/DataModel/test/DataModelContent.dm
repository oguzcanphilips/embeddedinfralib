#include "TestConfig.hpp"

FIELD_MINMAX(   0, NumberInt,   int,      1230,     0, 1000)
FIELD_MINMAX(   1, NumberShort, int16_t,  -678, -1000, 1000)
FIELD_MINMAX(  10, N10,         uint8_t,    33,     10,  100)
FIELD(         11, N11,         uint8_t,    33)
FIELD_P_MINMAX(12, N12,         uint32_t, 3311,0,100000000)
FIELD_P(       13, N13,         uint32_t, 13)

FIELD_P(       14, TestConfig1, TestConfig, {})
FIELD(         101, MobileConnected,    bool,        false)
FIELD(         102, NofPendingBNPMessages, int32_t,  0)
