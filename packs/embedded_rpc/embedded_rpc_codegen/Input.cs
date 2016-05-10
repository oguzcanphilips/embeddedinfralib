using System;
using System.IO;
using System.Text.RegularExpressions;


namespace codegen
{
  class Input
  {
    private string mInput;
    private string mPath;
    public Input(string filename)
    {
        mPath = Path.GetDirectoryName(filename);
        if (mPath.Equals("")) mPath = Directory.GetCurrentDirectory();
        Console.WriteLine("Input : " + mPath  + "\\"+Path.GetFileName(filename));
        string input = File.ReadAllText(filename);

        input = input.Replace("\r", "");
        input = (new Regex(@"#.*\n")).Replace(input, "\n");
        input = input.Replace("#", "");
        input = input.Replace("\n", " ");
        input = input.Replace("\t", " ");
        mInput = (new Regex(@"#.*\n")).Replace(input, "\n");

        mInput = mInput.Replace("=", " = ").Replace(",", " , ").Replace("(", " ( ").Replace(")", " ) ") + " ";
    }

    public String GetNext()
    {
        mInput = mInput.TrimStart(' ');
        int space = mInput.IndexOf(" ");
        if (space < 0) return "";
        string res = mInput.Substring(0, space);
        mInput = mInput.Substring(space+1);
        if(res.Equals("include"))
        {
            string filename = mPath + "\\" + GetNext();
            Input inc = new Input(filename);
            mInput = inc.mInput + " " + mInput;
            return GetNext();
        }
        return res; 
    }

    public bool Empty
    {
        get { return mInput.Length == 0; }
    }
  }
}
