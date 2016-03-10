#ifndef DATAMODELCONTENT_HPP
#define DATAMODELCONTENT_HPP

#include "DataModel.hpp"

#define FIELD(ID,N,T,V)
#define FIELD_P(ID,N,T,V)
#define FIELD_MINMAX(ID,N,T,V,MIN,MAX)
#define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX)
#define DATAMODELDEFINITIONS

#include "DataModelContent.dm"

#undef DATAMODELDEFINITIONS
#undef FIELD
#undef FIELD_P
#undef FIELD_MINMAX
#undef FIELD_P_MINMAX
namespace service
{
    class DataModelContentDummy {};

    enum class DataModelContentFieldId
    {
#define FIELD(ID,N,T,V) N = ID,
#define FIELD_P(ID,N,T,V) N = ID,
#define FIELD_MINMAX(ID,N,T,V,MIN,MAX) N = ID,
#define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) N = ID,
#include "DataModelContent.dm"
#undef FIELD
#undef FIELD_P
#undef FIELD_MINMAX
#undef FIELD_P_MINMAX
        Unknown
    };


    class DataModelContent
        : private DataModelContentDummy
    {
#define FIELD(ID,N,T,V) DataModel::FieldGeneric<DataModelContentFieldId, T> m##N;
#define FIELD_P(ID,N,T,V) DataModel::FieldNonVolatile<DataModelContentFieldId, T> m##N;
#define FIELD_MINMAX(ID,N,T,V,MIN,MAX) DataModel::FieldGenericBounded<DataModelContentFieldId, T, MIN, MAX > m##N;
#define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) DataModel::FieldNonVolatileBounded<DataModelContentFieldId, T, MIN, MAX > m##N;
#include "DataModelContent.dm"
#undef FIELD
#undef FIELD_P
#undef FIELD_MINMAX
#undef FIELD_P_MINMAX
    public:
        enum
        {
            NonVolatileSize = sizeof(DataModel::SerializeProtocolVersion)
#define FIELD(ID,N,T,V)
#define FIELD_P(ID,N,T,V) +sizeof(DataModelContentFieldId)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(T)
#define FIELD_MINMAX(ID,N,T,V,MIN,MAX)
#define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) +sizeof(DataModelContentFieldId)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(T)
#include "DataModelContent.dm"
#undef FIELD
#undef FIELD_P
#undef FIELD_MINMAX
#undef FIELD_P_MINMAX
        };

        DataModelContent()
            : DataModelContentDummy()
#define FIELD(ID,N,T,V)                  , m##N(DataModelContentFieldId::N, V)
#define FIELD_P(ID,N,T,V)                , m##N(DataModelContentFieldId::N, V)
#define FIELD_MINMAX(ID,N,T,V,MIN,MAX)   , m##N(DataModelContentFieldId::N, V)
#define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) , m##N(DataModelContentFieldId::N, V)
#include "DataModelContent.dm"
#undef FIELD
#undef FIELD_P
#undef FIELD_MINMAX
#undef FIELD_P_MINMAX
        {
        }

        template<DataModelContentFieldId ID>
        class Writer;
        template<DataModelContentFieldId ID>
        class Reader;
        template<DataModelContentFieldId I>
        struct IdToType;
    };

#define FIELD_P(ID,N,T,V)                template<> struct DataModelContent::IdToType<DataModelContentFieldId::N>{ typedef T Type; };
#define FIELD(ID,N,T,V)                  template<> struct DataModelContent::IdToType<DataModelContentFieldId::N>{ typedef T Type; };
#define FIELD_MINMAX(ID,N,T,V,MIN,MAX)   template<> struct DataModelContent::IdToType<DataModelContentFieldId::N>{ typedef T Type; };
#define FIELD_P_MINMAX(ID,N,T,V,MIN,MAX) template<> struct DataModelContent::IdToType<DataModelContentFieldId::N>{ typedef T Type; };
#include "DataModelContent.dm"
#undef FIELD
#undef FIELD_P
#undef FIELD_MINMAX
#undef FIELD_P_MINMAX

    template<DataModelContentFieldId ID>
    class DataModelContent::Writer : public DataModel::Writer<typename DataModelContent::IdToType<ID>::Type>
    {
    public:
        using DataModel::Writer<typename DataModelContent::IdToType<ID>::Type>::operator=;
        Writer() : DataModel::Writer<typename DataModelContent::IdToType<ID>::Type>(ID){}
    };

    template<DataModelContentFieldId ID>
    class DataModelContent::Reader : public DataModel::Reader<typename DataModelContent::IdToType<ID>::Type>
    {
    public:
        Reader() : DataModel::Reader<typename DataModelContent::IdToType<ID>::Type>(ID){}
    };
}
#endif
