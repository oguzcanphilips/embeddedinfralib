#include "DataModelString.hpp"
#include <cstring>

DataModelString::DataModelString(const char* s)
{
	Copy(s);
}

void DataModelString::operator=(const char* s)
{
	Copy(s);
}

bool DataModelString::operator==(const DataModelString& rhs)
{
    return strncmp(str, rhs, sizeof(str)) == 0;
}

bool DataModelString::operator!=(const DataModelString& rhs)
{
    return !(*this==rhs);
}

DataModelString::operator const char*() const
{
	return str;
}

void DataModelString::Copy(const char* s)
{
    uint32_t index = 0;
    for (; s[index] && index < (sizeof(str)-1); ++index)
        str[index] = s[index];
    for (; index < (sizeof(str)); ++index)
        str[index] = 0;
}
