@echo off
chcp 65001 >nul
cd /d "%~dp0"
echo ⚡ Building Main Project (FastJSON)...
call mvn clean install -DskipTests -q
if %ERRORLEVEL% NEQ 0 ( echo Main build failed. & exit /b %ERRORLEVEL% )
echo 🚀 Launching FastJSON Demo...
cd examples\00-basic-usage
call mvn compile exec:java -Dexec.mainClass=Example -q
cd ..\..
