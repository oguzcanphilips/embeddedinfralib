#ifndef FOOEVENTSPROXYRENAME_H
#define FOOEVENTSPROXYRENAME_H  
/* rename methods from FooEvents_UpdateValue to FooEventsProxy_UpdateValue.
*/
#ifdef FOOEVENTSSKELETON_H
#undef FOOEVENTSSKELETON_H
#endif

#define FooEventsSkeleton_Init         FooEventsSkeletonRename_Init
#define FooEventsSkeleton_DeInit       FooEventsSkeletonRename_DeInit
#define FooEvents_UpdateMe             FooEventsRename_UpdateMe
#define FooEvents_UpdateMeArray        FooEventsRename_UpdateMeArray
#include "Generated\C\FooEventsSkeleton.h"

#undef FOOEVENTSSKELETON_H

#endif  /* FOOEVENTSPROXYRENAME_H */