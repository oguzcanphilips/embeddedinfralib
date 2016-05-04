#include <gtest/gtest.h>
#include "../CRC.h"

TEST(TestCRC, InitTable)
{
    CRC_InitTable();
}

TEST(TestCRC, Update)
{
    uint16_t crc = 0;
    for(uint8_t i=0;i<100;++i)
    {
        CRC_Update(&crc, i*17);
    }
    ASSERT_EQ(38132,crc);
}

TEST(TestCRC, CheckCRCEven)
{
    uint8_t data[18] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,0};
    uint16_t crc = 0;
    for(uint8_t i=0;i<16;++i)
    {
        CRC_Update(&crc, data[i]);
    }
    data[16] = crc >> 8;
    data[17] = crc & 0xff;

    crc = 0;
    for(uint8_t i=0;i<18;++i)
    {
        CRC_Update(&crc, data[i]);
    }
    ASSERT_EQ(0,crc);
}

TEST(TestCRC, CheckCRCOdd)
{
    uint8_t data[18] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,0};
    uint16_t crc = 0;
    for(uint8_t i=1;i<16;++i)
    {
        CRC_Update(&crc, data[i]);
    }
    data[16] = crc >> 8;
    data[17] = crc & 0xff;

    crc = 0;
    for(uint8_t i=1;i<18;++i)
    {
        CRC_Update(&crc, data[i]);
    }
    ASSERT_EQ(0,crc);
}
