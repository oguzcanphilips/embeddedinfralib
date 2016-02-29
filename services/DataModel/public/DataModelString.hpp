#ifndef DATAMODELSTRING_HPP
#define DATAMODELSTRING_HPP

#include "DataModel.hpp"
#include <cstdint>
#include <cstring>

class DataModelString
{
public:
    DataModelString(const char* s);

    void operator=(const char* s);
    bool operator==(const DataModelString& rhs);
    bool operator!=(const DataModelString& rhs);
    operator const char*() const;

private:
    void Copy(const char* s);
    char str[32];
};

#endif
