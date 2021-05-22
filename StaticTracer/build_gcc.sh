#! /bin/bash

real_path=$(realpath $0)
dir_name=`dirname "${real_path}"`
echo "real_path: ${real_path}, dir_name: ${dir_name}"

new_dir_name=${dir_name}/build_sh

if [[ -e ${new_dir_name} ]]; then
    echo "rm build_sh dir"
    rm -rf build_sh
fi

mkdir -p ${new_dir_name}
cd ${new_dir_name}

test_name=main
test_type=.cpp
g++ -g -rdynamic -c ../src/MemoryAllocationWrap.cpp ../src/StackTracerManagement.cpp ../src/TracerSignal.cpp ../src/FileManagement.cpp ../test/${test_name}${test_type} -std=c++11
g++ -g -rdynamic -o ${test_name} ${test_name}.o MemoryAllocationWrap.o TracerSignal.o StackTracerManagement.o FileManagement.o -O2 -pthread -Wall -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=foo -Wl,--wrap=_Znwm -Wl,--wrap=_ZdlPv -std=c++11

./${test_name}
cd -

#g++ -g -rdynamic -o test_boost test_boost.cpp -lboost_stacktrace_addr2line -ldl -DBOOST_STACKTRACE_USE_ADDR2LINE
