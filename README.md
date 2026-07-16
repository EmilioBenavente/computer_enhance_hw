This repo is a recording of my homework assignment
for the first section on Computer Enhance.

Windows:
To run on Windows you will need MSVC to compile.
If running on the command line make sure to run VCVARSALL.bat x64
To get an executable simply go into \handmade_hero\code\
  and run win32_build.bat

Linux:
To run on Linux you will need gcc to compile.
To get an executable simply go into ./handmade_hero/code/
  and run linux_build.sh

Linux && Wine:
If you have wine installed, in a WindowsVM/Windows system, run win32_build.bat
Then with wine run
  wine /path/to/build/win32_enhance.exe
