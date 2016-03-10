#include "DataModelDebugger.hpp"
#include "Utils.hpp"

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
            output << util::Endl
                << "a #      - attach to field '#'" << util::Endl
                << "d #      - detach field '#'" << util::Endl
                << "d        - detach all fields" << util::Endl
                << "p #      - print field '#'" << util::Endl
                << "p filter - print fields with filter as part of 'name' " << util::Endl
                << "p        - print all fields" << util::Endl
                << "w # m    - write: field '#' = 'm'" << util::Endl
                << "l #      - lock field '#'" << util::Endl
                << "u #      - unlock field '#'" << util::Endl
                << util::Endl
                << "# is name or id of the selected field" << util::Endl;
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

                            res->Set(value);
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
        util::InputStreamString idStr(nameLocal);
        idStr >> idLocal;
        if (!idStr.Fail())
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
        util::OutputStreamBuffered<32> txt;
        if (showInfo)
            txt << (field->IsLocked() ? "L" : " ") << (field->IsSet() ? "S " : "  ");

        txt << static_cast<int32_t>(field->Id());
        uint32_t len = txt.StrLen();

        output << txt;
        for (uint32_t i = len; i < NumberFieldSize; ++i)
            output << " ";

        output << ", ";

        len = strlen(field->Name());
        output << field->Name();
        for (uint32_t i = len; i < NameFieldSize; ++i)
            output << " ";

        output << " : ";
        field->Get(output);
        output << util::Endl;
    }

    void DataModelDebugger::PrintModel(bool showInfo)
    {
        output << "--------------------" << util::Endl;
        if (callback)
            callback->PrintVersion(output);

        for (DebugFieldBase* it = mDebugFieldList; it; it = it->next)
            Print(it, showInfo);

        output << "---End---" << util::Endl;
    }

    util::OutputStream& DataModelDebugger::Output()
    {
        return output;
    }

    template<> void DataModelDebugger::DebugField<bool>::Get(util::OutputStream& output)    { output << (mField ? "True" : "False"); }
    template<> void DataModelDebugger::DebugField<uint8_t>::Get(util::OutputStream& output) { output << mField; }
    template<> void DataModelDebugger::DebugField<uint16_t>::Get(util::OutputStream& output){ output << mField; }
    template<> void DataModelDebugger::DebugField<uint32_t>::Get(util::OutputStream& output){ output << mField; }
    template<> void DataModelDebugger::DebugField<int8_t>::Get(util::OutputStream& output)  { output << mField; }
    template<> void DataModelDebugger::DebugField<int16_t>::Get(util::OutputStream& output) { output << mField; }
    template<> void DataModelDebugger::DebugField<int32_t>::Get(util::OutputStream& output) { output << mField; }
    template<> void DataModelDebugger::DebugField<float>::Get(util::OutputStream& output) { output << mField; }

    template<> void DataModelDebugger::DebugField<bool>::Set(const char* buffer)    { mField = (IsEqualCaseInsensitive("True", buffer) || buffer[0] == '1') ? true : false; }
    template<> void DataModelDebugger::DebugField<uint8_t>::Set(const char* buffer) { uint8_t v = 0;  util::InputStreamString is(buffer); is >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<uint16_t>::Set(const char* buffer){ uint16_t v = 0; util::InputStreamString is(buffer); is >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<uint32_t>::Set(const char* buffer){ uint32_t v = 0; util::InputStreamString is(buffer); is >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<int8_t>::Set(const char* buffer)  { int8_t v = 0;   util::InputStreamString is(buffer); is >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<int16_t>::Set(const char* buffer) { int16_t v = 0;  util::InputStreamString is(buffer); is >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<int32_t>::Set(const char* buffer) { int32_t v = 0;  util::InputStreamString is(buffer); is >> v; mField = v; }
    template<> void DataModelDebugger::DebugField<float>::Set(const char* buffer)   { float v = 0;  util::InputStreamString is(buffer); is >> v; mField = v; }
}
