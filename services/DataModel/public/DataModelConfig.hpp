#ifndef DATAMODELCONFIG_H
#define DATAMODELCONFIG_H

#ifdef WIN32
    #include <exception>
    #define HANDLE_ERROR throw std::exception();// DataModel not constructed??
#else
    #include <cassert>
    #define HANDLE_ERROR assert(false);
#endif

#endif
