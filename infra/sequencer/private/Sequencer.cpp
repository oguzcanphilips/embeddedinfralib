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
    }

    void Sequencer::Continue()
    {
        if (inHold)
        {
            inHold = false;
            ++currentStep;
            Evaluate();
        }
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
}
