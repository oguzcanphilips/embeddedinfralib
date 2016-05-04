using System.Collections.Generic;
using System.Linq;

namespace InterfaceCodeGen
{
  class Function
  {
    private byte mId;
    private readonly string mRet;
    private readonly string mName;
    private readonly List<Param> mParams = new List<Param>();

    public bool HasReturn
    {
        get 
        { 
            if (!mRet.Equals("void")) return true;
            return mParams.Any(param => param.Direction != Param.Dir.In);
        }
    }
    public bool ReturnVoid { get { return mRet.Equals(("void")); } }
    public string Return { get { return mRet; } }
    public string Name { get { return mName; } }
    public byte Id { set { mId = value; } get { return mId; } }
    public List<Param> Params { get { return mParams; } }
    
    public Function(string ret, string name, byte id)
    {
      mRet = ret;
      mName = name;
      mId = id;
    }
    public void Add(Param p)
    {
      mParams.Add(p);
    }

    public HashSet<string> GetDependencies()
    {
        HashSet<string> res = new HashSet<string>();
        foreach (var param in mParams)
        {
            res.Add(param.Type);
        }
        res.Add(mRet);
        return res;
    }
  }
}
