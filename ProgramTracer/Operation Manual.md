# Process-Memory-Tracker




## Program Memory Tracker

### Structure

```
ProgramTracer
├── CMakeLists.txt
├── include
│   ├── FileManagement.h
│   ├── MemoryAllocationWrap.h
│   ├── StackTracerManagement.h
│   └── TracerSignal.h
├── Operation Manual.md
├── src
│   ├── FileManagement.cpp
│   ├── MemoryAllocationWrap.cpp
│   ├── StackTracerManagement.cpp
│   └── TracerSignal.cpp
├── test
│   └── SimpleTest.cpp
└── tracerConfig.h.in
```

- `CMakeLists`: cmake config file
- `include`: Header folder of project **Program Memory Tracker**
- `src`: Source folder of project **Program Memory Tracker**
- `test`: Test folder of project **Program Memory Tracker**
- `tracerConfig.h.in`: Configure a header file to pass some of the CMake settings.

### Set Up

```shell
# Current path is "Process-Memory-Tracker/ProgramTracer"
mkdir build
cd build
cmake ..
make

# Execute test file.
./ProgramTracer
```



### Configuration Parameter

Configure parameters in `CMakeLists` file.

```shell
# Set the output file location of the memory leak results; if you want the output to be in the console, then set(PATH \"\")
set(PATH \"leakInfo\")	

# Set whether it is DEBUG mode. In DEBUG mode, you can see the function call information. true is open DEBUG, false is close DEBUG.
set(DEBUG_BUILD true)

# Set up the test file: test/DockingTest.cpp is the test file.
add_executable(${PROJECT_NAME} test/DockingTest.cpp ${SRC_FILES} ${project_HEADERS})
```
