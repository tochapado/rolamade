@echo off

mkdir .\compiled\
pushd .\compiled\
cl -FC -Zi ..\rolamade.cpp user32.lib gdi32.lib kernel32.lib
popd
