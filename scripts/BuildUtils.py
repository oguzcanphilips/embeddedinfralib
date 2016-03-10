import os, subprocess, sys, glob, itertools, shutil, AsyncFileReader, time
try:
    from termcolor import cprint
    cPrintAvailable = True
except ImportError:
    cPrintAvailable = False

def NormalizeNewlines(string):
    import re
    return re.sub(r'(\r\n|\r|\n)', '\n', string)

def SurroundWithQuotes(unquoted):
    if not isinstance(unquoted, str):
        Fail(unquoted + " must be a string.", __file__)
    
    return '"%s"' % unquoted

def PrefixLines(multiLineString, prefix=""):
    out = ""
    for line in iter(multiLineString.splitlines()):
        out += "%s%s\n" %(prefix, line)
    return out

def Print(text, color=None, on_color=None, attrs=None, **kwargs):
    if cPrintAvailable:
        cprint(text, color=color, on_color=on_color, attrs=attrs, **kwargs)
    else:
        print(text)

def PrintNow(text):
    print(text)
    sys.stdout.flush()

def PrintHeader(headerStr):
    dashes = '-' * len(headerStr);
    Print("\n----" +   dashes  + "----"
          "\n--- " + headerStr + " ---"
          "\n----" +   dashes  + "----", "blue", attrs = [ "bold" ])

def PrintSubHeader(subHeaderStr):
    Print("\n* %s\n" %(subHeaderStr), "blue")

def PrintError(errorStr):
    Print(errorStr, "red", attrs = [ "bold" ])

def PrintSuccess(text):
    Print("\n%s\n" %(text), "green", attrs = [ "bold" ])

def PrintLineWithPrefix(prefix, line, color=None):
    Print(prefix, end="", attrs=[ "bold" ], color=color)
    Print("%s" %(NormalizeNewlines(line)), end="", color=color)

def GetBuildEnvironmentVariable(variableName):
    # TODO: hardcoded here for now; these should become environment variables
    if variableName == "MAC_HOSTNAME_FOR_IOS_DEPLOYMENT":
        return "ORLANDO-MAC-3.DDNS.HTC.NL.PHILIPS.COM"
    elif variableName == "ECLIPSE_PATH":
        return "C:\\ARMEmbeddedTools\\eclipse-kepler-SR2-win32-x86_64\\"
    elif variableName == "ARTIFACT_DEPLOYMENT_LOCATION":
        return "C:/MoonshineBuildServer"
    elif variableName == "SSH_PRIVATE_KEY_FILE":
        ssh_key_file_in_artifacts = os.path.join(GetBuildEnvironmentVariable("WORKSPACE"), "id_rsa_new")
        return ssh_key_file_in_artifacts if os.path.isfile(ssh_key_file_in_artifacts) else "c:/MoonshineBuildServer/id_rsa_new"

    try:
        return os.environ[variableName]
    except KeyError:
        Fail("Can't find environment variable with name %s" %(variableName), __file__)

def GlobMultipleFiles(directory, patterns):
    return list(itertools.chain.from_iterable(glob.glob(os.path.join(directory, pattern)) for pattern in patterns))

def CreateDirIfNotExists(directory):
    if not os.path.exists(directory):
        os.makedirs(directory, 511)

def CopyFile(sourcePath, targetPath):
    CreateDirIfNotExists(os.path.dirname(targetPath))
    print("  %s -> %s" %(sourcePath, targetPath))
    shutil.copy(sourcePath, targetPath)

def CopyFiles(files, targetDirectory):
    CreateDirIfNotExists(targetDirectory)
    for file in files:
        CopyFile(file, os.path.join(targetDirectory, os.path.basename(file)))

def CopyTree(sourcePath, targetPath, ignorePatterns):
    print("  %s -> %s" %(sourcePath, targetPath))
    shutil.copytree(sourcePath, targetPath,ignore=shutil.ignore_patterns(*ignorePatterns))

def KillProcess(processName):
    if sys.platform == 'win32':
        DEVNULL = open(os.devnull, "w")
        p = subprocess.Popen("taskkill /F /IM %s" %(processName), stderr = DEVNULL)
        p.wait()
        DEVNULL.close()
    else:
        Fail("Can't kill process '%s'; KillProcess is not implemented for platform '%s'" %(processName, sys.platform), __file__)

