#ifndef FOOIMPL_HPP
#define FOOIMPL_HPP

#include "Generated\CPP\IFoo.hpp"

class FooImpl : public erpc::IFoo
{
public:
    erpc::Array mData;
    void DoThis(const erpc::Array& array)
    {
        mData = array;
    }
    uint32_t DoThat(uint32_t i)
    {
        return i * 2;
    }
    erpc::PCString UpperCase(const erpc::PCString& str)
    {
        char res[PCSTRING_LEN];
        uint8_t i = 0;
        while (str[i])
        {
            if (str[i] >= 'a' && str[i] <= 'z')
                res[i] = str[i] - 32;
            else
                res[i] = str[i];
            i++;
        }
        res[i] = 0;
        return res;
    }

    erpc::Scope NextScope(const erpc::Scope& s)
    {
        if (s == erpc::Scope::Low)
            return erpc::Scope::Med;
        if (s == erpc::Scope::Med)
            return erpc::Scope::High;
        return erpc::Scope::Low;
    }
};

#endif