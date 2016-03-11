#include "DataModelDebugger.hpp"
#include "infra/stream/public/StringInputStream.hpp"
#include "infra/stream/public/StringOutputStream.hpp"

#define BACKSPACE 8

static char ToLower(char c)
{
    return (c >= 'A' && c <= 'Z') ? c + 'a' - 'A' : c;
}

static bool IsEqualCaseInsensitive(const char* a, const char* b)
{
    while (*a && *b)
        if (ToLower(*a++) != ToLower(*b++))
            return false;
    return (*a == *b);
}
namespace service
{
    DataModelDebugger::DebugOutputStream::DebugOutputStream(DataModelDebugger* parent) 
        : infra::TextOutputStream(static_cast<infra::StreamWriter&>(*this))
        , parent(parent)
    {}
    void DataModelDebugger::DebugOutputStream::Insert(infra::ConstByteRange range)
    {
        for(uint8_t element : range)
            Insert(element);
    }
    void DataModelDebugger::DebugOutputStream::Insert(uint8_t element)
    {
        parent->OutputSignal(element);
    }
    void DataModelDebugger::DebugOutputStream::Forward(std::size_t amount)
    {
        while (amount--)
            Insert(' ');
    }

    DataModelDebugger::DebugFieldBase::DebugFieldBase(DataModelContentFieldId id, const char* name, DataModelDebugger& debugger)
        : mId(id)
        , mName(name)
        , next(debugger.mDebugFieldList)
    {
        debugger.mDebugFieldList = this;
    }

    DataModelDebugger::DataModelDebugger()
        : mDebugFieldList(0)
        , mInputIndex(0)
        , callback(0)
        , output(this)
    {}

    void DataModelDebugger::SetCallback(Callback& callback)
    {
        this->callback = &callback;
    }

    void DataModelDebugger::ProcessInput(char c)
    {
        if (mInputIndex == 0 && c == '?')
        {
            output << infra::endl
                << "a #      - attach to field '#'" << infra::endl
                << "d #      - detach field '#'" << infra::endl
                << "d        - detach all fields" << infra::endl
                << "p #      - print field '#'" << infra::endl
                << "p filter - print fields with filter as part of 'name' " << infra::endl
                << "p        - print all fields" << infra::endl
                << "w # m    - write: field '#' = 'm'" << infra::endl
                << "l #      - lock field '#'" << infra::endl
                << "u #      - unlock field '#'" << infra::endl
                << infra::endl
                << "# is name or id of the selected field" << infra::endl;
        }
        if (c == BACKSPACE)
        {
            if (mInputIndex)
                mInputIndex--;
        }
        else if (c == '\r' || c == '\n')
        {
            mInput[mInputIndex] = 0;
            switch (mInput[0])
            {
            case 'p':
            case 'P':
            {
                bool showInfo = mInput[0] == 'P';
                Print(mInput + 1, showInfo);
            }
                break;
            case 'a':
            {
                DebugFieldBase* res = GetField(mInput + 1);
                if (res)
                    res->Attach();
            }
                break;
            case 'd':
            {
                if (mInput[1] == 0)
                {
                    for (DebugFieldBase* it = mDebugFieldList; it; it = it->next)
                        it->Detach();
                }
                else
                {
                    DebugFieldBase* res = GetField(mInput + 1);
                    if (res)
                        res->Detach();
                }
            }
                break;
            case 'l':
            {
                DebugFieldBase* res = GetField(mInput + 1);
                if (res)
                    res->Lock();
            }
                break;
            case 'u':
            {
                DebugFieldBase* res = GetField(mInput + 1);
                if (res)
                    res->Unlock();
            }
                break;
            case 'w':
            {
                mInput[0] = ' ';

                DebugFieldBase* res = GetField(mInput);
                if (res)
                {
                    const char* value = mInput;

                    while (*value == ' ') value++;
                    while (*value != ' ') value++;
                    while (*value == ' ') value++;
                    infra::StringInputStream input(value, infra::softFail);
                    res->Set(input);
                    if (input.HasFailed())
                        output << "Error: '" << mInput << "'" << infra::endl;

                }
            }
                break;
            default:
                break;
            }
            mInputIndex = 0;
            mInput[0] = 0;
        }
        else
        {
            mInput[mInputIndex++] = c;
            if (mInputIndex == sizeof(mInput)) mInputIndex = 0;
        }
    }

    DataModelDebugger::DebugFieldBase* DataModelDebugger::GetField(DataModelContentFieldId id) const
    {
        for (DebugFieldBase* it = mDebugFieldList; it; it = it->next)
        {
            if (it->Id() == id) return it;
        }
        return 0;
    }

