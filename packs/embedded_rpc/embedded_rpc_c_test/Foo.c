#include "Generated\C\Foo.h"
#include "Generated\C\FooEvents.h"

static uint16_t value;
void Foo_DoThis(uint16_t v)
{
    value = v;
    FooEvents_UpdateMe(v);
}

uint16_t Foo_DoThat(uint8_t me)
{
    return value*me;
}

void Foo_DoThisArray(array_t* array)
{
    uint8_t i;
    for(i=0; i<sizeof(array->data); ++i)
    {
        //Just multiply all values by 2
        array->data[i] = array->data[i] * 2;
    }
    FooEvents_UpdateMeArray(array);
}

PCString Foo_UpperCase(PCString* str)
{
    PCString res;
    uint8_t i=0;
    while(str->text[i])
    {
        if(str->text[i]>='a' && str->text[i]<='z')
        {
            res.text[i] = str->text[i]-32;
        }
        else
        {
            res.text[i] = str->text[i];
        }
        i++;
    }
    res.text[i] = 0;
    return res;
}