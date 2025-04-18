@echo off

pushd %~dp0

rem Checks if cl is defined. If not call vcvars64.bat.
where /q cl
if errorlevel 1 (
    call "run_vcvar.bat"
)

call "copy_file_from_path.bat" clang_rt.asan_dynamic-x86_64.dll

pushd ..

mkdir Bin
copy DLL\* Bin\

setlocal enabledelayedexpansion

IF "%~1"=="" (
    set outputTarget=""
) ELSE (
    set outputTarget=> scripts/logfile.txt 2>&1
)

rem /O2
@set CC=cl
@set FLAGS=/std:c++17 /D_CRT_SECURE_NO_WARNINGS /fsanitize=address /DSDL_MAIN_HANDLED /Z7 /W4 /WX /MP /EHsc /I "./Include"
@set "SRC_FILES="
rem set "SRC_FILES=!SRC_FILES! "Src/main.cpp""
for /r "Src" %%I in (*.c) do (
    set "SRC_FILES=!SRC_FILES! "%%~fI""
)
for /r "Src" %%I in (*.cpp) do (
    set "SRC_FILES=!SRC_FILES! "%%~fI""
)

for /r "Include" %%I in (*.c) do (
    set "SRC_FILES=!SRC_FILES! "%%~fI""
)
for /r "Include" %%I in (*.cpp) do (
    set "SRC_FILES=!SRC_FILES! "%%~fI""
)

rem vcperf /start SessionName

%CC% %FLAGS% %SRC_FILES% /MP /link /out:bin/game.exe /LIBPATH:"./Lib" Shell32.lib %outputTarget%

rem vcperf /stop SessionName /timetrace outputFile.json

del /S *.obj 1>nul 2>nul
del /S vc140.pdb 1>nul 2>nul

endlocal

popd
popd