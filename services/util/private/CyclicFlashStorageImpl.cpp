#include "services/util/public/CyclicFlashStorageImpl.hpp"

namespace services
{

    CyclicFlashStorageImpl::CyclicFlashStorageImpl(hal::Flash& flash)
        : flash(flash)
    {

    }

    void Initialize(infra::Function<void()> onDone)
    {

    }
}
