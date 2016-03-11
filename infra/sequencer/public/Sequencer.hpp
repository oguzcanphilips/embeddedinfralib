#ifndef INFRA_SEQUENCER_HPP
#define INFRA_SEQUENCER_HPP

#include "infra/util/public/Compare.hpp"
#include "infra/util/public/Function.hpp"
#include "infra/util/public/Optional.hpp"

#ifndef INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE
#define INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 8
#endif

namespace infra
{
    template<class Context>
    class SequenceObject;

    struct EmptyContext;

    class Sequencer
    {
    public:
        void Load(const infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE>& newSequence);
        SequenceObject<EmptyContext> Sequence();
        void Evaluate();
        void Continue();
        bool Finished() const;

    public:
        void Hold();
        bool Skip();
        void SkipLevel(uint8_t level);
        void RemoveSkip(uint8_t level);
        void Again(uint32_t step);

    public:
        uint32_t currentStep = 0;
        uint32_t lastStep = 0;
        Sequencer* parent = nullptr;

        static const std::size_t MaxContext = 5;
        std::array<uint32_t, MaxContext> context;

    private:
        infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> sequence;
        infra::Optional<uint8_t> skipLevel;
        bool again = false;
        bool inHold = false;
    };

    struct Hold {};
    struct HoldWhile
    {
        HoldWhile(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition);

        infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition;
    };

    struct If
    {
        If(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition);

        infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition;
    };

    struct Else {};

    struct ElseIf {
        ElseIf(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition);

        infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition;
    };

    struct EndIf {};

    struct While
    {
        While(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition);

        infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition;
    };

    struct EndWhile {};

    struct DoWhile {};

    struct EndDoWhile
    {
        EndDoWhile(infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition);

        infra::Function<bool(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> condition;
    };

    struct RepeatInfinitely {};
    struct EndRepeatInfinitely {};

    struct ForEach
    {
        ForEach(uint32_t begin, uint32_t end);

        uint32_t begin;
        uint32_t end;
    };

    struct EndForEach {};

