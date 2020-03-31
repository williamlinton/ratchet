@echo off

cls

mkdir build
pushd build
cd
cl -Zi ..\src\parser.c shlwapi.lib /link /subsystem:console
popd
