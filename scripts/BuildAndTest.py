import os
from os import path
from BuildUtils import *
from BuildVisualStudio import BuildVsSolution
from TestGTest import *

def GetMicroTestsProjectDir():
    scriptDir = os.getcwd()
    dir = os.path.abspath(os.path.join(scriptDir, "..\\..\\EmbeddedReuse-Build-VisualStudio"))
    return dir

def RunMicroTests():
    scriptDir = os.getcwd()
    pathToTestExecutables = os.path.abspath(os.path.join(scriptDir, "..\\Release"))
    microTests = [ { "exeFile": "infra_util_test.exe",       "resultFile": "infra_util_test.xml" }]
    
    for microTest in microTests:
         TestGTest(path.join(pathToTestExecutables, microTest["exeFile"]), microTest["resultFile"])


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