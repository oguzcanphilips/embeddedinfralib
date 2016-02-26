import os
from os import path
from BuildUtils import *
from BuildVisualStudio import BuildVsSolution
from TestGTest import *

def RunMicroTests():
    PrintHeader("Running Embedded Reuse Platform Microtests")
    scriptDir = os.getcwd()
    pathToTestExecutables = os.path.abspath(os.path.join(scriptDir, "..\\Release"))
    
    microTests = []
    for test in glob.glob(pathToTestExecutables + "\\*test*.exe"):
        microTests.append({ "exeFile": test, "resultFile": (test + ".xml") })
    
    for microTest in microTests:
         TestGTest(microTest["exeFile"], microTest["resultFile"])


def BuildEmbeddedReuse_VisualStudio():
    PrintHeader("Building Embedded Reuse Visual Studio Solution")
    scriptDir = os.getcwd()
    project = os.path.abspath(os.path.join(scriptDir, "..\\EmbeddedReuse.sln"))
    BuildVsSolution(project, "Debug")
    BuildVsSolution(project, "Release")

if __name__ == '__main__':
    PrintHeader("Building Embedded Reuse Platform")
    BuildEmbeddedReuse_VisualStudio()
    RunMicroTests()