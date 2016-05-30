#ifndef BARIMPL_HPP
#define BARIMPL_HPP

#include "Generated\CPP\IBar.hpp"

class BarImpl : public erpc::IBar
{
public:
    erpc::KeyId mKid;
    void SetKeyId(const erpc::KeyId& v)
    {
        mKid = v;
    }
    erpc::KeyId GetKeyId()
    {
        return mKid;
    }
    void SetGetKeyId(erpc::KeyId& kid)
    {
        erpc::KeyId tmp = mKid;
        mKid = kid;
        kid = tmp;
    }
}; 

#endif