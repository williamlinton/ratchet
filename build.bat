@echo off

cls

mkdir build
pushd build
cd
cl -Zi /Fe"parser.exe" ..\src\win32_main.c shlwapi.lib /link /subsystem:console
popd
