#include "services\DataModel\public\DataModelDebugger.hpp"
#include "TestConfig.hpp"

namespace service
{
    void DataModelDebugger::DebugField<TestConfig>::Get(infra::TextOutputStream& ouput)
    {
        ouput << "-";
    }
    void DataModelDebugger::DebugField<TestConfig>::Set(infra::TextInputStream& input)
    {
    }
}