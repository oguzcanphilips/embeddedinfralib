#include "services\DataModel\public\DataModelDebugger.hpp"
#include "TestConfig.hpp"

namespace service
{
    void DataModelDebugger::DebugField<TestConfig>::Get(util::OutputStream& ouput)
    {
        ouput << "-";
    }
    void DataModelDebugger::DebugField<TestConfig>::Set(const char*)
    {
    }
}