#ifndef INFRA_SEQUENCER_HPP
#define INFRA_SEQUENCER_HPP

#include "infra/util/public/Compare.hpp"
#include "infra/util/public/Function.hpp"
#include "infra/util/public/Optional.hpp"
#include <iostream>

#ifndef INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE
#define INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 8
#endif

namespace infra
{
    class Sequencer
    {
    public:
        void Load(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& newSequence);
        
        void Step(const infra::Function < void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& action);
        void While(const infra::Function < bool()>& condition);
        void EndWhile();
        void If(const infra::Function < bool()>& condition);
        void Else();
        void EndIf();
        
        void Continue();
        bool Finished() const;

    private:
        infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> sequence;
        struct Context
        {
            uint8_t currentStep;
            uint8_t nextStepToExecute;
            int8_t stepIncrement;
            bool execute = true;
        };
        static const std::size_t MaxContext = 5;
        std::array<Context, MaxContext> context;
        int8_t currentLevel = -1;       
        
        bool ExecuteCurrentStep();
        void RepeatPreviousStepOnContinue();
        void SkipNextStepOnContinue();
        void ExecuteNextStep();
        void PushContext();
        void PopContext();
        void SkipNextLevels();
        void ExecuteNextLevel();
        bool Finished(uint32_t level) const;
        bool CurrentLevelFinished() const;
};    
}

#endif
