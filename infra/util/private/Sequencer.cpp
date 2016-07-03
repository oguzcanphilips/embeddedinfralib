#include "infra/util/public/Sequencer.hpp"

namespace infra
{
    void Sequencer::Load(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& newSequence)
    {
        sequence = newSequence;
        execute.clear();
        Continue();
    }

    void Sequencer::Continue()
    {
        examine.clear();

        repeat = true;
        while (repeat)
        {
            repeat = false;

            PushContext();
            sequence();
            PopContext();
        }

        if (!Finished())
            ExecuteNextStep();
    }

    void Sequencer::Step(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& action)
    {
        bool executeStep = ExecuteCurrentStep();

        IncreaseCurrentStep();

        if (executeStep)
            action();
    }

    void Sequencer::Execute(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& action)
    {
        bool executeStep = ExecuteCurrentStep();

        IncreaseCurrentStep();

        if (executeStep)
        {
            ExecuteNextStep();
            action();
        }
    }

    void Sequencer::If(const infra::Function<bool()>& condition)
    {
        Execute([this, condition]()
        {
            if (!condition())
            {
                ExecuteNextStep();
                ExecuteNextStep();  // Immediately continue with the step after EndIf, Else, or EndWhile
            }
        });

        PushContext();
    }

    void Sequencer::ElseIf(const infra::Function<bool()>& condition)
    {
        PopContext();

        Execute([this]()
        {
            ExecuteNextStep();
            ExecuteNextStep();
        });

        Execute([this, condition]()
        {
            if (!condition())
            {
                ExecuteNextStep();
                ExecuteNextStep();  // Immediately continue with the step after EndIf, Else, or EndWhile
            }
        });

        PushContext();
    }

    void Sequencer::EndIf()
    {
        PopContext();

        Execute([]() {});
    }

    void Sequencer::Else()
    {
        PopContext();

        Execute([this]() { ExecuteNextStep(); });

        PushContext();
    }

    void Sequencer::While(const infra::Function<bool()>& condition)
    {
        If(condition);
    }

    void Sequencer::EndWhile()
    {
        PopContext();

        Execute([this]()
        {
            ExecutePreviousStep();
            ExecutePreviousStep();
            ExecutePreviousStep();
        });
    }

    void Sequencer::DoWhile()
    {
        Execute(infra::emptyFunction);

        PushContext();
    }

    void Sequencer::EndDoWhile(const infra::Function<bool()>& condition)
    {
        PopContext();

        Execute([this, condition]()
        {
            if (condition())
            {
                ExecutePreviousStep();
                ExecutePreviousStep();
                ExecutePreviousStep();
            }
        });
    }

    void Sequencer::ForEach(uint32_t& variable_, uint32_t from_, uint32_t to_)
    {
        struct State
        {
            State(uint32_t& variable, uint32_t from, uint32_t to)
                : variable(variable), from(from), to(to)
            {}

            uint32_t& variable;
            uint32_t from;
            uint32_t to;
        };

        State state(variable_, from_, to_);

        Execute([this, &state]()
        {
            state.variable = state.from;
        });

        Execute([this, &state]()
        {
            if (state.variable == state.to)
            {
                ExecuteNextStep();
                ExecuteNextStep();  // Immediately continue with the step after EndForEach
            }
        });

        PushContext();
    }

    void Sequencer::EndForEach(uint32_t& variable)
    {
        Execute([this, &variable]() { ++variable; });

        PopContext();

        Execute([this]()
        {
            ExecutePreviousStep();
            ExecutePreviousStep();
            ExecutePreviousStep();
        });
    }

    void Sequencer::IncreaseCurrentStep()
    {
        ++examine.back();
    }

    bool Sequencer::ExecuteCurrentStep() const
    {
        return execute == examine;
    }

    void Sequencer::ExecuteNextStep()
    {
        ++execute.back();
    }

    void Sequencer::ExecutePreviousStep()
    {
        --execute.back();
        repeat = true;
    }

    void Sequencer::PushContext()
    {
        if (ExecuteCurrentStep())
            execute.push_back(0);

        examine.push_back(0);
    }

    void Sequencer::PopContext()
    {
        if (ExecuteCurrentStep())
        {
            execute.pop_back();

            if (!Finished())
                ExecuteNextStep();
        }
        
        examine.pop_back();
        if (!examine.empty())
            IncreaseCurrentStep();
    }

    bool Sequencer::Finished() const
    {
        return execute.empty();
    }   
}
