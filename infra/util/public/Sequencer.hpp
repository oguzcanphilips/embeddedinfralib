#ifndef INFRA_SEQUENCER_HPP
#define INFRA_SEQUENCER_HPP

#include "infra/util/public/BoundedVector.hpp"
#include "infra/util/public/Function.hpp"

#ifndef INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE
#define INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 8
#endif

namespace infra
{
    class Sequencer
    {
    public:
        void Load(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& newSequence);
        
        void Step(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& action);
        void Execute(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& action);
        void If(const infra::Function<bool()>& condition);
        void ElseIf(const infra::Function<bool()>& condition);
        void Else();
        void EndIf();
        void While(const infra::Function<bool()>& condition);
        void EndWhile();
        void DoWhile();
        void EndDoWhile(const infra::Function<bool()>& condition);
        void ForEach(uint32_t& variable, uint32_t from, uint32_t to);
        void EndForEach(uint32_t& variable);

        void Continue();
        bool Finished() const;

    private:
        infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> sequence;

        infra::BoundedVector<uint8_t>::WithMaxSize<5> execute;
        infra::BoundedVector<uint8_t>::WithMaxSize<5> examine;
        bool repeat;

        void IncreaseCurrentStep();
        bool ExecuteCurrentStep() const;

        void ExecutePreviousStep();
        void ExecuteNextStep();

        void PushContext();
        void PopContext();
    };    
}

#endif
