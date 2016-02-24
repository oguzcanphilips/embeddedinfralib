#include "gtest/gtest.h"
#include "infra/util/public/Variant.hpp"

TEST(VariantTest, TestEmptyConstruction)
{
    infra::Variant<bool> v;
}

TEST(VariantTest, TestConstructionWithBool)
{
    bool b;
    infra::Variant<bool> v(infra::inPlace, b);
}

TEST(VariantTest, TestConstructionWithVariant)
{
    infra::Variant<bool, int> i(infra::inPlace, 5);
    infra::Variant<bool, int> v(i);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestConstructionWithNarrowVariant)
{
    infra::Variant<int> i(infra::inPlace, 5);
    infra::Variant<bool, int> v(i);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestConstructionAtIndex)
{
    infra::Variant<uint8_t, uint16_t> v(infra::atIndex, 1, 3);
    EXPECT_EQ(1, v.Which());
    EXPECT_EQ(3, v.Get<uint16_t>());
}

TEST(VariantTest, TestConstructAtIndex)
{
    infra::Variant<uint8_t, uint16_t> v;
    v.ConstructAtIndex(1, 3);
    EXPECT_EQ(1, v.Which());
    EXPECT_EQ(3, v.Get<uint16_t>());
}

TEST(VariantTest, TestGetBool)
{
    bool b = true;
    infra::Variant<bool> v(infra::inPlace, b);
    EXPECT_TRUE(v.Get<bool>());
}

TEST(VariantTest, TestAssignment)
{
    infra::Variant<bool, int> v(infra::inPlace, true);
    v = 5;
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestInPlaceConstruction)
{
    struct MyStruct
    {
        MyStruct(int aX, int aY): x(aX), y(aY) {};

        int x;
        int y;
    };

    infra::Variant<bool, MyStruct> v(infra::inPlace, true);
    v.Construct<MyStruct>(2, 3);
    EXPECT_EQ(1, v.Which());
    EXPECT_EQ(2, v.Get<MyStruct>().x);
    EXPECT_EQ(3, v.Get<MyStruct>().y);
}

TEST(VariantTest, TestAssignmentFromVariant)
{
    infra::Variant<bool, int> v(infra::inPlace, true);
    v = infra::Variant<bool, int>(infra::inPlace, 5);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestAssignmentFromNarrowVariant)
{
    infra::Variant<bool, int> v(infra::inPlace, true);
    v = infra::Variant<int>(infra::inPlace, 5);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestVariantWithTwoTypes)
{
    int i = 5;
    infra::Variant<bool, int> v(infra::inPlace, i);
    EXPECT_EQ(5, v.Get<int>());
}

TEST(VariantTest, TestVisitor)
{
    struct Visitor
        : infra::StaticVisitor<void>
    {
        Visitor()
            : passed(0)
        {}

        void operator()(bool b)
        {}

        void operator()(int i)
        {
            ++passed;
        }

        int passed;
    };

    int i = 5;
    infra::Variant<bool, int> variant(infra::inPlace, i);
    Visitor visitor;
    infra::ApplyVisitor(visitor, variant);
    EXPECT_EQ(1, visitor.passed);
}

TEST(VariantTest, TestReturningVisitor)
{
    struct Visitor
        : infra::StaticVisitor<bool>
    {
        bool operator()(bool b)
        {
            return false;
        }

        bool operator()(int i)
        {
            return true;
        }
    };

    int i = 5;
    infra::Variant<bool, int> variant(infra::inPlace, i);
    Visitor visitor;
    EXPECT_EQ(true, infra::ApplyVisitor(visitor, variant));
}

TEST(VariantTest, TestEqual)
{
    int i = 1;
    int j = 2;
    bool k = true;
    const infra::Variant<bool, int> v1(infra::inPlace, i);
    const infra::Variant<bool, int> v2(infra::inPlace, i);
    const infra::Variant<bool, int> v3(infra::inPlace, j);
    const infra::Variant<bool, int> v4(infra::inPlace, k);

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
    EXPECT_NE(v1, v4);
}

