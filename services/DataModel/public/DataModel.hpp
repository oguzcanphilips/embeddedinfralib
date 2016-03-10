#ifndef DATAMODEL_HPP
#define DATAMODEL_HPP
#include "DataModelConfig.hpp"
#include "infra\util\public\SignalSlot.hpp"
#include <cstdint>
#include <cstring>
#include <cassert>

namespace service
{
    class DataModel
    {
        template<typename ID, typename TYPE> class Collection;
        template<typename ID, typename TYPE, TYPE Min, TYPE Max> class FieldBounded;
        template<typename ID, typename TYPE> class Field;
        template<typename TYPE> class FieldBase;
        class FieldNotification;
        class CollectionBase;

        CollectionBase* parts;
        bool deserializing;

    public:
        template<typename ID, typename TYPE> class FieldGeneric;
        template<typename ID, typename TYPE> class FieldNonVolatile;
        template<typename ID, typename TYPE, TYPE Min, TYPE Max> class FieldGenericBounded;
        template<typename ID, typename TYPE, TYPE Min, TYPE Max> class FieldNonVolatileBounded;
        template<typename TYPE> class Writer;
        template<typename TYPE> class Reader;
        template<typename TYPE, typename ID> class WriterId;
        template<typename TYPE, typename ID> class ReaderId;

        class Input
        {
        public:
            template<class T>
            bool Read(T& v) { return ReadBuffer(reinterpret_cast<uint8_t*>(&v), sizeof(v)); }
            virtual bool ReadBuffer(uint8_t* buffer, uint32_t len) = 0;
        };

        class InputMemory : public Input
        {
        public:
            InputMemory(uint8_t* mem, uint32_t len);
            bool ReadBuffer(uint8_t* buffer, uint32_t readLen) override;
        private:
            uint8_t* mem;
            uint32_t len;
            uint32_t readPos = 0;
        };


        infra::Signal<DataModel> NonVolatileFieldChanged;
        static DataModel& Instance();

        uint32_t SerializeSize() const;
        uint32_t Serialize(uint8_t* data);
        bool Deserialize(Input& input);
        void ResetToDefault(bool persistentOnly);

        enum
        {
            SerializeProtocolVersion = 1
        };
    private:
        DataModel();
        void Register(CollectionBase* datamodel);
        void NotifyNonVolatileChanged();
    };

    class DataModel::FieldNotification
    {
    public:
        infra::Signal<FieldNotification> UpdateNotification;
        infra::Signal<FieldNotification> ChangedNotification;
    protected:
        void NotifyUpdate()
        {
            UpdateNotification();
        }
        void NotifyChanged()
        {
            ChangedNotification();
        }
    };

    template<typename TYPE>
    class DataModel::FieldBase : public DataModel::FieldNotification
    {
        friend class DataModel;
        const TYPE defaultValue;
        TYPE value;
        bool isSet = false;
        DataModel::Writer<TYPE>* lockedByWriter;

    public:
        bool IsLocked() const
        {
            return lockedByWriter != nullptr;
        }
        bool IsSet() const
        {
            return isSet;
        }
        void ResetToDefault()
        {
            if (isSet)
            {
                isSet = false;
                AssignNewValue(defaultValue);
            }
        }
    protected:
        FieldBase(TYPE defaultValue) :defaultValue(defaultValue), value(defaultValue), lockedByWriter(0){}
        virtual void OnChanged(){}
        virtual TYPE& Clip(TYPE& value){ return value; }
    private:
        operator const TYPE&() const
        {
            return value;
        }

        void AssignNewValue(const TYPE& v)
        {
            value = v;
            OnChanged();
            NotifyChanged();
            NotifyUpdate();
        }

        const TYPE& operator=(const TYPE& rhs)
        {
            TYPE v = rhs;
            Clip(v);
            bool wasSet = isSet;
            isSet = true;
            if (memcmp(&value, &v, sizeof(TYPE)) != 0 || !wasSet)
                AssignNewValue(v);
            else
                NotifyUpdate();
            return rhs;
        }
        const DataModel::FieldBase<TYPE>& operator=(const FieldBase<TYPE>&) = delete;
        FieldBase(const FieldBase<TYPE>&) = delete;
    };

