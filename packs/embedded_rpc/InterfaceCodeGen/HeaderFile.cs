
namespace InterfaceCodeGen
{
    sealed class HeaderFile : OutputFile
    {
        private readonly string tag;

        public HeaderFile(string name)
        : base(name)
        {
            tag = name.ToUpper().Replace(".", "_");
            tag = tag.Substring(tag.LastIndexOf("/")+1);

            WriteLine(@"#ifndef " + tag);
            WriteLine(@"#define " + tag);
            WriteLine();
        }

        protected override void Dispose(bool disposing)
        {
            WriteLine();
            WriteLine("#endif /*"+ tag +"*/");
            base.Dispose(disposing);
        }
    }
}
