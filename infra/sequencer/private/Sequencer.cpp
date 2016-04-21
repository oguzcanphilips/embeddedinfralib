#include "infra/sequencer/public/Sequencer.hpp"


namespace infra
{
    void Sequencer::Load(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& newSequence)
    {
        sequence = newSequence;
        currentLevel = -1;
        ExecuteNextLevel();
        Continue();
    }

    void Sequencer::Continue()
    {
        PushContext();
        sequence();
        PopContext();
    }

    void Sequencer::Step(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& action)
    {
        if (ExecuteCurrentStep())
            action();
    }

    void Sequencer::If(const infra::Function < bool()>& condition)
    {
        Step([this, condition]()
        {
            ExecuteNextStep();  //execute the end check of the EndIf
            if (condition())
            {
                ExecuteNextLevel();                
            }
            else
            {
                SkipNextLevels();
                ExecuteNextStep();  //Immediatley continue with the step after EndIf or Else
            }
        });

        PushContext();
        
    }

    void Sequencer::EndIf()
    {
        PopContext();
        Step([this]()
        {
            SkipNextStepOnContinue();
        });

        Step([this]{ExecuteNextStep(); });
    }

    void Sequencer::Else()
    {
        PopContext();
        Step([this]()
        {
           ExecuteNextStep();
           ExecuteNextStep();
           SkipNextLevels();
        });

        Step([this]{
            ExecuteNextStep(); 
            ExecuteNextLevel();
        });
        PushContext();
    }

    void Sequencer::While(const infra::Function<bool()>& condition)
    {
        If(condition);
    }

    void Sequencer::EndWhile()
    {
        PopContext();
        Step([this]()
        {
            RepeatPreviousStepOnContinue();
        });

        Step([this]{ExecuteNextStep(); });
    }

    bool Sequencer::ExecuteCurrentStep()
    {
        uint8_t currentStep = context[currentLevel].currentStep++;
        if (currentLevel != MaxContext -1 && !Finished(currentLevel + 1))
        {
            context[currentLevel].stepIncrement = 0;
            return false;
        }
        return currentStep == context[currentLevel].nextStepToExecute && context[currentLevel].execute;
    }

    void Sequencer::RepeatPreviousStepOnContinue()
    {
        context[currentLevel].stepIncrement = -1;
    }

    void Sequencer::SkipNextStepOnContinue()
    {
        context[currentLevel].stepIncrement = 2;
    }

    void Sequencer::ExecuteNextStep()
    {
        ++context[currentLevel].nextStepToExecute;
    }

    void Sequencer::PushContext()
    {
        ++currentLevel; 
        assert(currentLevel < MaxContext);
        context[currentLevel].currentStep = 0;
        context[currentLevel].stepIncrement = 1;//by default the next step should be executed on continue
    }

    void Sequencer::PopContext()
    {
        assert(currentLevel >= 0);
        if (!CurrentLevelFinished())
            context[currentLevel].nextStepToExecute += context[currentLevel].stepIncrement;
        
        --currentLevel;
    }

    bool Sequencer::Finished(uint32_t level) const
    {
        return context[level].nextStepToExecute >= context[level].currentStep || !context[level].execute;
    }

    bool Sequencer::CurrentLevelFinished() const
    {
        return Finished(currentLevel);
    }

    bool Sequencer::Finished() const
    {
        return Finished(0);
    }   

    void Sequencer::SkipNextLevels()
    {
        for (uint8_t i = currentLevel + 1; i < MaxContext; ++i)
            context[i].execute = false;
    }

    void Sequencer::ExecuteNextLevel()
    {
        context[currentLevel + 1].nextStepToExecute = 0;
        context[currentLevel + 1].execute = true;
    }


}
