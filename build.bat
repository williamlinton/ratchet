@echo off

cls

mkdir build
pushd build
cd
cl -Zi ..\src\parser.c /link /subsystem:console
popd
