#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/sequencer/public/Sequencer.hpp"

using namespace infra;

class TestSequencer
    : public testing::StrictMock<testing::Test>
{
public:
    MOCK_METHOD0(a, void());
    MOCK_METHOD0(b, void());
    MOCK_METHOD0(c, void());

    MOCK_METHOD1(p1, void(uint32_t));

    MOCK_CONST_METHOD0(condition, bool());

    Sequencer sequencer;
};

TEST_F(TestSequencer, LoadEmptySequence)
{
    sequencer.Load([this]() { sequencer.Sequence(); });
}

TEST_F(TestSequencer, ExecuteEmptySequence)
{
    sequencer.Load([this]() { sequencer.Sequence(); });
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, SequenceInvoke)
{
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            [this]() { a(); };
    });
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, SequenceInvokeDouble)
{
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            [this]() { a(); },
            [this]() { b(); };
    });
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, HoldHoldsExecution)
{
    sequencer.Load([this]() {
        sequencer.Sequence(),
            Hold();
    });
    sequencer.Evaluate();
    EXPECT_FALSE(sequencer.Finished());
}

TEST_F(TestSequencer, HoldHoldsExecutionWithActions)
{
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            [this]() { a(); },
            Hold(),
            [this]() { b(); };
    });
    sequencer.Evaluate();
    EXPECT_FALSE(sequencer.Finished());
}

TEST_F(TestSequencer, ReleaseContinuesHeldExecution)
{
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            [this]() { a(); },
            Hold(),
            [this]() { b(); };
    });

    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfOnUnsuccessfulConditionDoesNotExecuteStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            EndIf(),
            [this]() { b(); };
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfOnSuccessfulConditionExecutesStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            EndIf(),
            [this]() { b(); };
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, NestedIfDoesNotReleaseIf)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                If([this] { return condition(); }),
                EndIf(),
                [this]() { a(); },
            EndIf(),
            [this]() { b(); };
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseOnUnsuccessfulConditionExecutesElseStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            Else(),
                [this]() { b(); },
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseOnSuccessfulConditionExecutesIfStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            Else(),
                [this]() { b(); },
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseIfElseOnSuccessfulConditionExecutesIfStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            ElseIf([this] { return condition(); }),
                [this]() { b(); },
            Else(),
                [this]() { c(); },
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseIfElseOnSecondConditionExecutesElseIfStatement)
{
    EXPECT_CALL(*this, condition()).Times(2).WillOnce(testing::Return(false)).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            ElseIf([this] { return condition(); }),
                [this]() { b(); },
            Else(),
                [this]() { c(); },
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseIfElseOnUnsuccessfulConditionExecutesElseStatement)
{
    EXPECT_CALL(*this, condition()).Times(2).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*this, c());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                [this]() { a(); },
            ElseIf([this] { return condition(); }),
                [this]() { b(); },
            Else(),
                [this]() { c(); },
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, NestedIfElseDoesNotReleaseIf)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                If([this] { return condition(); }),
                    [this]() { a(); },
                Else(),
                    [this]() { a(); },
                EndIf(),
                [this]() { a(); },
            Else(),
                [this]() { b(); },
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfCanBeHeld)
{
    sequencer.Load([this]() {
        sequencer.Sequence(),
            Hold(),
            If([this] { return condition(); }),
            EndIf();
    });

    EXPECT_FALSE(sequencer.Finished());
}

TEST_F(TestSequencer, HoldIsSkippedInIf)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this] { return condition(); }),
                Hold(),
            EndIf(),
            [this]() { a(); };
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, WhileWillNotExecuteWhenConditionIsFalse)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            While([this] { return condition(); }),
                [this]() { a(); },
            EndWhile();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, WhileWillExecuteOnceWhenConditionIsTrueOnce)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            While([this] { return condition(); }),
                [this]() { a(); },
            EndWhile();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, WhileWillExecuteTwiceWhenConditionIsTrueTwice)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    EXPECT_CALL(*this, a()).Times(2);

    sequencer.Load([this]() {
        sequencer.Sequence(),
            While([this] { return condition(); }),
                [this]() { a(); },
            EndWhile();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, NestedWhileDoesNotReleaseWhile)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            While([this] { return condition(); }),
                While([this] { return condition(); }),
                    [this]() { a(); },
                EndWhile(),
                [this]() { a(); },
            EndWhile();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, DoWhileWillExecuteOnceWhenConditionIsFalse)
{
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            DoWhile(),
                [this]() { a(); },
            EndDoWhile([this] { return condition(); });
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, DoWhileWillExecuteTwiceWhenConditionIsTrueOnce)
{
    EXPECT_CALL(*this, a())
        .Times(2);
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            DoWhile(),
                [this]() { a(); },
            EndDoWhile([this] { return condition(); });
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, DoWhileWillCanBeSkipped)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this]() { return condition(); }),
                DoWhile(),
                    [this]() { a(); },
                EndDoWhile([this] { return condition(); }),
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}


TEST_F(TestSequencer, RepeatInfinitelyRepeats10Times)
{
    EXPECT_CALL(*this, a())
        .Times(10);

    sequencer.Load([this]() {
        sequencer.Sequence(),
            RepeatInfinitely(),
                Hold(), 
                [this]() { a(); },
            EndRepeatInfinitely();
    });

    for (int i = 0; i < 10; ++i)
        sequencer.Continue();
    EXPECT_FALSE(sequencer.Finished());
}

TEST_F(TestSequencer, RepeatInfinitelyCanBeSkipped)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this]() { return condition(); }),
                RepeatInfinitely(),
                    Hold(), 
                    [this]() { a(); },
                EndRepeatInfinitely(),
            EndIf();
    });

    for (int i = 0; i < 10; ++i)
        sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, ForEachDoesntIterate)
{
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            ForEach(0, 0),
                [this](uint32_t index) { p1(index); },
            EndForEach(),
            [this]() { a(); };
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, ForEachIteratesOnce)
{
    EXPECT_CALL(*this, p1(0))
        .Times(1);

    sequencer.Load([this]() {
        sequencer.Sequence(),
            ForEach(0, 1),
                [this](uint32_t index) { p1(index); },
            EndForEach();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, ForEachIteratesTwice)
{
    EXPECT_CALL(*this, p1(0));
    EXPECT_CALL(*this, p1(1));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            ForEach(0, 2),
                [this](uint32_t index) { p1(index); },
            EndForEach();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, ForEachCanBeSkipped)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.Sequence(),
            If([this]() { return condition(); }),
                ForEach(0, 1),
                    [this](uint32_t index) { p1(index); },
                EndForEach(),
            EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, ForEachCanBeHeld)
{
    sequencer.Load([this]() {
        sequencer.Sequence(),
            Hold(),
            ForEach(0, 1),
                [this](uint32_t index) { p1(index); },
            EndForEach();
    });

    EXPECT_FALSE(sequencer.Finished());
}

TEST_F(TestSequencer, LoadNewSequencer)
{
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            [this]() { a(); };
    });
    EXPECT_TRUE(sequencer.Finished());

    sequencer.Load([this]() {
        sequencer.Sequence(),
            [this]() { b(); };
    });
    EXPECT_TRUE(sequencer.Finished());
}

