#include "DataModel.hpp"


DataModel::InputMemory::InputMemory(uint8_t* mem, uint32_t len)
    : mem(mem)
    , len(len)
{
}

bool DataModel::InputMemory::ReadBuffer(uint8_t* buffer, uint32_t readLen)
{
    if ((readLen + readPos) > len)
        return false;

    memcpy(buffer, mem + readPos, readLen);
    readPos += readLen;
    return true;
}

DataModel::DataModel()
: parts(0)
, deserializing(false)
{
}

DataModel& DataModel::Instance()
{
    static DataModel instance;
    return instance;
}

void DataModel::NotifyNonVolatileChanged()
{
    if(!deserializing)
    {
        NonVolatileFieldChanged();
    }
}

void DataModel::Register(CollectionBase* datamodel)
{
    datamodel->next = parts;
    parts = datamodel;
}

uint32_t DataModel::SerializeSize() const
{
    uint32_t s = sizeof((uint32_t)SerializeProtocolVersion);
    for(CollectionBase* i = parts; i; i=i->next)
    {
        s+= i->SerializeSize();
    }
    return s;
}

void DataModel::ResetToDefault(bool persistentOnly)
{
    for (CollectionBase* i = parts; i; i = i->next)
        i->ResetToDefault(persistentOnly);
}

uint32_t DataModel::Serialize(uint8_t* data)
{
    uint8_t* begin = data;
    uint32_t version = SerializeProtocolVersion;
    memcpy(data, &version, sizeof(version));
    data += sizeof(version);

    for(CollectionBase* i = parts; i; i=i->next)
    {
        data = i->Serialize(data);
    }
    return data-begin;
}

bool DataModel::Deserialize(Input& input)
{
    class ScopedFlag
    {
        bool& flag;
    public:
        ScopedFlag(bool& flag) : flag(flag){ flag = true; }
        ~ScopedFlag(){ flag = false; }
    };

    ScopedFlag deserializeFlag(deserializing);

    uint32_t protocolVersion = 0;
    if (!input.Read(protocolVersion) || protocolVersion != SerializeProtocolVersion)
        return false;

    uint8_t element[256];
    while (input.Read(element[0]))
    {
        if (element[0] == 0 || !input.ReadBuffer(element + 1, element[0] - 1))
            return false;

        for (CollectionBase* i = parts; i; i = i->next)
            i->DeserializeElement(element);
    }
    return true;
}
