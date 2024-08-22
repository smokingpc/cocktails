cmake_minimum_required(VERSION 3.9)
project(cpp_test CUDA CXX)
find_package(CUDA 11.8 REQUIRED)

set(CMAKE_CUDA_ARCHITECTURES 75)
set(CMAKE_CXX_STANDARD 17)
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "-ccbin 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\Llvm\\bin\\clang-cl.exe'")
#add_compile_options(-std=c++23)
#unset(CMAKE_CXX_STANDARD)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_WIN32_WINNT=0xA00 /D_CRT_SECURE_NO_WARNINGS /D_AMD64_")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_WIN32_WINNT=0xA00 /D_CRT_SECURE_NO_WARNINGS /D_AMD64_ /std:clatest")
#set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS}")
add_executable(cpp_test main.cc test.cu)
#set_property(TARGET cppstd_test 
#             PROPERTY CUDA_SEPARABLE_COMPILATION ON)
#target_compile_options(cpp_test PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-ccbin="C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\Llvm\\bin\\clang.exe">) 
#target_compile_options(cpp_test PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-ccbin clang>) 