    template<typename ID, typename TYPE>
    class DataModel::Field : public FieldBase<TYPE>
    {
        friend class Collection<ID, TYPE>;

        Field<ID, TYPE>* next;
        const ID id;
    protected:
        Field(ID identification, TYPE defaultValue)
            : FieldBase<TYPE>(defaultValue), next(0), id(identification){}
    private:
        const DataModel::Field<ID, TYPE>& operator= (const Field<ID, TYPE>&) = delete;
        Field(const Field<ID, TYPE>&) = delete;
    };

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    class DataModel::FieldBounded : public Field<ID, TYPE>
    {
    protected:
        FieldBounded(ID identification, TYPE defaultValue)
            : Field<ID, TYPE>(identification, Clip(defaultValue))
        {
                assert(Min < Max);
            }
        TYPE& Clip(TYPE& value){ if (value<Min) value = Min; else if (value>Max) value = Max; return value; }
    };

    class DataModel::CollectionBase
    {
        friend class DataModel;
        CollectionBase* next;
    protected:
        CollectionBase() : next(0){}
        virtual void ResetToDefault(bool persistentOnly) = 0;
        virtual uint32_t SerializeSize() const = 0;
        virtual uint8_t* Serialize(uint8_t* data) = 0;
        virtual void DeserializeElement(uint8_t* data) = 0;
    };

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    class DataModel::FieldNonVolatileBounded : public FieldBounded<ID, TYPE, Min, Max>
    {
    public:
        FieldNonVolatileBounded(ID identification, TYPE defaultValue);
        ~FieldNonVolatileBounded();
    protected:
        void OnChanged()
        {
            DataModel::Instance().NotifyNonVolatileChanged();
        }
    private:
        FieldNonVolatileBounded(const FieldNonVolatileBounded<ID, TYPE, Min, Max>&) = delete;
        const DataModel::FieldNonVolatileBounded<ID, TYPE, Min, Max>& operator=(const FieldNonVolatileBounded<ID, TYPE, Min, Max>&) = delete;

    };

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    class DataModel::FieldGenericBounded : public FieldBounded<ID, TYPE, Min, Max>
    {
    public:
        FieldGenericBounded(ID identification, TYPE defaultValue);
        ~FieldGenericBounded();
    private:
        FieldGenericBounded(const FieldGenericBounded<ID, TYPE, Min, Max>&) = delete;
        const DataModel::FieldGenericBounded<ID, TYPE, Min, Max>& operator=(const FieldGenericBounded<ID, TYPE, Min, Max>&) = delete;
    };

    template<typename ID, typename TYPE>
    class DataModel::FieldGeneric : public Field<ID, TYPE>
    {
    public:
        FieldGeneric(ID identification, TYPE defaultValue);
        ~FieldGeneric();
    private:
        FieldGeneric(const FieldGeneric<ID, TYPE>&) = delete;
        const DataModel::FieldGeneric<ID, TYPE>& operator=(const FieldGeneric<ID, TYPE>&) = delete;
    };

    template<typename ID, typename TYPE>
    class DataModel::FieldNonVolatile : public Field<ID, TYPE>
    {
    public:
        FieldNonVolatile(ID identification, TYPE defaultValue);
        ~FieldNonVolatile();
    protected:
        void OnChanged()
        {
            DataModel::Instance().NotifyNonVolatileChanged();
        }
    private:
        FieldNonVolatile(const FieldNonVolatile&);
        const FieldNonVolatile& operator=(const FieldNonVolatile&);
    };