    DataModelDebugger::DebugFieldBase* DataModelDebugger::GetField(const char* name) const
    {
        while (*name == ' ')
            ++name;

        char nameLocal[sizeof(mInput)];
        for (uint32_t i = 0; i < sizeof(nameLocal); ++i)
        {
            if (name[i] == 0 || name[i] == ' ')
            {
                nameLocal[i] = 0;
                break;
            }
            else
                nameLocal[i] = name[i];
        }

        int32_t idLocal = -1;
        infra::StringInputStream idStr(nameLocal, infra::softFail);
        idStr >> idLocal;
        if (!idStr.HasFailed())
        {
            DataModelDebugger::DebugFieldBase* res = GetField((DataModelContentFieldId)idLocal);
            if (res)
                return res;
        }

        for (DebugFieldBase* it = mDebugFieldList; it; it = it->next)
        {
            if (strcmp(it->Name(), nameLocal) == 0)
            {
                return it;
            }
        }
        return 0;
    }

    void DataModelDebugger::Print(const char* name, bool showInfo)
    {
        while (*name == ' ')
            ++name;

        DebugFieldBase* rd = GetField(name);
        if (rd)
        {
            Print(rd, showInfo);
        }
        else
        {
            bool found = false;
            if (name[0])
            {
                for (DebugFieldBase* it = mDebugFieldList; it; it = it->next)
                {
                    if (strstr(it->Name(), name))
                    {
                        found = true;
                        Print(it, showInfo);
                    }
                }
            }
            if (!found)
                PrintModel(showInfo);
        }
    }

    void DataModelDebugger::Print(DebugFieldBase* field, bool showInfo)
    {
        infra::StringOutputStream::WithStorage<32> txt;
        if (showInfo)
            txt << (field->IsLocked() ? "L" : " ") << (field->IsSet() ? "S " : "  ");

        txt << static_cast<int32_t>(field->Id());
        uint32_t len = txt.Storage().size();

        output << txt.Storage();
        for (uint32_t i = len; i < NumberFieldSize; ++i)
            output << " ";

        output << ", ";

        len = strlen(field->Name());
        output << field->Name();
        for (uint32_t i = len; i < NameFieldSize; ++i)
            output << " ";

        output << " : ";
        field->Get(output);
        output << infra::endl;
    }

    void DataModelDebugger::PrintModel(bool showInfo)
    {
        output << "--------------------" << infra::endl;
        if (callback)
            callback->PrintVersion(output);

        for (DebugFieldBase* it = mDebugFieldList; it; it = it->next)
            Print(it, showInfo);

        output << "---End---" << infra::endl;
    }

    infra::TextOutputStream& DataModelDebugger::Output()
    {
        return output;
    }

    template<> void DataModelDebugger::DebugField<bool>::Get(infra::TextOutputStream& output)    { output << (mField ? "True" : "False"); }
    template<> void DataModelDebugger::DebugField<uint8_t>::Get(infra::TextOutputStream& output) { output << mField; }
    template<> void DataModelDebugger::DebugField<uint16_t>::Get(infra::TextOutputStream& output){ output << mField; }
    template<> void DataModelDebugger::DebugField<uint32_t>::Get(infra::TextOutputStream& output){ output << mField; }
    template<> void DataModelDebugger::DebugField<int8_t>::Get(infra::TextOutputStream& output)  { output << mField; }
    template<> void DataModelDebugger::DebugField<int16_t>::Get(infra::TextOutputStream& output) { output << mField; }
    template<> void DataModelDebugger::DebugField<int32_t>::Get(infra::TextOutputStream& output) { output << mField; }
    template<> void DataModelDebugger::DebugField<float>::Get(infra::TextOutputStream& output)   { output << mField; }

    template<> void DataModelDebugger::DebugField<bool>::Set(infra::TextInputStream& input)    
    { 
        infra::BoundedString::WithStorage<1> v; 
        input >> v; 
        char c = v.Storage()[0];
        mField = (c == 't' || c == 'T' || c == '1'); 
    }
    template<> void DataModelDebugger::DebugField<uint8_t>::Set(infra::TextInputStream& input) { uint8_t v = 0;  input >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<uint16_t>::Set(infra::TextInputStream& input){ uint16_t v = 0; input >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<uint32_t>::Set(infra::TextInputStream& input){ uint32_t v = 0; input >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<int8_t>::Set(infra::TextInputStream& input)  { int8_t v = 0;   input >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<int16_t>::Set(infra::TextInputStream& input) { int16_t v = 0;  input >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<int32_t>::Set(infra::TextInputStream& input) { int32_t v = 0;  input >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<float>::Set(infra::TextInputStream& input)   { float v = 0;    input >> v; mField = v; }
}
