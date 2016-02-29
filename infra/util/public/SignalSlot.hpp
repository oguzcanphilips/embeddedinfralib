#ifndef SIGNALSLOT_HPP
#define SIGNALSLOT_HPP

#if __cplusplus >= 201103L || defined(WIN32) 
    #ifndef COMPILERCPP11 
        #include <utility>
        #define COMPILERCPP11 1
    #endif
#endif

namespace infra
{
    #if COMPILERCPP11 == 1

    template<class T, class ...A>
    class Slot;
    template<class... A>
    class SignalBase;
    template<class... A>
    class SlotBase
    {
        friend class SignalBase<A...>;
        template<class T, class ...AR> friend class Slot;

        SlotBase* next = nullptr;
        SignalBase<A...>* parent = nullptr;
    protected:
        ~SlotBase()
        {
            if (parent)
                *parent -= *this;
        }
        virtual void Notify(A... a) = 0;
    private:
        SlotBase(){}
        SlotBase(const SlotBase&) = delete;
        const SlotBase& operator=(const SlotBase&) = delete;
    };

    template<class ...A>
    class SignalBase
    {
        SlotBase<A...>* slots = nullptr;
        SlotBase<A...>* slotsIt = nullptr;
    public:
        ~SignalBase()
        {
            while (slots)
                operator-=(*slots);
        }

        void operator+=(SlotBase<A...>& slot)
        {
            if (slot.parent) (*slot.parent) -= slot;
            slot.parent = this;
            slot.next = slots;
            slots = &slot;
        }

        void operator-=(SlotBase<A...>& slot)
        {
            if (slots == nullptr)
                return;

            if (slotsIt == &slot)
                slotsIt = slotsIt->next;

            if (&slot == slots)
            {
                slot.parent = nullptr;
                slots = slots->next;
            }
            else
            {
                for (SlotBase<A...>* it = slots; it->next; it = it->next)
                {
                    if (it->next == &slot)
                    {
                        slot.parent = nullptr;
                        it->next = slot.next;
                        return;
                    }
                }
            }
        }
    protected:
        void operator()(A... a)
        {
            for (slotsIt = slots; slotsIt;)
            {
                SlotBase<A...>* tmp = slotsIt;
                slotsIt = slotsIt->next;
                tmp->Notify(std::forward<A>(a)...);
            }
        }
    };

    template<class OWNER, class ...A>
    class Signal : public SignalBase<A...>
    {
        friend OWNER;
    };

    template<class T, class ...A>
    class Slot final : public SlotBase<A...>
    {
        T* obj = nullptr;
        void(T::*func)(A...);

    protected:
        void Notify(A... a) override
        {
            (obj->*func)(std::forward<A>(a)...);
        }

    public:
        Slot(T* obj, void(T::*func)(A...)) : obj(obj), func(func) {}
    };

#else
    namespace SignalSlot
    {
        template <typename T1, typename T2>
        class SameTypes
        {
        public: 
            const static bool result = false;
        };

        template <typename T>
        class SameTypes<T, T>
        {
        public:
            const static bool result = true;
        };

        template <typename T1, typename T2>
        class NotSameTypes
        {
        public:
            const static bool result = true;
        };

        template <typename T>
        class NotSameTypes<T, T>
        {
        public:
            const static bool result = false;
        };

        struct NotUsed
        {
        };
    }

    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    class SignalBase;
    template<class A1 = SignalSlot::NotUsed, class A2 = SignalSlot::NotUsed, class A3 = SignalSlot::NotUsed, class A4 = SignalSlot::NotUsed, class A5 = SignalSlot::NotUsed, class A6 = SignalSlot::NotUsed, class A7 = SignalSlot::NotUsed, class A8 = SignalSlot::NotUsed>
    class SlotBase
    {
        friend class SignalBase<A1, A2, A3, A4, A5, A6, A7, A8>;
        SlotBase* next;
        SignalBase<A1, A2, A3, A4, A5, A6, A7, A8>* parent;
        virtual void Notify(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) = 0;
    protected:
        SlotBase()
            :next(0)
            ,parent(0)
        {}

        ~SlotBase()
        {
            if (parent) *parent -= *this;
        }
    };

    template<class A1 = SignalSlot::NotUsed, class A2 = SignalSlot::NotUsed, class A3 = SignalSlot::NotUsed, class A4 = SignalSlot::NotUsed, class A5 = SignalSlot::NotUsed, class A6 = SignalSlot::NotUsed, class A7 = SignalSlot::NotUsed, class A8 = SignalSlot::NotUsed>
    class SignalBase
    {
        SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>* slots;
        SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>* slotsIt;
    public:
        SignalBase()
            :slots(0)
            ,slotsIt(0)    
        {}
        ~SignalBase()
        {
            while (slots)
                operator-=(*slots);
        }

        void operator+=(SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>& slot)
        {
            if (slot.parent) (*slot.parent) -= slot;
            slot.parent = this;
            slot.next = slots;
            slots = &slot;
        }