    template<typename ID, typename TYPE>
    class DataModel::Collection : public CollectionBase
    {
        DataModel::Field<ID, TYPE>* listVolatile;
        DataModel::Field<ID, TYPE>* listNonVolatile;
        Collection() : listVolatile(0), listNonVolatile(0)
        {
            DataModel::Instance().Register(this);
        }
        void Remove(DataModel::Field<ID, TYPE>*& list, DataModel::Field<ID, TYPE>& field)
        {
            if (&field == list)
            {
                list = list->next;
                return;
            }
            for (DataModel::Field<ID, TYPE>* it = list; it; it = it->next)
            {
                if (&field == it->next)
                {
                    it->next = it->next->next;
                    return;
                }
            }
        }
        void Add(DataModel::Field<ID, TYPE>*& list, DataModel::Field<ID, TYPE>& field)
        {
            if (GetField(list, field.id)) return;
            field.next = list;
            list = &field;
        }
        DataModel::Field<ID, TYPE>* GetField(DataModel::Field<ID, TYPE>*& list, ID id)
        {
            for (DataModel::Field<ID, TYPE>* it = list; it; it = it->next)
            if (it->id == id) return it;
            return 0;
        }

        const static uint8_t elementSerializeSize = 1 + sizeof(ID)+1 + sizeof(TYPE); // length, ID, isUsed, Value
    public:
        static Collection<ID, TYPE>& Instance()
        {
            static Collection<ID, TYPE> instance;
            return instance;
        }
        void AddNonVolatile(DataModel::Field<ID, TYPE>& field)
        {
            Add(listNonVolatile, field);
        }
        void AddVolatile(DataModel::Field<ID, TYPE>& field)
        {
            Add(listVolatile, field);
        }
        void RemoveVolatile(DataModel::Field<ID, TYPE>& field)
        {
            Remove(listVolatile, field);
        }
        void RemoveNonVolatile(DataModel::Field<ID, TYPE>& field)
        {
            Remove(listNonVolatile, field);
        }
        DataModel::Field<ID, TYPE>& GetField(ID id)
        {
            DataModel::Field<ID, TYPE>* field = GetField(listVolatile, id);
            if (field == 0)
            {
                field = GetField(listNonVolatile, id);
                if (field == 0)
                {
                    HANDLE_ERROR
                }
            }
            return *field;
        }
    protected:
        virtual void ResetToDefault(bool persistentOnly)
        {
            for (DataModel::Field<ID, TYPE>* it = listNonVolatile; it; it = it->next)
                it->ResetToDefault();
            if (!persistentOnly)
            for (DataModel::Field<ID, TYPE>* it = listVolatile; it; it = it->next)
                it->ResetToDefault();
        }

        virtual uint32_t SerializeSize() const
        {
            uint32_t s = 0;
            for (DataModel::Field<ID, TYPE>* it = listNonVolatile; it; it = it->next)
                s += elementSerializeSize;
            return s;
        }

        virtual uint8_t* Serialize(uint8_t* data)
        {
            for (DataModel::Field<ID, TYPE>* it = listNonVolatile; it; it = it->next)
            {
                data[0] = elementSerializeSize;
                data++;
                memcpy(data, &it->id, sizeof(ID));
                data += sizeof(ID);
                data[0] = it->isSet ? 1 : 0;
                data++;
                memcpy(data, &it->value, sizeof(TYPE));
                data += sizeof(TYPE);
            }
            return data;
        }
        virtual void DeserializeElement(uint8_t* data)
        {
            uint8_t size = data[0];
            if (size == elementSerializeSize)
            {
                data++;
                ID id;
                memcpy(&id, data, sizeof(ID));
                DataModel::FieldBase<TYPE>* field = GetField(listNonVolatile, id);
                if (field)
                {
                    data += sizeof(ID);
                    field->isSet = data[0] == 1;
                    memcpy(&field->value, &data[1], sizeof(TYPE));
                }
            }
        }
    };

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    DataModel::FieldNonVolatileBounded<ID, TYPE, Min, Max>::FieldNonVolatileBounded(ID identification, TYPE defaultValue)
        : FieldBounded<ID, TYPE, Min, Max>(identification, defaultValue)
    {
            Collection<ID, TYPE>::Instance().AddNonVolatile(*this);
        }

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    DataModel::FieldNonVolatileBounded<ID, TYPE, Min, Max>::~FieldNonVolatileBounded()
    {
        Collection<ID, TYPE>::Instance().RemoveNonVolatile(*this);
    }

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    DataModel::FieldGenericBounded<ID, TYPE, Min, Max>::FieldGenericBounded(ID identification, TYPE defaultValue)
        : FieldBounded<ID, TYPE, Min, Max>(identification, defaultValue)
    {
            Collection<ID, TYPE>::Instance().AddVolatile(*this);
        }

