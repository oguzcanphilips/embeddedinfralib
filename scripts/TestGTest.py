from BuildUtils import ExecuteCommand

def TestGTest(testExecutable, resultsFile):
    ExecuteCommand([ testExecutable, "--gtest_output=xml:" + resultsFile ])

