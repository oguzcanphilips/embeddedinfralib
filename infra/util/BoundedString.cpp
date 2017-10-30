#include "BoundedString.hpp"

namespace infra
{
	BoundedString MemoryRangeAsString(infra::MemoryRange<char> range)
	{
		return BoundedString(range.begin(), range.size());
	}

	BoundedConstString ConstMemoryRangeAsString(infra::MemoryRange<const char> range)
	{
		return BoundedConstString(range.begin(), range.size());
	}
}