    struct NestType
    {
        NestType(infra::Function<Sequencer&(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> function);

        infra::Function<Sequencer&(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> function;
    };

    template<class Function>
    NestType Nest(Function&& function, typename std::enable_if<infra::CanCall<Function()>::value>::type* = 0)
    {
        return NestType(std::forward<Function>(function));
    }

    struct NestTypeInForEach
    {
        NestTypeInForEach(infra::Function<Sequencer&(uint32_t), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> function);

        infra::Function<Sequencer&(uint32_t), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> function;
    };

    template<class Function>
    NestTypeInForEach Nest(Function&& function, typename std::enable_if<infra::CanCall<Function(uint32_t)>::value>::type* = 0)
    {
        return NestTypeInForEach(std::forward<Function>(function));
    }

    struct EmptyContext
    {
        static const uint32_t sequenceContextPointer = 0;
        static const uint8_t level = 0;
    };

    template<class Context>
    struct IfContext
        : public Context
    {
        IfContext(const Context& other);

        typedef Context BaseContext;
        static const uint32_t sequenceContextPointer = BaseContext::sequenceContextPointer + 1;
        static const uint8_t level = Context::level + 1;
    };

    template<class Context>
    struct WhileContext
        : public Context
    {
        WhileContext(const Context& other);

        typedef Context BaseContext;
        static const uint32_t sequenceContextPointer = BaseContext::sequenceContextPointer + 1;
        static const uint8_t level = Context::level + 1;

        uint32_t step;
    };

    template<class Context>
    struct DoWhileContext
        : public Context
    {
        DoWhileContext(const Context& other);

        typedef Context BaseContext;
        static const uint32_t sequenceContextPointer = BaseContext::sequenceContextPointer;
        static const uint8_t level = Context::level + 1;

        uint32_t step;
    };

    template<class Context>
    struct RepeatInfinitelyContext
        : public Context
    {
        RepeatInfinitelyContext(const Context& other);

        typedef Context BaseContext;
        static const uint32_t sequenceContextPointer = BaseContext::sequenceContextPointer;
        static const uint8_t level = Context::level + 1;

        uint32_t step;
    };

    template<class Context>
    struct ForEachContext
        : public Context
    {
        ForEachContext(const Context& other);

        typedef Context BaseContext;
        static const uint32_t sequenceContextPointer = BaseContext::sequenceContextPointer + 1;
        static const uint8_t level = Context::level + 1;

        uint32_t step;
    };

    template<class Context>
    class SequenceObjectBase
    {
    public:
        explicit SequenceObjectBase(Sequencer& sequencer);

        template<class OtherContext>
        SequenceObjectBase(const SequenceObjectBase<OtherContext>& other);

        SequenceObject<Context> operator,(HoldWhile holdWhile);
        SequenceObject<IfContext<Context>> operator,(If&& ifStatement);
        SequenceObject<WhileContext<Context>> operator,(While&& whileStatement);
        SequenceObject<DoWhileContext<Context>> operator,(DoWhile);
        SequenceObject<RepeatInfinitelyContext<Context>> operator,(RepeatInfinitely);
        SequenceObject<ForEachContext<Context>> operator,(ForEach&& forEachStatement);
        SequenceObject<Context> operator,(Hold);
        SequenceObject<Context> operator,(NestType nest);

    public:
        Sequencer& sequencer;
        uint32_t currentStep = 0;
        Context context;
    };

    template<class Context>
    class SequenceObjectBaseWithInvoke
        : public SequenceObjectBase<Context>
    {
    public:
        explicit SequenceObjectBaseWithInvoke(Sequencer& sequencer);

        template<class OtherContext>
        SequenceObjectBaseWithInvoke(const SequenceObjectBase<OtherContext>& other);

        using SequenceObjectBase<Context>::operator,;
        SequenceObject<Context> operator,(infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> invoke);
    };

    template<>
    class SequenceObject<EmptyContext>
        : public SequenceObjectBaseWithInvoke<EmptyContext>
    {
    public:
        explicit SequenceObject(Sequencer& sequencer);

        SequenceObject(const SequenceObjectBase<EmptyContext>& other);

        using SequenceObjectBaseWithInvoke<EmptyContext>::operator,;
    };

    template<class Context>
    class SequenceObject<IfContext<Context>>
        : public SequenceObjectBaseWithInvoke<IfContext<Context>>
    {
    public:
        SequenceObject(const SequenceObjectBase<IfContext<Context>>& other);

        using SequenceObjectBaseWithInvoke<IfContext<Context>>::operator,;
        SequenceObject<IfContext<Context>> operator,(ElseIf&& elseIfStatement);
        SequenceObject<IfContext<Context>> operator,(Else);
        SequenceObject<Context> operator,(EndIf);
    };

    template<class Context>
    class SequenceObject<WhileContext<Context>>
        : public SequenceObjectBaseWithInvoke<WhileContext<Context>>
    {
    public:
        SequenceObject(const SequenceObjectBase<WhileContext<Context>>& other);

        using SequenceObjectBaseWithInvoke<WhileContext<Context>>::operator,;
        SequenceObject<Context> operator,(EndWhile);
    };

    template<class Context>
    class SequenceObject<DoWhileContext<Context>>
        : public SequenceObjectBaseWithInvoke<DoWhileContext<Context>>
    {
    public:
        SequenceObject(const SequenceObjectBase<DoWhileContext<Context>>& other);

        using SequenceObjectBaseWithInvoke<DoWhileContext<Context>>::operator,;
        SequenceObject<Context> operator,(EndDoWhile&& endDoWhileStatement);
    };

    template<class Context>
    class SequenceObject<RepeatInfinitelyContext<Context>>
        : public SequenceObjectBaseWithInvoke<RepeatInfinitelyContext<Context>>
    {
    public:
        SequenceObject(const SequenceObjectBase<RepeatInfinitelyContext<Context>>& other);

        using SequenceObjectBaseWithInvoke<RepeatInfinitelyContext<Context>>::operator,;
        SequenceObject<Context> operator,(EndRepeatInfinitely);
    };

    template<class Context>
    class SequenceObject<ForEachContext<Context>>
        : public SequenceObjectBase<ForEachContext<Context>>
    {
    public:
        SequenceObject(const SequenceObjectBase<ForEachContext<Context>>& other);

        using SequenceObjectBase<ForEachContext<Context>>::operator,;
        SequenceObject<ForEachContext<Context>> operator,(infra::Function<void(uint32_t), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> invoke);
        SequenceObject<ForEachContext<Context>> operator,(NestTypeInForEach nest);
        SequenceObject<Context> operator,(EndForEach);
    };

    ////    Implementation    ////

    template<class Context>
    IfContext<Context>::IfContext(const Context& other)
        : Context(other)
    {}

    template<class Context>
    WhileContext<Context>::WhileContext(const Context& other)
        : Context(other)
    {}

    template<class Context>
    DoWhileContext<Context>::DoWhileContext(const Context& other)
        : Context(other)
    {}

    template<class Context>
    RepeatInfinitelyContext<Context>::RepeatInfinitelyContext(const Context& other)
        : Context(other)
    {}

    template<class Context>
    ForEachContext<Context>::ForEachContext(const Context& other)
        : Context(other)
    {}

    template<class Context>
    SequenceObjectBase<Context>::SequenceObjectBase(Sequencer& sequencer)
        : sequencer(sequencer)
    {
        static_assert(Context::sequenceContextPointer <= Sequencer::MaxContext, "Not enough context space available in sequencer");
    }

    template<class Context>
    template<class OtherContext>
    SequenceObjectBase<Context>::SequenceObjectBase(const SequenceObjectBase<OtherContext>& other)
        : sequencer(other.sequencer)
        , currentStep(other.currentStep)
        , context(other.context)
    {
        static_assert(Context::sequenceContextPointer <= Sequencer::MaxContext, "Not enough context space available in sequencer");
    }

    template<class Context>
    SequenceObject<Context> SequenceObjectBase<Context>::operator,(HoldWhile holdWhile)
    {
        if (sequencer.currentStep == currentStep)
        {
            if (!holdWhile.condition())
                ++sequencer.currentStep;
        }

        ++currentStep;
        sequencer.lastStep = currentStep;

        return *this;
    }

    template<class Context>
    SequenceObject<IfContext<Context>> SequenceObjectBase<Context>::operator,(If&& ifStatement)
    {
        if (sequencer.currentStep == currentStep)
        {
            ++sequencer.currentStep;

            if (!sequencer.Skip())
            {
                bool conditionResult = ifStatement.condition();
                sequencer.context[Context::sequenceContextPointer] = conditionResult;

                if (!conditionResult)
                    sequencer.SkipLevel(Context::level);
            }
        }

        ++currentStep;
        sequencer.lastStep = currentStep;

        return SequenceObject<IfContext<Context>>(*this);
    }

    template<class Context>
    SequenceObject<WhileContext<Context>> SequenceObjectBase<Context>::operator,(While&& whileStatement)
    {
        uint32_t startStep = currentStep;

        if (sequencer.currentStep == currentStep)
        {
            ++sequencer.currentStep;

            if (!sequencer.Skip())
            {
                bool conditionResult = whileStatement.condition();
                sequencer.context[Context::sequenceContextPointer] = conditionResult;

                if (!conditionResult)
                    sequencer.SkipLevel(Context::level);
            }
        }

        ++currentStep;
        sequencer.lastStep = currentStep;

        SequenceObject<WhileContext<Context>> result(*this);
        result.context.step = startStep;
        return result;
    }

    template<class Context>
    SequenceObject<DoWhileContext<Context>> SequenceObjectBase<Context>::operator,(DoWhile)
    {
        if (sequencer.currentStep == currentStep)
            ++sequencer.currentStep;

        ++currentStep;
        sequencer.lastStep = currentStep;

        SequenceObject<DoWhileContext<Context>> result(*this);
        result.context.step = currentStep;
        return result;
    }

    template<class Context>
    SequenceObject<RepeatInfinitelyContext<Context>> SequenceObjectBase<Context>::operator,(RepeatInfinitely)
    {
        if (sequencer.currentStep == currentStep)
            ++sequencer.currentStep;

        ++currentStep;
        sequencer.lastStep = currentStep;

        SequenceObject<RepeatInfinitelyContext<Context>> result(*this);
        result.context.step = currentStep;
        return result;
    }

    template<class Context>
    SequenceObject<ForEachContext<Context>> SequenceObjectBase<Context>::operator,(ForEach&& forEachStatement)
    {
        if (sequencer.currentStep == currentStep)
        {
            ++sequencer.currentStep;

            sequencer.context[Context::sequenceContextPointer] = forEachStatement.begin;
        }

        ++currentStep;

        uint32_t startStep = currentStep;

        if (sequencer.currentStep == currentStep)
        {
            ++sequencer.currentStep;

            uint32_t current = sequencer.context[Context::sequenceContextPointer];
            if (current == forEachStatement.end)
                sequencer.SkipLevel(Context::level);
        }

        ++currentStep;
        sequencer.lastStep = currentStep;

        SequenceObject<ForEachContext<Context>> result(*this);
        result.context.step = startStep;
        return result;
    }

    template<class Context>
    SequenceObject<Context> SequenceObjectBase<Context>::operator,(Hold)
    {
        if (sequencer.currentStep == currentStep)
        {
            ++sequencer.currentStep;

            if (!sequencer.Skip())
                sequencer.Hold();
            else
                ++sequencer.currentStep;
        }

        ++currentStep;
        ++currentStep;
        sequencer.lastStep = currentStep;

        return *this;
    }

    template<class Context>
    SequenceObject<Context> SequenceObjectBase<Context>::operator,(NestType nest)
    {
        Sequencer* nested = nullptr;
        if (sequencer.currentStep == currentStep)
        {
            if (!sequencer.Skip())
            {
                nested = &nest.function();
                nested->parent = &sequencer;
            }

            ++sequencer.currentStep;
        }

        ++currentStep;

        if (sequencer.currentStep == currentStep)
        {
            if (!sequencer.Skip())
            {
                assert(nested);
                nested->Evaluate();
                if (nested->Finished())
                {
                    nested->parent = nullptr;
                    ++sequencer.currentStep;
                }
                else
                    sequencer.Hold();
            }
            else
                ++sequencer.currentStep;
        }

        ++currentStep;
        sequencer.lastStep = currentStep;

        return *this;
    }

    template<class Context>
    SequenceObjectBaseWithInvoke<Context>::SequenceObjectBaseWithInvoke(Sequencer& sequencer)
        : SequenceObjectBase<Context>(sequencer)
    {}

    template<class Context>
    template<class OtherContext>
    SequenceObjectBaseWithInvoke<Context>::SequenceObjectBaseWithInvoke(const SequenceObjectBase<OtherContext>& other)
        : SequenceObjectBase<Context>(other)
    {}

    template<class Context>
    SequenceObject<Context> SequenceObjectBaseWithInvoke<Context>::operator,(infra::Function<void(), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> invoke)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
                invoke();
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return *this;
    }

