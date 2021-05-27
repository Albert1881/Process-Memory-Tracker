# Process-Memory-Tracker




# Program Memory Tracker

## Structure

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
│   └── main.cpp
└── tracerConfig.h.in
```

### Set up

```shell
# Current path is "Process-Memory-Tracker/ProgramTracer"
mkdir build
cd build
cmake ..
make
./ProgramTracer
```

