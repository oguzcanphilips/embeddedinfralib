#ifndef FOOPROXYRENAME_H
#define FOOPROXYRENAME_H
/* rename methods from Foo_DoThis to FooProxy_DoThis.
*/
#ifdef FOO_H
#undef FOO_H
#endif

#define Foo_Init            FooProxy_Init
#define Foo_DeInit          FooProxy_DeInit
#define Foo_DoThis          FooProxy_DoThis
#define Foo_DoThat          FooProxy_DoThat
#define Foo_DoThisArray     FooProxy_DoThisArray
#define Foo_UpperCase       FooProxy_UpperCase

#include "Generated\C\Foo.h"

#undef FOO_H

#endif