from os import path
from BuildUtils import *


def GetFullVsToolPath(tool):
    vs = "Microsoft Visual Studio 12.0\\Common7\\IDE"

    programFilesDir32 = path.expandvars("%ProgramFiles(x86)%")
    programFilesDir64 = path.expandvars("%ProgramFiles%")
    
    devenv32 = path.join(programFilesDir32, vs, tool)
    devenv64 = path.join(programFilesDir64, vs, tool)
    
    devenv = None
    if(path.exists(devenv32)):
        devenv = devenv32
    if(path.exists(devenv64)):
        devenv = devenv64

    if devenv is None:
        Fail("Path " + devenv32 + " and path " + devenv64 + " don't exist.", __file__)

    return  devenv
    

def BuildVsSolution(solutionFile, configuration):
    PrintNow("Building Visual Studio solution: " + solutionFile)
    ExecuteCommand([ GetFullVsToolPath("devenv.com"),
                     solutionFile,
                     "/Rebuild",
                     configuration ])