    inline SequenceObject<EmptyContext>::SequenceObject(Sequencer& sequencer)
        : SequenceObjectBaseWithInvoke<EmptyContext>(sequencer)
    {}

    inline SequenceObject<EmptyContext>::SequenceObject(const SequenceObjectBase<EmptyContext>& other)
        : SequenceObjectBaseWithInvoke<EmptyContext>(other)
    {}

    template<class Context>
    SequenceObject<IfContext<Context>>::SequenceObject(const SequenceObjectBase<IfContext<Context>>& other)
        : SequenceObjectBaseWithInvoke<IfContext<Context>>(other)
    {}

    template<class Context>
    SequenceObject<IfContext<Context>> SequenceObject<IfContext<Context>>::operator,(ElseIf&& elseIfStatement)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            this->sequencer.RemoveSkip(Context::level);
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
            {
                bool conditionResult = this->sequencer.context[Context::sequenceContextPointer] != 0;
                if (conditionResult)
                    this->sequencer.SkipLevel(Context::level);
                else
                {
                    conditionResult = elseIfStatement.condition();
                    this->sequencer.context[Context::sequenceContextPointer] = conditionResult;
                    if (!conditionResult)
                        this->sequencer.SkipLevel(Context::level);
                }
            }
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return *this;
    }

