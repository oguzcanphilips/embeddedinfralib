using System;

namespace codegen
{
  class Program
  {
    static int Main(string[] args)
    {
        if (args.Length == 0)
        {
            Console.WriteLine(AppDomain.CurrentDomain.FriendlyName + " interface-file [output-path]");
            return 1;
        }
        if (args.Length >= 2)
        {
            OutputFile.SetRoot(args[1]);
        }
        Input input = new Input(args[0]);
        try
        {
            Parser p = new Parser(input);
        }
        catch (Exception e)
        {
            Console.WriteLine("Error : " + e.Message);
            return 1;
        }
        return 0;
    }
  }
}
