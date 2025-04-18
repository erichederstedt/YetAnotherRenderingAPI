@echo off
setlocal enabledelayedexpansion
pushd %~dp0
pushd ..

set "FILENAME=%1"
set "DESTDIR=Bin\"
REM Replace semicolons with newlines for safe iteration
for %%D in ("%PATH:;=" "%") do (
    set "DIR=%%~D"
    if exist "!DIR!\%FILENAME%" (
        echo Found: !DIR!\%FILENAME%
        copy "!DIR!\%FILENAME%" "%DESTDIR%"
        goto :done
    )
)

echo clang_rt.asan_dynamic-x86_64.dll not found in any PATH directories.
:done

popd
popd
endlocal