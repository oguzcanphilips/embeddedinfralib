using System;
using System.Collections.Generic;
using System.Linq;

namespace codegen
{
    internal class Interface
    {
        private static byte mInterfaceId = 0;
        private readonly byte mId;
        private readonly string mName;
        private List<Function> mFunctions = new List<Function>();
        private List<Function> mEvents = new List<Function>();

        public byte Id
        {
            get { return mId; }
        }

        public string Name
        {
            get { return mName; }
        }

        public List<Function> Functions
        {
            get { return mFunctions; }
        }

        public List<Function> Events
        {
            get { return mEvents; }
        }

        public bool HasEvents
        {
            get { return (mEvents.Count > 0); }
        }

        public bool HasReturnValues
        {
            get { return mFunctions.Any(f => f.HasReturn); }
        }

        public Interface CreateEventInterface()
        {
            if (mEvents.Count == 0) return null;

            Interface i = new Interface(Name + "Events", (byte) (mId + 1));
            i.mFunctions = mEvents;
            if (i.HasReturnValues)
            {
                throw new Exception("Events shall not have <Out> or <inOut> params");
            }
            return i;
        }

        private Interface(string name, byte id)
        {
            mId = id;
            mName = name;
        }

        public Interface(Input input)
        {
            mId = mInterfaceId;
            mInterfaceId += 2;
            mName = input.GetNext();
            string tag = input.GetNext();
            if (tag.Equals("="))
            {
                string id = input.GetNext();
                try
                {
                    mId = (byte) (2*byte.Parse(id));
                    mInterfaceId = (byte) (2*(mId + 1));
                }
                catch (Exception)
                {
                    throw new Exception("Invalid interface id: " + id);
                }
                tag = input.GetNext();
            }
            byte eventId = 0;
            byte functionId = 0;
            while (true)
            {
                if (tag.Equals("end")) return;
                string retVal = tag;
                string function = input.GetNext();
                string startParam = input.GetNext();
                bool isEvent = retVal.Equals("EVENT");

                if (!startParam.Equals("("))
                    throw new Exception("Syntax error in function: " + function + " " + startParam + " in " + mName);
                Function func = isEvent ? new Function("void", function, eventId) : new Function(retVal, function, functionId);
                tag = input.GetNext();
                if (tag.Equals("void")) tag = input.GetNext(); // skip void function declarations
                while (!tag.Equals(")"))
                {
                    try
                    {
                        func.Add(new Param(tag, input.GetNext()));
                    }
                    catch (Exception e)
                    {
                        throw new Exception("Wrong function declaration at " + mName + "." + function + ", " + e.Message);
                    }
                    tag = input.GetNext();
                    if (tag.Equals(",")) tag = input.GetNext();
                    if (tag.Equals("") || tag.Equals("("))
                    {
                        throw new Exception("Wrong function declaration at " + mName + "." + function);
                    }
                }
                tag = input.GetNext();
                if (tag.Equals("="))
                {
                    byte id = 0;
                    try
                    {
                        id = byte.Parse(input.GetNext());
                    }
                    catch (Exception)
                    {
                        throw new Exception("Invalid event/function id: " + id);
                    }
                    func.Id = id;
                    if (isEvent)
                        eventId = id;
                    else
                        functionId = id;
                    tag = input.GetNext();
                }
                if (isEvent)
                {
                    AddFunction(mEvents, func);
                    eventId++;
                }
                else
                {
                    AddFunction(mFunctions, func);
                    functionId++;
                }
            }
        }

        private void AddFunction(List<Function> container, Function func)
        {
            foreach (Function f in container)
            {
                if (f.Id == func.Id)
                {
                    throw new Exception("Duplicate function id: " + f.Id + ", (" + f.Name + " = " + func.Name + ")");
                }
            }
            container.Add(func);
        }

        public HashSet<string> GetDependencies()
        {
            HashSet<string> res = new HashSet<string>();
            foreach (var f in mFunctions)
            {
                HashSet<string> deps = f.GetDependencies();
                foreach (var type in deps)
                {
                    res.Add(type);
                }
            }
            foreach (var e in mEvents)
            {
                HashSet<string> deps = e.GetDependencies();
                foreach (var type in deps)
                {
                    res.Add(type);
                }
            }
            foreach (var t in Parser.mSystemTypes)
            {
                res.Remove(t);
            }
            return res;
        }
    }
}
