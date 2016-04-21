#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/sequencer/public/Sequencer.hpp"
#include "infra/sequencer/public/SequencerIdiom.hpp"

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
    sequencer.Load([this]() { });
}

TEST_F(TestSequencer, ExecuteEmptySequence)
{
    sequencer.Load([this]() { });
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, SequenceInvoke)
{
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
            sequencer.Step([this]() { a(); });
    });
    EXPECT_TRUE(sequencer.Finished());
}


TEST_F(TestSequencer, SequenceInvokeDouble)
{
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());


    sequencer.Load([this]() {
        sequencer.Step([this]() { a();  b(); });
        sequencer.Step([this]() { c(); });
    });
    EXPECT_FALSE(sequencer.Finished());
}

TEST_F(TestSequencer, SequenceInvokeDoubleAndContinue)
{
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());
    EXPECT_CALL(*this, c());


    sequencer.Load([this]() {
        sequencer.Step([this]() { a();  b(); });
        sequencer.Step([this]() { c(); });
    });
    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfOnUnsuccessfulConditionDoesNotExecuteStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); }); 
            sequencer.Step([this]() { a(); });
        sequencer.EndIf();
        sequencer.Step([this]() { b(); });
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfOnUnsuccessfulConditionDoesNotExecuteStatements)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, c());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
            sequencer.Step([this]() { b(); });
        sequencer.EndIf();
        sequencer.Step([this]() { c(); });
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfOnSuccessfulConditionExecutesStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.EndIf();
        sequencer.Step([this]() { b(); });
    });

    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfOnSuccessfulConditionExecutesStatements)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());
    EXPECT_CALL(*this, b());
    EXPECT_CALL(*this, c());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
            sequencer.Step([this]() { b(); });
        sequencer.EndIf();
        sequencer.Step([this]() { c(); });
    });

    EXPECT_FALSE(sequencer.Finished());
    sequencer.Continue();
    EXPECT_FALSE(sequencer.Finished());
    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}


TEST_F(TestSequencer, NestedIfDoesNotReleaseIf)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.If([this](){return condition(); });
            sequencer.EndIf();
        sequencer.EndIf();
        sequencer.Step([this]() { b(); });
    });

    EXPECT_TRUE(sequencer.Finished());
}


TEST_F(TestSequencer, IfElseOnUnsuccessfulConditionExecutesElseStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(false));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); }); 
        sequencer.Else(); 
            sequencer.Step([this]() { b(); });
        sequencer.EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseOnSuccessfulConditionExecutesIfStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.Else();
            sequencer.Step([this]() { b(); });
        sequencer.EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseIfElseOnSuccessfulConditionExecutesIfStatement)
{
    EXPECT_CALL(*this, condition()).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, a());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.Else();
            sequencer.If([this] { return condition(); });
                sequencer.Step([this](){ b(); });
            sequencer.Else();
                sequencer.Step([this]() { c(); }); 
            sequencer.EndIf();
       sequencer.EndIf();
    });
    
    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseIfElseOnSecondConditionExecutesElseIfStatement)
{
    EXPECT_CALL(*this, condition()).Times(2).WillOnce(testing::Return(false)).WillOnce(testing::Return(true));
    EXPECT_CALL(*this, b());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.Else();
            sequencer.If([this] { return condition(); });
                sequencer.Step([this](){ b(); });
            sequencer.Else();
                sequencer.Step([this]() { c(); });
            sequencer.EndIf();
        sequencer.EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

TEST_F(TestSequencer, IfElseIfElseOnUnsuccessfulConditionExecutesElseStatement)
{
    EXPECT_CALL(*this, condition()).Times(2).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*this, c());

    sequencer.Load([this]() {
        sequencer.If([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.Else();
            sequencer.If([this] { return condition(); });
                sequencer.Step([this](){ b(); });
            sequencer.Else();
                sequencer.Step([this]() { c(); });
            sequencer.EndIf();
        sequencer.EndIf();
    });

    EXPECT_TRUE(sequencer.Finished());
}

/*

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

*/

TEST_F(TestSequencer, WhileWillNotExecuteWhenConditionIsFalse)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(false));

    sequencer.Load([this]() {
        sequencer.While([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.EndWhile();
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
        sequencer.While([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.EndWhile();
    });

    sequencer.Continue();
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
        sequencer.While([this] { return condition(); });
            sequencer.Step([this]() { a(); });
        sequencer.EndWhile();
    });

    sequencer.Continue();
    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}

/*
TEST_F(TestSequencer, WhileWillExecuteTwiceWhenConditionIsTrueTwice)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    EXPECT_CALL(*this, a()).Times(2);

    sequencer.Load([this]() {
        sequencer.While([this] { return condition(); }, [this]
        {
            sequencer.Step([this]() { a(); });
            sequencer.Step([this]() { b(); });
        });
    });

    sequencer.Continue();
    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());
}
*/

/*
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



*/

TEST_F(TestSequencer, Idiom)
{
    EXPECT_CALL(*this, condition())
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false));
    EXPECT_CALL(*this, a()).Times(1);
   
    SEQ_START();
        SEQ_WHILE(condition(););
            SEQ_IF(condition());
                SEQ_STEP(a();); 
            SEQ_ELSE(); 
                SEQ_IF(condition(););
                    SEQ_STEP(b(););
                SEQ_ELSE();
                    SEQ_STEP(c(););
                SEQ_ENDIF();
            SEQ_ENDIF();
        SEQ_ENDWHILE();
    SEQ_END();

    sequencer.Continue();
    sequencer.Continue();
    EXPECT_TRUE(sequencer.Finished());

}