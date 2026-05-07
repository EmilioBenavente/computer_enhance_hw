@echo off

set COMMON_CMD_FLAGS=-Z7
set COMMON_LINKER_FLAGS=gdi32.lib

if not exist ..\..\build mkdir ..\..\build

pushd ..\..\build

cl %COMMON_CMD_FLAGS% ..\computer_enhance\code\win32_app.c %COMMON_LINKER_FLAGS%

popd
