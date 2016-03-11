#include "infra/sequencer/public/Sequencer.hpp"

namespace infra
{
    void Sequencer::Load(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& newSequence)
    {
        sequence = newSequence;
        currentStep = 0;
        Evaluate();
    }

    SequenceObject<EmptyContext> Sequencer::Sequence()
    {
        return SequenceObject<EmptyContext>(*this);
    }

    void Sequencer::Evaluate()
    {
        again = true;

        while (again)
        {
            again = false;
            sequence();
        }

        /*if (Finished() && parent)
        infra::EventDispatcher::Instance().Schedule([this]()
        {
        if (parent)
        parent->Evaluate();
        });*/
    }

    void Sequencer::Continue()
    {
        if (inHold)
        {
            inHold = false;
            ++currentStep;
            Evaluate();
        }
        if (Finished() && parent)
            parent->Continue();
    }

    bool Sequencer::Finished() const
    {
        return currentStep == lastStep;
    }

    void Sequencer::Hold()
    {
        inHold = true;
    }

    bool Sequencer::Skip()
    {
        return skipLevel != infra::none;
    }

    void Sequencer::SkipLevel(uint8_t level)
    {
        assert(!skipLevel);
        skipLevel = level;
    }

    void Sequencer::RemoveSkip(uint8_t level)
    {
        if (skipLevel == level)
            skipLevel = infra::none;
    }

    void Sequencer::Again(uint32_t step)
    {
        currentStep = step;
        again = true;
    }

    HoldWhile::HoldWhile(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition)
        : condition(condition)
    {}

    If::If(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition)
        : condition(condition)
    {}

    ElseIf::ElseIf(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition)
        : condition(condition)
    {}

    While::While(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition)
        : condition(condition)
    {}

    EndDoWhile::EndDoWhile(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition)
        : condition(condition)
    {}

    ForEach::ForEach(uint32_t begin, uint32_t end)
        : begin(begin)
        , end(end)
    {}

    NestType::NestType(infra::Function<Sequencer&(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> function)
        : function(function)
    {}

    NestTypeInForEach::NestTypeInForEach(infra::Function<Sequencer&(uint32_t), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> function)
        : function(function)
    {}
}
