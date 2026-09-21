@echo off
REM Builds the program and the tests with g++ (MinGW) on Windows.

g++ -std=c++17 -Wall -Wextra -o grab_fare.exe src/main.cpp src/fare_calculator.cpp
if errorlevel 1 goto failed

g++ -std=c++17 -Wall -Wextra -Isrc -o run_tests.exe tests/test_fare_calculator.cpp src/fare_calculator.cpp
if errorlevel 1 goto failed

echo Build successful.
echo   Run the program: grab_fare.exe
echo   Run the tests:   run_tests.exe
goto end

:failed
echo Build failed.

:end
