import os
from BuildUtils import ExecuteCommand
from BuildUtils import PrintHeader
from BuildVisualStudio import BuildVsSolution

def GetMicroTestsProjectDir():
    scriptDir = os.getcwd()
    dir = os.path.abspath(os.path.join(scriptDir, "..\\..\\EmbeddedReuse-Build-VisualStudio"))
    return dir

def ConfigureCcolaProjects():
    PrintHeader("Configuring Ccola Embedded Reuse Platform Projects")
    scriptDir = os.getcwd()
    repoRootDir = os.path.abspath(os.path.join(scriptDir, "..\\"))
    os.chdir(repoRootDir)
    ExecuteCommand(["ConfigVisualStudio.cmd"])
    #ExecuteCommand(["ConfigSTM32F407.cmd"])
    os.chdir(scriptDir)

def BuildFirmware():
    scriptDir = os.getcwd()
    dir = os.path.abspath(os.path.join(scriptDir, "..\\"))
    os.chdir(dir)
    ExecuteCommand(["BuildSTM32F407.cmd"])
    os.chdir(scriptDir)

def BuildEmbeddedReuse_CcolaProjects():
    PrintHeader("Building Embedded Reuse Ccola Projects")
    dir = GetMicroTestsProjectDir()
    ExecuteCommand(["cmake","--build",dir,"--config","Debug"])

def RunMicroTests_CcolaProjects():
    dir = GetMicroTestsProjectDir()
    ExecuteCommand(["cmake","--build",dir,"--config","Debug", "--target", "RUN_TESTS"])

if __name__ == '__main__':
    PrintHeader("Building Embedded Reuse Platform")
    ConfigureCcolaProjects()
    BuildEmbeddedReuse_CcolaProjects()
    RunMicroTests_CcolaProjects()
      