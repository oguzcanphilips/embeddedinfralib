#ifndef DATAMODELDEBUGGERCONTENT_HPP
#define DATAMODELDEBUGGERCONTENT_HPP

#include "DataModelContent.hpp"
#include "DataModelDebugger.hpp"

class DataModelDebuggerContent
{
    class Dummy{};
private:
    Dummy dummy;
    #define FIELD(ID,N,T,V)   DataModelDebugger::DebugField<T> m##N;
    #define FIELD_P(ID,N,T,V)   DataModelDebugger::DebugField<T> m##N;
    #define FIELD_MINMAX(ID,N,T,V,MIN,MAX) DataModelDebugger::DebugField<T> m##N;
    #define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) DataModelDebugger::DebugField<T> m##N;
    #include "DataModelContent.dm"
    #undef FIELD
    #undef FIELD_P
    #undef FIELD_MINMAX
    #undef FIELD_P_MINMAX
public:
    DataModelDebuggerContent(DataModelDebugger& debugger)
        : dummy()
    #define FIELD(ID,N,T,V) , m##N(DataModelContentFieldId::N, #N, debugger)
    #define FIELD_P(ID,N,T,V) , m##N(DataModelContentFieldId::N, #N, debugger)
    #define FIELD_MINMAX(ID,N,T,V,MIN,MAX) , m##N(DataModelContentFieldId::N, #N, debugger)
    #define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) , m##N(DataModelContentFieldId::N, #N, debugger)
    #include "DataModelContent.dm"
    #undef FIELD
    #undef FIELD_P
    #undef FIELD_MINMAX
    #undef FIELD_P_MINMAX
    {}
};
#endif