TEST(VariantTest, TestLessThan)
{
    int i = 1;
    int j = 2;
    bool k = true;
    const infra::Variant<bool, int> v1(infra::inPlace, i);
    const infra::Variant<bool, int> v2(infra::inPlace, i);
    const infra::Variant<bool, int> v3(infra::inPlace, j);
    const infra::Variant<bool, int> v4(infra::inPlace, k);

    EXPECT_GE(v1, v2);
    EXPECT_LE(v1, v2);
    EXPECT_LT(v1, v3);
    EXPECT_GT(v1, v4);
}

struct DoubleVisitor
    : infra::StaticVisitor<int>
{
    template<class T, class U>
    int operator()(T x, U y)
    {
        return x + y;
    }
};

TEST(VariantTest, TestDoubleVisitor)
{
    infra::Variant<bool, int> variant1(infra::inPlace, 5);
    infra::Variant<bool, int> variant2(infra::inPlace, true);
    DoubleVisitor visitor;
    EXPECT_EQ(6, infra::ApplyVisitor(visitor, variant1, variant2));
}

struct EmptyVisitor
    : infra::StaticVisitor<void>
{
    template<class T>
    void operator()(T)
    {}

    template<class T>
    void operator()(T, T)
    {}

    template<class T, class U>
    void operator()(T, U)
    {}
};

TEST(VariantTest, TestRecursiveLoopUnrolling)
{
    struct A {};
    struct B {};
    struct C {};
    struct D {};
    struct E {};
    struct F {};

    infra::Variant<A, B, C, D, E, F> v(infra::inPlace, F());
    EmptyVisitor visitor;
    infra::ApplyVisitor(visitor, v);
    infra::ApplyVisitor(visitor, v, v);
    infra::ApplySameTypeVisitor(visitor, v, v);    
}

template<class Trigger, class To>
struct Transition {};

struct EmptyState {};

typedef const void* StateIdentifier;
typedef const void* TriggerIdentifier;

template<class StateList>
struct StateConstructor;

template<class State>
struct StateConstructor<infra::List<State>>
{
    infra::Variant<State> operator()(StateIdentifier ident) const
    {
        if (State::ident != ident)
            std::abort();

        return infra::Variant<State>();
    }
};

template<class State, class State2, class... States>
struct StateConstructor<infra::List<State, State2, States...>>
{
    infra::Variant<State, State2, States...> operator()(StateIdentifier ident) const
    {
        if (State::ident == ident)
            return infra::Variant<State, State2, States...>(infra::inPlace, State());
        else
            return StateConstructor<infra::List<State2, States...>>()(ident);
    }
};

template<class StateList, class TransitionList>
struct MakeStateVariantHelper;

template<class StateList>
struct MakeStateVariantHelper<StateList, infra::List<>>
{
    typedef StateList Type;
};
    
template<class StateList, class Trigger, class State, class... Transitions>
struct MakeStateVariantHelper<StateList, infra::List<Transition<Trigger, State>, Transitions...>>
{
    template<class State, class StateList2, bool StateExistsInList>
    struct AddStateHelper;

    template<class State, class StateList2>
    struct AddStateHelper<State, StateList2, false>
    {
        typedef typename MakeStateVariantHelper<
            typename infra::ListPushBack<StateList, State>::Type,
            typename infra::ListPushBack<typename State::Transitions, Transitions...>::Type
        >::Type Type;
    };

    template<class State, class StateList2>
    struct AddStateHelper<State, StateList2, true>
    {
        typedef typename MakeStateVariantHelper<
            StateList,
            infra::List<Transitions...>
        >::Type Type;
    };

    typedef typename AddStateHelper<State, StateList, infra::ExistsInList<StateList, State>::value>::Type Type;
};

