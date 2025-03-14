@echo off
gcc main.c -o scheduler
if errorlevel 1 (
    echo Compilation failed. Check your code for errors.
    pause
    exit /b
)
scheduler
pause
