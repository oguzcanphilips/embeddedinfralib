#include "upgrade/boot_loader/test/MockDiComm.hpp"

namespace application
{
    bool MockDiComm::Initialize()
    {
        return InitializeMock();
    }

    bool MockDiComm::GetProps(infra::BoundedConstString port, infra::BoundedString& values)
    {
        std::pair<bool, std::string> result = GetPropsMock(std::string(port.begin(), port.end()));
        assert(values.max_size() >= result.second.size());
        values.resize(result.second.size());
        std::copy(result.second.begin(), result.second.end(), values.begin());

        return result.first;
    }

    bool MockDiComm::PutProps(infra::BoundedConstString port, infra::BoundedConstString values, infra::BoundedString& resultingValues)
    {
        std::pair<bool, std::string> result = PutPropsMock(std::string(port.begin(), port.end()), std::string(values.begin(), values.end()));
        assert(resultingValues.max_size() >= result.second.size());
        resultingValues.resize(result.second.size());
        std::copy(result.second.begin(), result.second.end(), resultingValues.begin());

        return result.first;
    }
}
