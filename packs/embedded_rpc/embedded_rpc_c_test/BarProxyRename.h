#ifndef BARPROXYRENAME_H
#define BARPROXYRENAME_H
/* rename methods from Bar_SetKeyId to BarProxy_SetKeyId.
*/
#ifdef BAR_H
#undef BAR_H
#endif

#define Bar_Init       BarProxy_Init
#define Bar_DeInit     BarProxy_DeInit
#define Bar_SetKeyId   BarProxy_SetKeyId
#define Bar_GetKeyId   BarProxy_GetKeyId
#define Bar_IO_In      BarProxy_In
#define Bar_IO_Out     BarProxy_Out
#define Bar_IO_InOut   BarProxy_InOut

#include "Generated\C\Bar.h"

#undef BAR_H

#endif