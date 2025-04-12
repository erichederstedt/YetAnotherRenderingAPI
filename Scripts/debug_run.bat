@echo off

pushd %~dp0
pushd ..

bin\metapts.exe -s Src -f

popd
popd