def Fail(reason, file):
    errorStr = ("Failure:\n%s"
                "In file:\n  %s" %(PrefixLines(reason, "  "), file))
    PrintError(errorStr)
    sys.exit(1)

def ReplaceInFile(inFile, replacements, outFile=None):
    _outFile = outFile if outFile else inFile + "_"

    with open(_outFile, "wt") as fout:
        with open(inFile, "rt") as fin:
            lineNumber = 0
            for line in fin:
                lineNumber += 1
                for replacement in replacements:
                    if line.find(replacement["from"]) != -1:
                        PrintLineWithPrefix("[ %s ] " % os.path.basename(inFile),
                                            "Replacing '%s' by '%s' (line nr = %d)\n" % (replacement["from"],
                                                                                         replacement["to"],
                                                                                         lineNumber))
                        line = line.replace(replacement["from"], replacement["to"])

                fout.write(line)

    if not outFile:
        shutil.copyfile(_outFile, inFile)
        os.remove(_outFile)

''' ExecuteCommand
    -------------- '''

def __StartFileReaders(filesToRead):
    for fileToRead in filesToRead:
        if "fileName" in fileToRead:
            fileToRead["fileReader"] = AsyncFileReader.AsyncFileReader.fromFileName(fileToRead["fileName"])
        else:
            fileToRead["fileReader"] = AsyncFileReader.AsyncFileReader(fileToRead["fd"])

def __PrintFileReaderOutputWhileProcessIsRunning(process, filesToRead):
    while process.poll() == None:
        __PrintRemainingFileReaderOutput(filesToRead)
        time.sleep(0.1)

def __PrintRemainingFileReaderOutput(filesToRead):
    for fileToRead in filesToRead:
        while not fileToRead["fileReader"].queue.empty():
            PrintLineWithPrefix("  [ %s - %s ] " %(time.strftime("%H:%M:%S"), fileToRead["prefix"]), fileToRead["fileReader"].queue.get())

def __StopFileReaders(filesToRead):
    for fileToRead in filesToRead:
        fileToRead["fileReader"].stop()

def __ExecuteCommand(args, errorString, filesToTail):
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=1)
    filesToRead = [ { "fd": process.stdout, "prefix": "stdout", "fileReader": None },
                    { "fd": process.stderr, "prefix": "stderr", "fileReader": None }, ]
    for fileToTail in filesToTail:
        filesToRead.append({ "fileName": fileToTail,  "prefix": os.path.basename(fileToTail) })

    __StartFileReaders(filesToRead)
    __PrintFileReaderOutputWhileProcessIsRunning(process, filesToRead)
    process.communicate()
    __StopFileReaders(filesToRead)
    __PrintRemainingFileReaderOutput(filesToRead)

    errorStringFound = False
    if errorString and ((errorString in filesToRead[0]["fileReader"].readLines) or
                        (errorString in filesToRead[1]["fileReader"].readLines)):
        errorStringFound = True

    return process.returncode, errorStringFound

def ExecuteCommand(args, errorString=None, retryCount=0, failInCaseOfError=True, filesToTail = []):
    Print("--------------------------")
    Print("Executing command (%sretryCount = '%d'):" %("errorString = '%s', " %(errorString) if errorString else "",
                                                       retryCount), attrs=[ "bold" ])
    Print("  %s" %(args[0]))
    Print("with arguments:", attrs=[ "bold" ])
    Print("  %s" %("\n  ".join(args[1:])))
    Print("--------------------------")

    for i in range(retryCount + 1):
        (returncode, errorStringFound) = __ExecuteCommand(args, errorString, filesToTail)
        success = (returncode == 0) and not errorStringFound

        if success or not failInCaseOfError:
            return returncode
        else:
            if returncode != 0:
                failString = "Command failed with code: '%s'" %(str(returncode))
            else:
                failString = "Command failed because '%s' was found in either stdout or stderr" %(errorString)
        
            if (i == retryCount):
                Fail(failString, __file__)
            else:
                Print("\n%s; retrying (%d / %d)\n" %(failString, i + 1, retryCount), color="red")