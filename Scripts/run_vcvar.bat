@echo off

rem Notes:
rem Only VS2019 and VS2022 has been tested and works.
rem You cannot have more that one install of a VS version. 
rem Such as two installs of VS2022.

pushd %~dp0

for /f "tokens=*" %%a in ('VCVarsFinder.exe') do set filepath=%%a

echo Executing: "%filepath%"

call "%filepath%"

popd