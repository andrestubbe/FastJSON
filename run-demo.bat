@echo off
echo [FastJSON] Running Demo (via JitPack)...
cd examples\00-basic-usage
call mvn compile exec:java -Dexec.mainClass=Example
cd ..\..
pause
