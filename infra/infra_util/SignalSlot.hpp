#ifndef INFRA_SIGNAL_SLOT_HPP
#define INFRA_SIGNAL_SLOT_HPP

//  Signal and Slot are two classes that cooperate in order to make a caller make many function calls
//  in succession to subscribers. Each Slot is initialized with a Function, after which a Slot is
//  attached to a Signal. Multiple Slots may be attached to a single Signal. When the Signal is invoked,
//  the Function of each attached Slot is executed. For example:
//  
//  Slot<void()> a = []() { hal::Gpio::Pin<Led1>().SetOn(); };
//  Slot<void()> b = []() { hal::Gpio::Pin<Led2>().SetOff(); };
//  Signal<C, void()> s;
//  s += a;
//  s += b;
//  s();    // Sets Led1 on and Led2 off

#include "infra_util/Function.hpp"
#include "infra_util/IntrusiveForwardList.hpp"
#include <memory>

namespace infra
{
    template<class F>
    class SignalBase;

    template<class Owner, class F>
    class Signal;

    template<class F, std::size_t ExtraSize = UTIL_DEFAULT_FUNCTION_EXTRA_SIZE>
    class Slot;

    template<class... Args, std::size_t ExtraSize>
    class Slot<void(Args...), ExtraSize>
        : public IntrusiveForwardList<Slot<void(Args...)>>::NodeType
    {
    public:
        typedef Function<void(Args...), ExtraSize> FunctionType;

        Slot(const FunctionType& f);
        ~Slot();

    private:
        friend class SignalBase<void(Args...)>;

        void Notify(Args&&... a);

    private:
        SignalBase<void(Args...)>* mParent;
        FunctionType mFunction;
    };

    template<class... Args>
    class SignalBase<void(Args...)>
    {
    protected:
        SignalBase() = default;
        ~SignalBase();

        void operator()(Args... a);

    public:
        void operator+=(Slot<void(Args...)>& slot);
        void operator-=(Slot<void(Args...)>& slot);

    private:
        friend class Slot<void(Args...)>;

        void Attach(Slot<void(Args...)>& slot);
        void Detach(Slot<void(Args...)>& slot);

    private:
        IntrusiveForwardList<Slot<void(Args...)>> mSlots;
    };

    template<class Owner, class... Args>
    class Signal<Owner, void(Args...)>
        : public SignalBase<void(Args...)>
    {
    public:
        friend Owner;
    };
    
    ////    Implementation    ////

    template<class... Args, std::size_t ExtraSize>
    Slot<void(Args...), ExtraSize>::Slot(const FunctionType& f)
        : mParent(nullptr)
        , mFunction(f)
    {}

    template<class... Args, std::size_t ExtraSize>
    Slot<void(Args...), ExtraSize>::~Slot()
    {
        if (mParent)
            mParent->Detach(*this);
    }

    template<class... Args, std::size_t ExtraSize>
    void Slot<void(Args...), ExtraSize>::Notify(Args&&... a)
    {
        mFunction(std::forward<Args>(a)...);
    }

    template<class... Args>
    SignalBase<void(Args...)>::~SignalBase()
    {
        while (!mSlots.empty())
            Detach(mSlots.front());
    }

    template<class... Args>
    void SignalBase<void(Args...)>::operator()(Args... a)
    {
        for (Slot<void(Args...)>& slot : mSlots)
            slot.Notify(std::forward<Args>(a)...);
    }

    template<class... Args>
    void SignalBase<void(Args...)>::operator+=(Slot<void(Args...)>& slot)
    {
        Attach(slot);
    }

    template<class... Args>
    void SignalBase<void(Args...)>::operator-=(Slot<void(Args...)>& slot)
    {
        Detach(slot);
    }

    template<class... Args>
    void SignalBase<void(Args...)>::Attach(Slot<void(Args...)>& slot)
    {
        if (slot.mParent)
            slot.mParent->Detach(slot);

        slot.mParent = this;
        mSlots.push_front(slot);
    }

    template<class... Args>
    void SignalBase<void(Args...)>::Detach(Slot<void(Args...)>& slot)
    {
        slot.mParent = nullptr;

        if (!mSlots.empty())
        {
            if (&slot == &mSlots.front())
                mSlots.pop_front();
            else
            {
                for (typename IntrusiveForwardList<Slot<void(Args...)>>::iterator previousSlot = mSlots.begin(); std::next(previousSlot) != mSlots.end(); ++previousSlot)
                {
                    if (&*std::next(previousSlot) == &slot)
                    {
                        mSlots.erase_after(*previousSlot);
                        break;
                    }
                }
            }
        }
    }
}

#endif
