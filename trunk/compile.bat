call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
cl /EHsc /Ox /errorReport:prompt /WX "%~dp0lltcpc\lltcpc.cpp" 
pause