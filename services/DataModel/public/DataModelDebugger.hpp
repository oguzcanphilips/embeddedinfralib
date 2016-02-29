#ifndef DATAMODELDEBUGGER_H
#define DATAMODELDEBUGGER_H

#include "DataModel.hpp"
#include "infra\util\public\InterfaceConnector.hpp"
#include "Utils.hpp"

enum class DataModelContentFieldId;

class DataModelDebugger :  public infra::InterfaceConnector<DataModelDebugger>
{
    class DebugOutputStream : public util::OutputStream
    {
    public: 
        DebugOutputStream(DataModelDebugger* parent) : parent(parent){}
    protected:
        void ProcessOutput(char c){parent->OutputSignal(c); }
    private:
        DataModelDebugger* parent;
    };
public:
    friend class DebugFieldBase;
    class DebugFieldBase
    {
        DataModelContentFieldId mId;
        const char* mName;
    public:
        DebugFieldBase* next;
        virtual void Get(util::OutputStream& output) = 0;
        virtual void Set(const char* buffer) = 0;

        DebugFieldBase(DataModelContentFieldId id, const char* name, DataModelDebugger& debugger);
        DataModelContentFieldId Id(){return mId;}
        const char* Name(){return mName;}
        virtual bool IsLocked() const = 0;
        virtual bool IsSet() const = 0;
        virtual void Attach() = 0;
        virtual void Detach() = 0;
        virtual void Lock() = 0;
        virtual void Unlock() = 0;
    };

    template<typename TYPE>
    class DebugField : public DebugFieldBase
    {
        DataModelDebugger& mDebugger;
        DataModel::Writer<TYPE> mField;
        infra::Slot<DebugField<TYPE> > mUpdateSlot;
    public:
        DebugField(DataModelContentFieldId id, const char* name, DataModelDebugger& debugger) 
		: DebugFieldBase(id, name, debugger)
		, mDebugger(debugger)
		, mField(id)
        , mUpdateSlot(Self(), &DebugField::Update)
        {}
        void Get(util::OutputStream& output);
        //{
        //    if(len>=2) {buffer[0] = '?';buffer[1] = 0;}
        //}
        void Set(const char* buffer);
		//{}
        bool IsLocked() const override
        {
            return mField.IsLocked();
        }
        bool IsSet() const override
        {
            return mField.IsSet();
        }
        void Attach(){ mField.UpdateNotification() += mUpdateSlot; }
        void Detach(){ mField.UpdateNotification() -= mUpdateSlot; }
        void Lock(){ mField.Lock();}
        void Unlock() {mField.Unlock();}
    private:
        DebugField<TYPE>* Self(){ return this; }
        void Update() { mDebugger.Print(this, false); }
    };

    enum
    {
        NumberFieldSize = 4,
		NameFieldSize = 10
    };

    DataModelDebugger();

    virtual void ProcessInput(char c);
    util::OutputStream& Output();

    infra::Signal<DebugOutputStream, char> OutputSignal;

    class Callback
    {
    public:
        virtual void PrintVersion(util::OutputStream& outputStream) = 0;
    };
    void SetCallback(Callback& callback);

private:
    DebugFieldBase* GetField(const char* name) const;
    DebugFieldBase* GetField(DataModelContentFieldId id) const;
    void Print(DebugFieldBase* field, bool showInfo);
    void PrintModel(bool showInfo);
    void Print(const char* name, bool showInfo);

    char mInput[64];
    DebugFieldBase* mDebugFieldList;
    uint8_t mInputIndex;
    Callback* callback;
    DebugOutputStream output;

    DataModelDebugger(const DataModelDebugger&);
    const DataModelDebugger& operator=(const DataModelDebugger&);
};
#endif