    template<class Context>
    SequenceObject<IfContext<Context>> SequenceObject<IfContext<Context>>::operator,(Else)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            this->sequencer.RemoveSkip(Context::level);
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
            {
                bool conditionResult = this->sequencer.context[Context::sequenceContextPointer] != 0;
                if (conditionResult)
                    this->sequencer.SkipLevel(Context::level);
            }
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return *this;
    }

    template<class Context>
    SequenceObject<Context> SequenceObject<IfContext<Context>>::operator,(EndIf)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            this->sequencer.RemoveSkip(Context::level);
            ++this->sequencer.currentStep;
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return SequenceObject<Context>(*this);
    }

    template<class Context>
    SequenceObject<WhileContext<Context>>::SequenceObject(const SequenceObjectBase<WhileContext<Context>>& other)
        : SequenceObjectBaseWithInvoke<WhileContext<Context>>(other)
    {}

    template<class Context>
    SequenceObject<Context> SequenceObject<WhileContext<Context>>::operator,(EndWhile)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            this->sequencer.RemoveSkip(Context::level);
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
            {
                bool conditionResult = this->sequencer.context[Context::sequenceContextPointer] != 0;
                if (conditionResult)
                    this->sequencer.Again(this->context.step);
            }
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return SequenceObject<Context>(*this);
    }

    template<class Context>
    SequenceObject<DoWhileContext<Context>>::SequenceObject(const SequenceObjectBase<DoWhileContext<Context>>& other)
        : SequenceObjectBaseWithInvoke<DoWhileContext<Context>>(other)
    {}

    template<class Context>
    SequenceObject<Context> SequenceObject<DoWhileContext<Context>>::operator,(EndDoWhile&& endDoWhileStatement)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            this->sequencer.RemoveSkip(Context::level);
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
            {
                if (endDoWhileStatement.condition())
                    this->sequencer.Again(this->context.step);
            }
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return SequenceObject<Context>(*this);
    }

    template<class Context>
    SequenceObject<RepeatInfinitelyContext<Context>>::SequenceObject(const SequenceObjectBase<RepeatInfinitelyContext<Context>>& other)
        : SequenceObjectBaseWithInvoke<RepeatInfinitelyContext<Context>>(other)
    {}

    template<class Context>
    SequenceObject<Context> SequenceObject<RepeatInfinitelyContext<Context>>::operator,(EndRepeatInfinitely)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            this->sequencer.RemoveSkip(Context::level);
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
                this->sequencer.Again(this->context.step);
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return SequenceObject<Context>(*this);
    }

    template<class Context>
    SequenceObject<ForEachContext<Context>>::SequenceObject(const SequenceObjectBase<ForEachContext<Context>>& other)
        : SequenceObjectBase<ForEachContext<Context>>(other)
    {}

    template<class Context>
    SequenceObject<ForEachContext<Context>> SequenceObject<ForEachContext<Context>>::operator,(infra::Function<void(uint32_t), INFRA_SEQUENCER_FUNCTION_EXTRA_SIZE> invoke)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            if (!this->sequencer.Skip())
                invoke(this->sequencer.context[Context::sequenceContextPointer]);

            ++this->sequencer.currentStep;
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return *this;
    }

    template<class Context>
    SequenceObject<ForEachContext<Context>> SequenceObject<ForEachContext<Context>>::operator,(NestTypeInForEach nest)
    {
        Sequencer* nested = nullptr;
        if (this->sequencer.currentStep == this->currentStep)
        {
            if (!this->sequencer.Skip())
            {
                nested = &nest.function(this->sequencer.context[Context::sequenceContextPointer]);
                nested->parent = &this->sequencer;
            }

            ++this->sequencer.currentStep;
        }

        ++this->currentStep;

        if (this->sequencer.currentStep == this->currentStep)
        {
            if (!this->sequencer.Skip())
            {
                assert(nested);
                nested->Evaluate();
                if (nested->Finished())
                {
                    nested->parent = nullptr;
                    ++this->sequencer.currentStep;
                }
                else
                    this->sequencer.Hold();
            }
            else
                ++this->sequencer.currentStep;
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return *this;
    }

    template<class Context>
    SequenceObject<Context> SequenceObject<ForEachContext<Context>>::operator,(EndForEach)
    {
        if (this->sequencer.currentStep == this->currentStep)
        {
            ++this->sequencer.currentStep;

            if (!this->sequencer.Skip())
            {
                ++this->sequencer.context[Context::sequenceContextPointer];

                this->sequencer.Again(this->context.step);
            }

            this->sequencer.RemoveSkip(Context::level);
        }

        ++this->currentStep;
        this->sequencer.lastStep = this->currentStep;

        return SequenceObject<Context>(*this);
    }
}

#endif
