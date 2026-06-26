@echo off
echo ============================================
echo  Building Flexible Timetabling System
echo ============================================
echo.

where cmake >nul 2>nul
if %errorlevel% equ 0 (
    echo CMake found, building with CMake...
    if not exist build mkdir build
    cd build
    cmake ..
    if %errorlevel% equ 0 (
        cmake --build . --config Release
    )
    cd ..
) else (
    echo CMake not found, trying g++...
    where g++ >nul 2>nul
    if %errorlevel% equ 0 (
        echo Building with g++...
        g++ -std=c++17 -O2 -I include ^
            src/main.cpp ^
            src/models/TimeSlot.cpp ^
            src/models/Classroom.cpp ^
            src/models/Course.cpp ^
            src/models/ClassGroup.cpp ^
            src/models/Teacher.cpp ^
            src/models/ScheduledClass.cpp ^
            src/models/TimetableData.cpp ^
            src/constraints/ConstraintManager.cpp ^
            src/constraints/HardConstraints.cpp ^
            src/constraints/SoftConstraints.cpp ^
            src/algorithm/Chromosome.cpp ^
            src/algorithm/NSGAII.cpp ^
            src/engine/TimetableEngine.cpp ^
            src/config/ConfigManager.cpp ^
            src/interpreter/ResultInterpreter.cpp ^
            src/utils/SampleDataGenerator.cpp ^
            -o timetabling.exe
    ) else (
        echo.
        echo ERROR: Neither CMake nor g++ found.
        echo Please install MinGW or Visual Studio with CMake.
        echo.
        echo Option 1: Install MinGW-w64 (g++)
        echo Option 2: Install Visual Studio with C++ support
        echo Option 3: Install CMake + a C++17 compiler
        pause
        exit /b 1
    )
)

echo.
echo Build complete!
echo Run: timetabling.exe
echo.
pause
