@echo off

mkdir .\compiled\
pushd .\compiled\
cl -FAsc -Zi ..\rolamade.cpp user32.lib gdi32.lib kernel32.lib xinput.lib dsound.lib
popd
