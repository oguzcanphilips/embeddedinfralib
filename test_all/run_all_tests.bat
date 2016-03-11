@echo off

for %%v in ("*_test.exe") do (
	call :ExecuteTest "%%~v"
)
echo Executed tests:
echo %RunList%
if "%ErrorList%" == "" (
	echo No Errors.
	Exit /B 0
) else (
	echo Errors:
	echo %ErrorList%
	Exit /B 1
)

:ExecuteTest
@echo off
"%~1"
if NOT %ERRORLEVEL% == 0 (
	set ErrorList=%ErrorList% %~1
)
set RunList=%RunList% %~1


