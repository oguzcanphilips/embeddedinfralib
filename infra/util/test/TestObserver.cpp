#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "infra/util/public/Observer.hpp"

class MySubject;

class MyObserver
    : public infra::Observer<MyObserver, MySubject>
{
public:
    MyObserver() = default;

    MyObserver(MySubject& subject)
        : infra::Observer<MyObserver, MySubject>(subject)
    {}

    using infra::Observer<MyObserver, MySubject>::Attached;
    using infra::Observer<MyObserver, MySubject>::Attach;
    using infra::Observer<MyObserver, MySubject>::Detach;

    MOCK_METHOD0(Callback, void());
    MOCK_METHOD0(Query, int());
};

class MySubject
    : public infra::Subject<MyObserver>
{};

class MySingleSubject;

class MySingleObserver
    : public infra::SingleObserver<MySingleObserver, MySingleSubject>
{
public:
    MySingleObserver() = default;

    MySingleObserver(MySingleSubject& subject)
        : infra::SingleObserver<MySingleObserver, MySingleSubject>(subject)
    {}

    using infra::SingleObserver<MySingleObserver, MySingleSubject>::Attached;
    using infra::SingleObserver<MySingleObserver, MySingleSubject>::Attach;
    using infra::SingleObserver<MySingleObserver, MySingleSubject>::Detach;

    MOCK_METHOD0(Callback, void());
    MOCK_METHOD0(Query, int());
};

class MySingleSubject
    : public infra::Subject<MySingleObserver>
{};

class MyAggregator
{
public:
    void operator()(int x)
    {
        Aggregate(x);
    }

    MOCK_METHOD1(Aggregate, void(int));
};

TEST(ObserverTest, ConstructWithoutRegistration)
{
    MyObserver observer;
    EXPECT_FALSE(observer.Attached());
}

TEST(ObserverTest, ConstructWithRegistration)
{
    MySubject subject;

    MyObserver observer(subject);
    EXPECT_TRUE(observer.Attached());
}

TEST(ObserverTest, TwoRegistrations)
{
    MySubject subject;

    MyObserver observer1(subject);
    MyObserver observer2(subject);
}

TEST(ObserverTest, NotifyOneObserver)
{
    MySubject subject;

    MyObserver observer(subject);

    EXPECT_CALL(observer, Callback());
    subject.NotifyObservers([](MyObserver& o) { o.Callback(); } );
}

TEST(ObserverTest, RegisterAfterConstruction)
{
    MySubject subject;

    MyObserver observer;
    observer.Attach(subject);
    EXPECT_TRUE(observer.Attached());

    EXPECT_CALL(observer, Callback());
    subject.NotifyObservers([](MyObserver& o) { o.Callback(); });
}

TEST(ObserverTest, Detach)
{
    MySubject subject;

    testing::StrictMock<MyObserver> observer(subject);
    observer.Detach();
    EXPECT_FALSE(observer.Attached());

    subject.NotifyObservers([](MyObserver& o) { o.Callback(); });
}

TEST(SingleObserverTest, ConstructWithoutRegistration)
{
    MySingleObserver observer;
    EXPECT_FALSE(observer.Attached());
}

TEST(SingleObserverTest, ConstructWithRegistration)
{
    MySingleSubject subject;

    MySingleObserver observer(subject);
    EXPECT_TRUE(observer.Attached());
    EXPECT_TRUE(subject.HasObserver());
}

TEST(SingleObserverTest, NotifyObserver)
{
    MySingleSubject subject;

    MySingleObserver observer(subject);

    EXPECT_CALL(observer, Callback());
    subject.GetObserver().Callback();
}

TEST(SingleObserverTest, RegisterAfterConstruction)
{
    MySingleSubject subject;

    MySingleObserver observer;
    observer.Attach(subject);
    EXPECT_TRUE(observer.Attached());
    EXPECT_TRUE(subject.HasObserver());

    EXPECT_CALL(observer, Callback());
    subject.GetObserver().Callback();
}

TEST(SingleObserverTest, Detach)
{
    MySingleSubject subject;

    testing::StrictMock<MySingleObserver> observer(subject);
    observer.Detach();
    EXPECT_FALSE(observer.Attached());
    EXPECT_FALSE(subject.HasObserver());
}