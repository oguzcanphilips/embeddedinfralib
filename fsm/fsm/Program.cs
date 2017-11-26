using System;
using System.IO;
using System.Windows.Forms;

namespace FSM
{
    static class Program
    {
        static void StartWindowsApp(Config config = null)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FSM(config));
        }

        static void StartCommandLine(Config config, string outputPath, bool generateC, bool generateCpp)
        {
            OutputFile.SetRoot(outputPath);
            string err = ModelChecker.CheckModel(config);
            if (err.Length > 0) throw new Exception(err);
            if (generateCpp) new GeneratorCPP(config);
            if (generateC) new GeneratorC(config);
        }
        static void StartAllInDirectory(string root, string outputPath, bool generateC, bool generateCpp)
        {
            string[] dirs = Directory.GetDirectories(root);
            foreach (var dir in dirs)
            {
                StartAllInDirectory(dir, outputPath, generateC, generateCpp);
            }
            string[] files = Directory.GetFiles(root);
            foreach (var file in files)
            {
                try
                {
                    if (file.EndsWith(".fsm"))
                    {
                        Console.WriteLine("Process : " + file);
                        StartCommandLine(Persistent.Load(file), root + "/" + outputPath, generateC, generateCpp);
                    }
                }
                catch (Exception)
                {
                    Console.WriteLine("Invalid file : " + file);
                }
            }
        }

        // FSM.exe [input-file] [outputPath] [C/CPP]
        // FSM.exe [input-dir] [outputPath wrt input-dir] [C/CPP]
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                if (args.Length == 0)
                {
                    StartWindowsApp();
                }
                else if (args.Length < 2)
                {
                    StartWindowsApp(Persistent.Load(args[0]));
                }
                else
                {
                    bool generateC = true;
                    bool generateCpp = true;
                    if (args.Length == 3)
                    {
                        if (args[2].ToUpper().Equals("C")) generateCpp = false;
                        if (args[2].ToUpper().Equals("CPP")) generateC = false;
                    }

                    if (!Directory.Exists(args[0]))
                    {
                        try
                        {
                            StartCommandLine(Persistent.Load(args[0]), args[1], generateC, generateCpp);
                        }
                        catch (Exception)
                        {
                            Console.WriteLine("Invalid file : " + args[0]);
                        }
                    }
                    else
                    {
                        StartAllInDirectory(args[0], args[1], generateC, generateCpp);
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }
    }
}