        void operator-=(SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>& slot)
        {
            if (slots == 0) return;
            if (slotsIt && slotsIt == &slot) slotsIt = slotsIt->next;
            if (&slot == slots) { slot.parent = 0; slots = slots->next; return; }
            SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>* it = slots;
            while (it->next) if (it->next == &slot) { slot.parent = 0; it->next = slot.next; return; }
            else it = it->next;
        }

    protected:
        void operator()(A1 a1 = SignalSlot::NotUsed(), A2 a2 = SignalSlot::NotUsed(), A3 a3 = SignalSlot::NotUsed(), A4 a4 = SignalSlot::NotUsed(), A5 a5 = SignalSlot::NotUsed(), A6 a6 = SignalSlot::NotUsed(), A7 a7 = SignalSlot::NotUsed(), A8 a8 = SignalSlot::NotUsed())
        {
            for (slotsIt = slots; slotsIt;)
            {
                SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>* tmp = slotsIt;
                slotsIt = slotsIt->next;
                tmp->Notify(a1,a2,a3,a4,a5,a6,a7,a8);
            }
        }
    };

    template<class OWNER, class A1 = SignalSlot::NotUsed, class A2 = SignalSlot::NotUsed, class A3 = SignalSlot::NotUsed, class A4 = SignalSlot::NotUsed, class A5 = SignalSlot::NotUsed, class A6 = SignalSlot::NotUsed, class A7 = SignalSlot::NotUsed, class A8 = SignalSlot::NotUsed>
    class Signal : public SignalBase<A1, A2, A3, A4, A5, A6, A7, A8>
    {
        friend OWNER;
    };

    template<class T, class A1 = SignalSlot::NotUsed, class A2 = SignalSlot::NotUsed, class A3 = SignalSlot::NotUsed, class A4 = SignalSlot::NotUsed, class A5 = SignalSlot::NotUsed, class A6 = SignalSlot::NotUsed, class A7 = SignalSlot::NotUsed, class A8 = SignalSlot::NotUsed>
    class Slot : public SlotBase<A1, A2, A3, A4, A5, A6, A7, A8>
    {
        typedef void(T::*Func0)();
        typedef void(T::*Func1)(A1);
        typedef void(T::*Func2)(A1, A2);
        typedef void(T::*Func3)(A1, A2, A3);
        typedef void(T::*Func4)(A1, A2, A3, A4);
        typedef void(T::*Func5)(A1, A2, A3, A4, A5);
        typedef void(T::*Func6)(A1, A2, A3, A4, A5, A6);
        typedef void(T::*Func7)(A1, A2, A3, A4, A5, A6, A7);
        typedef void(T::*Func8)(A1, A2, A3, A4, A5, A6, A7, A8);
        T* obj;
        Func0 func;

        void Notify(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        {
            if (SignalSlot::SameTypes<SignalSlot::NotUsed, A1>::result) (obj->*(Func0)func)();
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A2>::result) (obj->*(Func1)func)(a1);
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A3>::result) (obj->*(Func2)func)(a1, a2);
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A4>::result) (obj->*(Func3)func)(a1, a2, a3);
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A5>::result) (obj->*(Func4)func)(a1, a2, a3, a4);
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A6>::result) (obj->*(Func5)func)(a1, a2, a3, a4, a5);
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A7>::result) (obj->*(Func6)func)(a1, a2, a3, a4, a5, a6);
            else if (SignalSlot::SameTypes<SignalSlot::NotUsed, A8>::result) (obj->*(Func7)func)(a1, a2, a3, a4, a5, a6, a7);
            else (obj->*(Func8)func)(a1, a2, a3, a4, a5, a6, a7, a8);
        }

        template<bool check, class TT = void>
        struct EnableIf
        {
            typedef SignalSlot::NotUsed Type;
        };

        template<class TT>
        struct EnableIf<true, TT>
        {
            typedef TT Type;
        };

    public:
        Slot(typename EnableIf<SignalSlot::SameTypes<SignalSlot::NotUsed, A1>::result, T>::Type* obj, void(T::*func)()) : obj(obj), func(func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A1>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A2>::result, T>::Type* obj, void(T::*func)(A1)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A2>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A3>::result, T>::Type* obj, void(T::*func)(A1, A2)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A3>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A4>::result, T>::Type* obj, void(T::*func)(A1, A2, A3)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A4>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A5>::result, T>::Type* obj, void(T::*func)(A1, A2, A3, A4)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A5>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A6>::result, T>::Type* obj, void(T::*func)(A1, A2, A3, A4, A5)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A6>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A7>::result, T>::Type* obj, void(T::*func)(A1, A2, A3, A4, A5, A6)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A7>::result && SignalSlot::SameTypes<SignalSlot::NotUsed, A8>::result, T>::Type* obj, void(T::*func)(A1, A2, A3, A4, A5, A6, A7)) : obj(obj), func((Func0)func){}
        Slot(typename EnableIf<SignalSlot::NotSameTypes<SignalSlot::NotUsed, A8>::result, T>::Type* obj, void(T::*func)(A1, A2, A3, A4, A5, A6, A7, A8)) : obj(obj), func((Func0)func){}
    };
#endif
};
#endif
