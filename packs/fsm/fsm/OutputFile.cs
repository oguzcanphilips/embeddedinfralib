using System;
using System.IO;

namespace FSM
{
    sealed class OutputFile : StreamWriter
    {
        static private string mRoot = "./";
        private readonly string mName;
        private readonly string mNameTmp;

        static public void SetRoot(string root)
        {
            mRoot = root + "/";
            Directory.CreateDirectory(mRoot);
        }
        static public string GetRoot()
        {
            return mRoot;
        }
        public OutputFile(string name)
            : base(mRoot + name + ".tmp", false)
        {
            mName = mRoot + name;
            mNameTmp = mName + ".tmp";
            WriteLine("/***************************************************************************");
            WriteLine();
            WriteLine("  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             ");
            WriteLine();
            WriteLine("  Tool     : " + AppDomain.CurrentDomain.FriendlyName);
            WriteLine("  Author   : Patrick van Kaam");
            WriteLine();
            WriteLine("****************************************************************************");
            WriteLine("  Copyright (c) 2014 Koninklijke Philips N.V.,");
            WriteLine("  All Rights Reserved.");
            WriteLine("***************************************************************************/");
            WriteLine();
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            try
            {
                string oldContent = File.ReadAllText(mName);
                string newContent = File.ReadAllText(mNameTmp);
                if (oldContent.Equals(newContent))
                {
                    File.Delete(mNameTmp);
                    return;
                }
            }
            catch
            {
            }
            Console.WriteLine("Updated file : " + mName);
            try
            {
                File.Delete(mName);
            }
            catch
            {
            }
            File.Delete(mName);
            File.Move(mNameTmp, mName);
        }
    }
}
