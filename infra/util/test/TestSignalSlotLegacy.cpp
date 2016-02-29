#define COMPILERCPP11 0
#include <gtest/gtest.h>
#include "infra\util\public\SignalSlot.hpp"
#include <stdint.h>
class SenderLegacy
{
public:
    infra::Signal<SenderLegacy, uint8_t, uint16_t> s2;
    infra::Signal<SenderLegacy, uint8_t, uint16_t, uint32_t> s3;
    void Fire2(uint8_t a, uint16_t b)
    {
        s2(a,b);
    }
    void Fire3(uint8_t a, uint16_t b, uint32_t c)
    {
        s3(a,b,c);
    }
};

class ReceiverLegacy
{
public:
    infra::Slot<ReceiverLegacy, uint8_t, uint16_t> r2;
    infra::Slot<ReceiverLegacy, uint8_t, uint16_t, uint32_t> r3;
    ReceiverLegacy()
        : r2(this, &ReceiverLegacy::Update)
        , r3(this, &ReceiverLegacy::Update3)
    {}

    uint8_t a2;
    uint16_t b2;
    uint8_t a3;
    uint16_t b3; 
    uint32_t c3;
private:
    void Update(uint8_t a, uint16_t b)
    {
        a2 = a;
        b2 = b;
    }
    void Update3(uint8_t a, uint16_t b, uint32_t c)
    {
        a3 = a;
        b3 = b;
        c3 = c;
    }
};

class ReceiverSlotsLegacy
{
public:
    infra::Signal<ReceiverSlotsLegacy, uint8_t, uint16_t> signal;
    infra::Slot<ReceiverSlotsLegacy, uint8_t, uint16_t> s1;
    infra::Slot<ReceiverSlotsLegacy, uint8_t, uint16_t> s2;
    ReceiverSlotsLegacy()
        : s1(this, &ReceiverSlotsLegacy::Update1)
        , s2(this, &ReceiverSlotsLegacy::Update2)
        , a1(0), b1(0), a2(0), b2(0)
    {
        signal += s2;
        signal += s1;
    }

    uint8_t a1;
    uint16_t b1;
    uint8_t a2;
    uint16_t b2;

    void FireSignal(uint8_t a, uint16_t b)
    {
        signal(a, b);
    }
private:
    void Update1(uint8_t a, uint16_t b)
    {
        a1 = a;
        b1 = b;
        signal -= s1;
        signal -= s2;
    }
    void Update2(uint8_t a, uint16_t b)
    {
        a2 = a;
        b2 = b;
        signal -= s1;
        signal -= s2;
    }
};

TEST(TestSignalSlotLegacy, SendWithoutAttach)
{
    SenderLegacy s;
    s.Fire2(1,2);
}

TEST(TestSignalSlotLegacy, SendWithAttach)
{
    SenderLegacy s;
    ReceiverLegacy r;

    s.s2 += r.r2;
    s.s3 += r.r3;

    s.Fire2(1,2);
    s.Fire3(3,4,5);
    ASSERT_EQ(1, r.a2);
    ASSERT_EQ(2, r.b2);
    ASSERT_EQ(3, r.a3);
    ASSERT_EQ(4, r.b3);
    ASSERT_EQ(5, r.c3);

    s.s3 -= r.r3;
    s.Fire3(10,20,30);
    ASSERT_EQ(1, r.a2);
    ASSERT_EQ(2, r.b2);
    ASSERT_EQ(3, r.a3);
    ASSERT_EQ(4, r.b3);
    ASSERT_EQ(5, r.c3);
}

TEST(TestSignalSlotLegacy, RegisterAtOtherSignal)
{
    SenderLegacy s1;
    SenderLegacy s2;

    ReceiverLegacy r;

    s1.s2 += r.r2;

    s1.Fire2(12,34);
    ASSERT_EQ(12, r.a2);
    ASSERT_EQ(34, r.b2);

    s2.s2 += r.r2;
    s1.Fire2(0,0);
    ASSERT_EQ(12, r.a2);
    ASSERT_EQ(34, r.b2);
    s2.Fire2(123,345);
    ASSERT_EQ(123, r.a2);
    ASSERT_EQ(345, r.b2);
}

TEST(TestSignalSlotLegacy, RemoveDuringNotification)
{
    ReceiverSlotsLegacy rs;

    rs.FireSignal(1, 2);
    ASSERT_EQ(1, rs.a1);
    ASSERT_EQ(2, rs.b1);
    ASSERT_EQ(0, rs.a2);
    ASSERT_EQ(0, rs.b2);
}

