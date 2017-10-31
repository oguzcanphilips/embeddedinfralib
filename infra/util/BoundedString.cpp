#include "BoundedString.hpp"

namespace infra
{
	BoundedString ByteRangeAsString(infra::MemoryRange<uint8_t> range)
	{
		return BoundedString(reinterpret_cast<char*>(range.begin()), range.size());
	}

	BoundedConstString ConstByteRangeAsString(infra::MemoryRange<const uint8_t> range)
	{
		return BoundedConstString(reinterpret_cast<const char*>(range.begin()), range.size());
	}
}