    template<typename ID, typename TYPE, TYPE Min, TYPE Max>
    DataModel::FieldGenericBounded<ID, TYPE, Min, Max>::~FieldGenericBounded()
    {
        Collection<ID, TYPE>::Instance().RemoveVolatile(*this);
    }

    template<typename ID, typename TYPE>
    DataModel::FieldGeneric<ID, TYPE>::FieldGeneric(ID identification, TYPE defaultValue)
        : Field<ID, TYPE>(identification, defaultValue)
    {
            Collection<ID, TYPE>::Instance().AddVolatile(*this);
        }

    template<typename ID, typename TYPE>
    DataModel::FieldGeneric<ID, TYPE>::~FieldGeneric()
    {
        Collection<ID, TYPE>::Instance().RemoveVolatile(*this);
    }


    template<typename ID, typename TYPE>
    DataModel::FieldNonVolatile<ID, TYPE>::FieldNonVolatile(ID identification, TYPE defaultValue)
        : Field<ID, TYPE>(identification, defaultValue)
    {
            Collection<ID, TYPE>::Instance().AddNonVolatile(*this);
        }

    template<typename ID, typename TYPE>
    DataModel::FieldNonVolatile<ID, TYPE>::~FieldNonVolatile()
    {
        Collection<ID, TYPE>::Instance().RemoveNonVolatile(*this);
    }

    template<typename TYPE>
    class DataModel::Reader
    {
        friend class DataModel::Writer<TYPE>;

        DataModel::FieldBase<TYPE>& field;
    public:
        bool IsLocked() const
        {
            return field.IsLocked();
        }
        bool IsSet() const
        {
            return field.IsSet();
        }
        infra::Signal<DataModel::FieldNotification>& UpdateNotification()
        {
            return field.UpdateNotification;
        }
        infra::Signal<DataModel::FieldNotification>& ChangedNotification()
        {
            return field.ChangedNotification;
        }
        operator const TYPE&() const
        {
            return field;
        }
        template<typename ID>
        Reader(ID id)
            : field(Collection<ID, TYPE>::Instance().GetField(id))
        {
        }

        Reader(const Reader&) = delete;
        const DataModel::Reader<TYPE>& operator=(const Reader<TYPE>&) = delete;
    };

    template<typename TYPE>
    class DataModel::Writer : public DataModel::Reader<TYPE>
    {
    public:
        const TYPE& operator=(const TYPE& rhs)
        {
            if (DataModel::Reader<TYPE>::field.lockedByWriter == 0 || DataModel::Reader<TYPE>::field.lockedByWriter == this)
                DataModel::Reader<TYPE>::field = rhs;
            return rhs;
        }

        template<typename ID>
        Writer(ID id)
            : DataModel::Reader<TYPE>(id)
        {
            }
        bool Lock()
        {
            if (DataModel::Reader<TYPE>::field.lockedByWriter == 0)
                DataModel::Reader<TYPE>::field.lockedByWriter = this;
            return (DataModel::Reader<TYPE>::field.lockedByWriter == this);
        }
        void Unlock()
        {
            if (DataModel::Reader<TYPE>::field.lockedByWriter == this) DataModel::Reader<TYPE>::field.lockedByWriter = 0;
        }
        ~Writer()
        {
            Unlock();
        }
        void ResetToDefault()
        {
            DataModel::Reader<TYPE>::field.ResetToDefault();
        }
        Writer(const Writer&) = delete;
        const DataModel::Writer<TYPE>& operator=(const Writer<TYPE>&) = delete;
    };
}
#endif
