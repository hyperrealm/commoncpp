
set target=%1\expat-2.0.1

mkdir %target%
mkdir %target%\bin
mkdir %target%\lib
mkdir %target%\include

copy /y lib\expat.h %target%\include
copy /y lib\expat_external.h %target%\include
copy /y win32\bin\release\expat.dll %target%\bin
copy /y win32\bin\debug\expat_d.dll %target%\bin
copy /y win32\bin\release\expat.lib %target%\lib
copy /y win32\bin\debug\expat_d.lib %target%\lib
