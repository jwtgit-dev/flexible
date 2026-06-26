#!/bin/bash
echo "============================================"
echo " Building Flexible Timetabling System"
echo "============================================"
echo ""

if command -v cmake &> /dev/null; then
    echo "CMake found, building with CMake..."
    mkdir -p build
    cd build
    cmake ..
    cmake --build . --config Release
    cd ..
elif command -v g++ &> /dev/null; then
    echo "Building with g++..."
    g++ -std=c++17 -O2 -I include \
        src/main.cpp \
        src/models/TimeSlot.cpp \
        src/models/Classroom.cpp \
        src/models/Course.cpp \
        src/models/ClassGroup.cpp \
        src/models/Teacher.cpp \
        src/models/ScheduledClass.cpp \
        src/models/TimetableData.cpp \
        src/constraints/ConstraintManager.cpp \
        src/constraints/HardConstraints.cpp \
        src/constraints/SoftConstraints.cpp \
        src/algorithm/Chromosome.cpp \
        src/algorithm/NSGAII.cpp \
        src/engine/TimetableEngine.cpp \
        src/config/ConfigManager.cpp \
        src/interpreter/ResultInterpreter.cpp \
        src/utils/SampleDataGenerator.cpp \
        -o timetabling -lpthread
else
    echo ""
    echo "ERROR: Neither CMake nor g++ found."
    echo "Please install build-essential or cmake."
    exit 1
fi

echo ""
echo "Build complete!"
echo "Run: ./timetabling"