template<class FirstState>
struct CompileStates
{
    typedef typename MakeStateVariantHelper<infra::List<FirstState>, typename FirstState::Transitions>::Type StateList;

    typedef typename infra::MakeVariantOver<StateList>::Type StateVariant;
    typedef StateConstructor<StateList> StateConstructor;
};

template<class T>
struct GetNextStateIdentifierVisitor
    : infra::StaticVisitor<StateIdentifier>
{
    template<class T2>
    StateIdentifier operator()(T2& state)
    {
        return state.nextState<T>();
    }
};

template<class StateList, class NestedStateList>
struct GetNestedStates;

template<class NestedStateList>
struct GetNestedStates<infra::List<>, NestedStateList>
{
    typedef NestedStateList Type;
};

template<class State, class... States, class NestedStateList>
struct GetNestedStates<infra::List<State, States...>, NestedStateList>
{
    typedef typename GetNestedStates<
        infra::List<States...>,
        typename infra::ListPushBack<NestedStateList, typename State::FirstNestedState>::Type
    >::Type Type;
};

template<class FirstState>
class StateMachine
{
public:
    StateMachine()
        : targetState(FirstState::ident)
    {}

    template<class T>
    void post()
    {
        StateIdentifier currentState = targetState;

        GetNextStateIdentifierVisitor<T> visitor;
        targetState = infra::ApplyVisitor(visitor, state);

        if (targetState != nullptr && currentState != targetState)
            constructTargetState();
    }

    void constructTargetState()
    {
        state = typename CompiledStates::StateConstructor()(targetState);
    }

private:
    typedef CompileStates<FirstState> CompiledStates;
    typename CompiledStates::StateVariant state;
    typedef typename GetNestedStates<typename CompiledStates::StateList, infra::List<>>::Type NestedStates;

    StateIdentifier targetState;
};

template<class T>
struct Trigger
{
    static const TriggerIdentifier ident;
};

template<class T>
const TriggerIdentifier Trigger<T>::ident = &Trigger<T>::ident;

template<class Trigger, class... Transitions>
struct GetNextStateIdent;

template<class Trigger>
struct GetNextStateIdent<Trigger, infra::List<>>
{
    static StateIdentifier ident()
    {
        return nullptr;
    }
};

template<class Trigger, class State, class... OtherTransitions>
struct GetNextStateIdent<Trigger, infra::List<Transition<Trigger, State>, OtherTransitions...>>
{
    static StateIdentifier ident()
    {
        return State::ident;
    }
};

template<class Trigger, class OtherTrigger, class State, class... OtherTransitions>
struct GetNextStateIdent<Trigger, infra::List<Transition<OtherTrigger, State>, OtherTransitions...>>
{
    static StateIdentifier ident()
    {
        return GetNextStateIdent<Trigger, infra::List<OtherTransitions...>>::ident();
    }
};

template<class T>
struct State
{
    static const StateIdentifier ident;

    typedef infra::List<> Transitions;
    typedef EmptyState FirstNestedState;

    template<class Trigger>
    StateIdentifier nextState() const
    {
        return GetNextStateIdent<Trigger, T::Transitions>::ident();
    }
};

template<class T>
const StateIdentifier State<T>::ident = &State<T>::ident;

struct Tr1: Trigger<Tr1> {};
struct Tr2: Trigger<Tr2> {};

struct StateA;
struct StateB;

struct StateA
    : State<StateA>
{
    typedef infra::List<Transition<Tr1, StateB>> Transitions;
};

struct StateB
    : State<StateB>
{
    typedef infra::List<Transition<Tr1, StateA>, Transition<Tr2, StateB>> Transitions;
};

TEST(StateMachineTest, a)
{
    StateMachine<StateA> s;
    s.post<Tr1>();
    s.post<Tr2>();
    s.post<Tr1>();
    s.post<Tr2>();
}
