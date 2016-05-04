#include "Generated\C\Bar.h"


static keyId_t myKid;
static uint16_t myV;

keyId_t Bar_GetKeyId(void)
{
    return myKid;
}

void Bar_SetKeyId(keyId_t* kid)
{
    myKid = *kid;
}

bool Bar_IO_In(uint16_t v)
{
    myV = v;
    return true;
}

bool Bar_IO_Out(uint16_t* v)
{
    *v = myV;
    return true;
}

void Bar_IO_InOut(uint16_t* v)
{
    uint16_t tmp = myV;
    myV = *v;
    *v = tmp;
}
