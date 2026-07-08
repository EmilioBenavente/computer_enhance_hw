@echo off

set COMMON_CMD_FLAGS=-nologo -MTd -Z7 -FC
set COMMON_LINKER_FLAGS=/link gdi32.lib user32.lib winmm.lib shell32.lib -INCREMENTAL:NO

if not exist ..\..\build mkdir ..\..\build

pushd ..\..\build

del /S /Q *

cl %COMMON_CMD_FLAGS% ..\computer_enhance\code\win32_enhance.c  %COMMON_LINKER_FLAGS%

popd
