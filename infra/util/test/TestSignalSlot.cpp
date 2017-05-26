#include <gtest/gtest.h>
#include "infra/util/SignalSlot.hpp"
#include <stdint.h>

class Sender
{
public:
    infra::Signal<Sender, uint8_t, uint16_t> s2;
    infra::Signal<Sender, uint8_t, uint16_t, uint32_t> s3;
    void Fire(uint8_t a, uint16_t b)
    {
        s2(a, b);
    }
    void Fire(uint8_t a, uint16_t b, uint32_t c)
    {
        s3(a, b, c);
    }
};

class Receiver
{
public:
    infra::Slot<Receiver, uint8_t, uint16_t> r2;
    infra::Slot<Receiver, uint8_t, uint16_t, uint32_t> r3;
    Receiver()
        : r2(this, &Receiver::Update2)
        , r3(this, &Receiver::Update3)
    {}

    uint8_t a2;
    uint16_t b2;
    uint8_t a3;
    uint16_t b3;
    uint32_t c3;
private:
    void Update2(uint8_t a, uint16_t b)
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

class ReceiverSlots
{
public:
    infra::Signal<ReceiverSlots, uint8_t, uint16_t> signal;
    infra::Slot<ReceiverSlots, uint8_t, uint16_t> s1;
    infra::Slot<ReceiverSlots, uint8_t, uint16_t> s2;
    ReceiverSlots()
        : s1(this, &ReceiverSlots::Update1)
        , s2(this, &ReceiverSlots::Update2)
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

TEST(TestSignalSlot, SendWithoutAttach)
{
    Sender s;
    s.Fire(1, 2);
}

TEST(TestSignalSlot, SendWithAttach)
{
    Sender s;
    Receiver r;

    s.s2 += r.r2;
    s.s3 += r.r3;

    s.Fire(1, 2);
    s.Fire(3, 4, 5);
    ASSERT_EQ(1, r.a2);
    ASSERT_EQ(2, r.b2);
    ASSERT_EQ(3, r.a3);
    ASSERT_EQ(4, r.b3);
    ASSERT_EQ(5, r.c3);

    s.s3 -= r.r3;
    s.Fire(10, 20, 30);
    ASSERT_EQ(1, r.a2);
    ASSERT_EQ(2, r.b2);
    ASSERT_EQ(3, r.a3);
    ASSERT_EQ(4, r.b3);
    ASSERT_EQ(5, r.c3);
}

TEST(TestSignalSlot, RegisterAtOtherSignal)
{
    Sender s1;
    Sender s2;

    Receiver r;

    s1.s2 += r.r2;

    s1.Fire(12, 34);
    ASSERT_EQ(12, r.a2);
    ASSERT_EQ(34, r.b2);

    s2.s2 += r.r2;
    s1.Fire(0, 0);
    ASSERT_EQ(12, r.a2);
    ASSERT_EQ(34, r.b2);
    s2.Fire(123, 345);
    ASSERT_EQ(123, r.a2);
    ASSERT_EQ(345, r.b2);
}

TEST(TestSignalSlot, RemoveDuringNotification)
{
    ReceiverSlots rs;

    rs.FireSignal(1, 2);
    ASSERT_EQ(1, rs.a1);
    ASSERT_EQ(2, rs.b1);
    ASSERT_EQ(0, rs.a2);
    ASSERT_EQ(0, rs.b2);
}

