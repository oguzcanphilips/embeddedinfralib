using System;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace FSM
{
    class Persistent
    {
        private static string lastUsedFilename="";
        static public string LastFilename
        {
            get { return lastUsedFilename; }
            set { lastUsedFilename = value; }
        }
        static public void Save(string filename, Config config)
        {
            lastUsedFilename = filename;
            using (Stream stream = new FileStream(filename, FileMode.Create, FileAccess.Write))
            {
                IFormatter formatter = new BinaryFormatter();
                formatter.Serialize(stream, config);
                formatter.Serialize(stream, StateCollection.GetStates().ToArray());
                formatter.Serialize(stream, EventCollection.GetEvents().ToArray());
            }
        }

        static public Config Load(string filename)
        {
            lastUsedFilename = filename;
            StateCollection.Clear();
            EventCollection.Clear();
            IFormatter formatter = new BinaryFormatter();
            using (Stream stream = File.OpenRead(filename))
            {
                Config config = (Config)formatter.Deserialize(stream);

                State[] states = (State[])formatter.Deserialize(stream);
                Event[] events = (Event[])formatter.Deserialize(stream);

                return config;
            }            
        }
    }
}
